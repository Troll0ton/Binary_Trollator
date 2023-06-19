push 1
push -2
push 1

pop rcx
pop rbx
pop rax

call 13:

hlt

:13

push rbx
push rbx
mul
push 4
push rax
push rcx
mul
mul
sub
pop rdx

push rbx
push -1
mul
push rdx 
sqrt
sub
push 2
push rax
mul
div
out

push rbx
push -1
mul
push rdx 
sqrt
add
push 2
push rax
mul
div
out

ret
