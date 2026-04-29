all:chat_client ChatServer

chat_client:ChatClient.cpp
	g++ -o $@ $^ -std=c++17 -static 
ChatServer:ChatMain.cpp
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f chat_client ChatServer