CC=g++
CFLAGS=-std=c++11 -pthread
TARGET=block_queue_test

all: $(TARGET)

$(TARGET): main.cc BlockQueue.hpp
	$(CC) $(CFLAGS) main.cc -o $(TARGET)

clean:
	rm -f $(TARGET)
