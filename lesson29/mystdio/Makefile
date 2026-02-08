target=libmyc.so
src=$(wildcard *.c)
obj=$(src:.c=.o)
cc=gcc 

$(target):$(obj)
	$(cc) -shared -o $@ $^
%.o:%.c
	$(cc) -fPIC -c $<

.PHONY:output
output:
	@mkdir -p myc/lib 
	@mkdir -p myc/include
	@cp *.h myc/include
	@cp *.so myc/lib 
	@tar czf myc.tgz myc 

.PHONY:clean
clean:
	rm -rf *.o $(target) myc myc.tgz 

debug:
	@echo $(target)
	@echo $(src)
	@echo $(obj)
