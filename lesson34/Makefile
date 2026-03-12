all:Reader Writer
Reader:Reader.cc
	g++ -o $@ $^ -std=c++11
Writer:Writer.cc
	g++ -o $@ $^ -std=c++11

.Phony:clean
clean:
	rm -f Reader Writer