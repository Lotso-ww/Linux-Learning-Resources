.PHONY: all clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: DictServer DictClient

DictServer: DictServer.cpp UdpServer.hpp logger.hpp Mutex.hpp
	$(CXX) $(CXXFLAGS) -o DictServer DictServer.cpp

DictClient: DictClient.cpp
	$(CXX) $(CXXFLAGS) -o DictClient DictClient.cpp

clean:
	rm -f DictServer DictClient