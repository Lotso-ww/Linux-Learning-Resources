Bin=process_bar
Cc=gcc
Src=$(wildcard *.c)
Obj=$(Src:.c=.o)

$(Bin):$(Obj)
	@echo "$^ link to $@"
	@$(Cc) -o $@ $^
%.o:%.c
	@echo "compiling $< to $@"
	@$(Cc) -c $<

.PHONY:clean
clean:
	@echo "File cleanup complete"
	@rm -f $(Obj) $(Bin)

.PHONY:debug
debug:
	@echo "Bin: $(Bin)"
	@echo "Cc:  $(Cc)"
	@echo "Src: $(Src)"
	@echo "Obj: $(Obj)"

