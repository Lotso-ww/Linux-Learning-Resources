#include "httplib.h"

int main() {
    httplib::Server svr;
    
    // 1. 托管静态资源：将 ./static 目录下的文件作为静态资源服务
    //    访问 http://localhost:8080/index.html 会返回 ./static/index.html
    svr.set_mount_point("/", "../wwwroot");
    
    // 2. 提供搜索接口（回调路由）
    //    访问 http://localhost:8080/search?q=关键字
    svr.Get("/search", [](const httplib::Request& req, httplib::Response& res) {
        // 获取查询参数 q
        // std::string query = req.get_param_value("q");
        
        // if (query.empty()) {
        //     res.set_content(R"({"error": "请提供搜索关键字 q"})", "application/json");
        //     return;
        // }
        
        // // 模拟搜索结果（实际可从数据库查询）
        // std::string results = R"([
        //     {"title": "关于 )" + query + R"( 的第一条结果", "url": "/page1"},
        //     {"title": "关于 )" + query + R"( 的第二条结果", "url": "/page2"},
        //     {"title": "关于 )" + query + R"( 的第三条结果", "url": "/page3"}
        // ])";
        
        // res.set_content(results, "application/json");

        res.set_content("hello result", "text/plain");
    });
    
    // 启动服务
    std::cout << "Server started at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}