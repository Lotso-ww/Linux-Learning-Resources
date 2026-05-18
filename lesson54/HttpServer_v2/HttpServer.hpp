#ifndef __HTTPSERVER__HPP
#define __HTTPSERVER__HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "TcpServer.hpp"
#include "Logger.hpp"
#include "HttpProtocol.hpp"

using namespace LogModule;
using route_t = std::function<void(const HttpRequest &req, HttpResponse &resp)>;

class HttpServer
{
public:
    HttpServer(uint16_t port)
        : _port(port)
        , _tsvr(std::make_unique<TcpServer>(port))
    {}

    std::string HandlerHttpRequest(std::string &streamstr)
    {
        // 1. 检查报文完整性 -- 我们今天默认报文是完整的, 这里就不处理了
        // 2. 对收到的请求进行反序列化
        HttpRequest httpreq;
        httpreq.Deserialize(streamstr);

        std::cout << "method: " << httpreq["method"]<< std::endl;
        std::cout << "path: " << httpreq["path"]<< std::endl;
        std::cout << "args: " << httpreq["args"]<< std::endl;

               // 3. httpreq -> httpresp
        HttpResponse httpresp;
        // 我们想测试重定向的话 -- 下面我们404页面的方法2其实也测试到了
        // httpresp.SetCode(302);
        // httpresp.SetHeader("Location", "https://www.qq.com/");
        
        // 处理动态资源
        if (IsNeedRoute(httpreq["path"])) 
        {
            _route[httpreq["path"]](httpreq, httpresp);
        } 
        else 
        {
          // 处理静态资源
          std::string filecontent = GetFileContentHelper(httpreq["path"]);
          std::string suffix = httpreq["suffix"];
          if (filecontent.empty()) {
            // 如果为空, 我们加上404页面
            // 方法一
            // std::string page404 = "wwwroot/404.html";
            // httpresp.SetCode(404);
            // std::string file404 = GetFileContentHelper(page404);
            // suffix = ".html";
            // httpresp.SetHeader("Content-Length", file404.size());
            // httpresp.SetHeader("Content-Type", Suffix2Type(suffix));
            // httpresp.SetBody(file404);

            // 方法二: 重定向
            httpresp.SetCode(301); // 试试 301 也可以
            httpresp.SetHeader("Location", "/404.html");
          } 
          else 
          {
            httpresp.SetCode(200);
            httpresp.SetHeader("Content-Length", filecontent.size());
            httpresp.SetHeader("Content-Type", Suffix2Type(suffix));
            httpresp.SetHeader("Connection", "close"); // 应答告诉浏览器我是短链接
            httpresp.SetBody(filecontent);
          }
        }

        // 4. 应答进行序列化
        std::string httprespstr;
        httpresp.Serialize(&httprespstr); // 带出来

        // 5. 返回
        return httprespstr;
    }

    void Run()
    {
        _tsvr->Run([this](std::string &streamstr){
            return this->HandlerHttpRequest(streamstr);
        });
    }

    void Register(std::string uri, route_t handler)
    {
        std::string key = webroot + uri;
        _route[key] = handler;
    }
    ~HttpServer()
    {}
private:
    bool IsNeedRoute(const std::string &key)
    {
        return _route.find(key) != _route.end();
    }
    std::string GetFileContentHelper(const std::string &fileurl)
    {
        std::ifstream in(fileurl);
        if(!in.is_open())
        {
            return std::string();
        }
        // 获取文件长度
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg);

        // 把文件内容全部读进Content
        std::string content;
        content.resize(filesize);
        in.read(content.data(), filesize);  // 用data,cpp17
        in.close();

        return content;
    }

    // suffix: .html
    // return: text/html
    std::string Suffix2Type(const std::string &suffix)
    {
        // 使用静态映射表提高效率，避免每次调用都重新创建
        static const std::unordered_map<std::string, std::string> mime_map = {
            // 文本类型
            {".html", "text/html"},
            {".htm", "text/html"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".mjs", "application/javascript"},
            {".json", "application/json"},
            {".xml", "application/xml"},
            {".txt", "text/plain"},
            {".csv", "text/csv"},
            {".md", "text/markdown"},

            // 图片类型
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".bmp", "image/bmp"},
            {".webp", "image/webp"},
            {".svg", "image/svg+xml"},
            {".ico", "image/x-icon"},
            {".tiff", "image/tiff"},
            {".tif", "image/tiff"},

            // 字体类型
            {".woff", "font/woff"},
            {".woff2", "font/woff2"},
            {".ttf", "font/ttf"},
            {".otf", "font/otf"},

            // 视频类型
            {".mp4", "video/mp4"},
            {".webm", "video/webm"},
            {".ogv", "video/ogg"},
            {".avi", "video/x-msvideo"},
            {".mov", "video/quicktime"},
            {".mpeg", "video/mpeg"},
            {".mpg", "video/mpeg"},

            // 音频类型
            {".mp3", "audio/mpeg"},
            {".wav", "audio/wav"},
            {".ogg", "audio/ogg"},
            {".flac", "audio/flac"},
            {".m4a", "audio/mp4"},
            {".aac", "audio/aac"},

            // 应用类型
            {".pdf", "application/pdf"},
            {".zip", "application/zip"},
            {".rar", "application/vnd.rar"},
            {".7z", "application/x-7z-compressed"},
            {".tar", "application/x-tar"},
            {".gz", "application/gzip"},
            {".exe", "application/vnd.microsoft.portable-executable"},
            {".dll", "application/x-msdownload"},
            {".msi", "application/x-msi"},
            {".doc", "application/msword"},
            {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {".xls", "application/vnd.ms-excel"},
            {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            {".ppt", "application/vnd.ms-powerpoint"},
            {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
            {".wasm", "application/wasm"},

            // 其他常见类型
            {".jsonld", "application/ld+json"},
            {".rss", "application/rss+xml"},
            {".atom", "application/atom+xml"},
            {".manifest", "text/cache-manifest"},
            {".map", "application/json"}, // source maps
            {".ts", "video/mp2t"},        // MPEG transport stream
            {".m3u8", "application/vnd.apple.mpegurl"}
        };

        auto it = mime_map.find(suffix);
        if (it != mime_map.end())
        {
            return it->second;
        }

        // 默认返回二进制流，或根据需求返回其他默认值
        return "application/octet-stream";
    }
private:
    uint16_t _port;
    std::unique_ptr<TcpServer> _tsvr;
    std::unordered_map<std::string, route_t> _route; // 注册服务
};


#endif