push 100
pop rbx

:3
push rbx
push 1
sub
pop rbx

push rbx
push 0
jb 4:
pop
pop

push 40000
pop rax

:1
push rax
push 1
sub
pop rax

push rax
push 0
jb 2:

pop
pop
jmp 1:

:2
pop
pop

jmp 3:

:4
pop
pop

push rbx
out

hlt