CC = g++
CXXFLAGS = -std=c++17 -Wall

TARGETS = TcpEchoServer TcpEchoClient

all: $(TARGETS)

TcpEchoServer: TcpEchoServer.cpp TcpEchoServer.hpp Logger.hpp Mutex.hpp
	$(CC) $(CXXFLAGS) -o $@ TcpEchoServer.cpp

TcpEchoClient: TcpEchoClient.cpp
	$(CC) $(CXXFLAGS) -o $@ TcpEchoClient.cpp

clean:
	rm -f $(TARGETS)

.PHONY: all clean