
code.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <run>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	bf 00 00 00 00       	mov    $0x0,%edi
   9:	e8 00 00 00 00       	callq  e <run+0xe>
   e:	5d                   	pop    %rbp
   f:	c3                   	retq   
