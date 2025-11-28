Bin=code.exe         # 定义变量 
#Src=$(shell ls *.c) # 做法1 -- 采用shell命令行方式,获取当前所有.c文件名
Src=$(wildcard *.c)  # 做法2 -- 使用 wildcard 函数,获取当前所有.c文件名
Obj=$(Src:.c=.o)     # 将Src的所有同名 .c 替换成 .o 形成目标文件列表
Echo=echo           
cc=gcc 
Rm=rm -f
Flags=-c -Wall       # 编译选项
LD_Flags=-o          # 链接选项

$(Bin):$(Obj)
	@$(Echo) "我要开始链接了...$(Obj) -> $(Bin)"    # $@: 代表目标文件名    $^: 代表依赖文件列表
	@$(cc) $(LD_Flags) $@ $^                        
%.o:%.c                                             # %.c: 展开当前目录下的所有.c  %.o: 同时展开同名.o
	@$(Echo) "我要开始编译了...$< -> $@"            # @: 不回显命令
	@$(cc) $(Flags) $<                              # %<: 对展开的依赖.c文件，一个个的交给gcc
.PHONY:clean
clean:
	$(Rm) $(Obj) $(Bin)                             # $(RM): 替换,用变量内容替换它

.PHONY:debug
debug:
	@$(Echo) "Bin: $(Bin)"
	@$(Echo) "Obj: $(Obj)"
	@$(Echo) "Src: $(Src)"
