all:Reader Writer
Reader:Reader.cpp
	g++ -o $@ $^
Writer:Writer.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f Reader Writer