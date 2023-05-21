push 200
push 100
jb 14:

push 100
push 200
add
out

push 100
push 50
sub
out

push 23
push 4
div
out

push 23
push 4

:78

div
out

:14

call 66:

push 23
out

pop rax
pop rax

hlt

:66
push 1005
out
ret