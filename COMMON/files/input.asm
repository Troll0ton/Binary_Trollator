push 4
pop rax

push 1
pop rbx

call 13:

push rbx
out

hlt

:13

push rax
push rbx
mul
pop rbx

push rax
push 1
sub
pop rax

push 1
push rax
jae 5: 

pop
pop
call 13:

jmp 7:

:5

pop
pop

:7
ret