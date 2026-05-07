CC = g++
CXXFLAGS = -std=c++17 -Wall

TARGETS = ssh_server ssh_client

all: $(TARGETS)

ssh_server: TcpServer.cpp
	$(CC) $(CXXFLAGS) -o $@ TcpServer.cpp

ssh_client: TcpEchoClient.cpp
	$(CC) $(CXXFLAGS) -o $@ TcpEchoClient.cpp -static

clean:
	rm -f $(TARGETS)

.PHONY: all clean