CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS = -lpthread

TARGET = main
SRC = main.cc

$(TARGET): $(SRC) Thread.hpp
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)