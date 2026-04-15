TARGET := main
CXX := g++
CXXFLAGS := -O -Wall -std=c++17
SRCS := Main.cc
OBJS := $(SRCS:.cc=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
