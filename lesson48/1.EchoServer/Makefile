CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGETS = udpEchoServer udpEchoClient

all: $(TARGETS)

udpEchoServer: UdpEchoServer.cpp
	$(CXX) $(CXXFLAGS) -o udpEchoServer UdpEchoServer.cpp

udpEchoClient: UdpEchoClient.cpp
	$(CXX) $(CXXFLAGS) -o udpEchoClient UdpEchoClient.cpp -static

clean:
	rm -f $(TARGETS)

.PHONY: all clean