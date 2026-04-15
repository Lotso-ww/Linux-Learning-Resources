CC = g++
TARGET = main
SRC = Main.cc
OBJS = $(SRC:.cc=.o)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.cc
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
