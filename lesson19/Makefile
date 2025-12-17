myproc:myproc.o
	gcc -o $@ $^
myproc.o:myproc.c
	gcc -c $< -o $@

.PHONY:clean
clean:
	rm -f myproc.o myproc
