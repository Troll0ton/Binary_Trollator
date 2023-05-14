call 97:
jmp 2:
:97
in
pop [97]
in
pop [98]
in
pop [99]
call 98:
pop [68]
push [68]
push 0
ja 5:
push 0
jmp 6:
:5
push 1
:6
push 0
je 3:
call 99:
jmp 4:
:3
push [68]
push 0
je 9:
push 0
jmp 10:
:9
push 1
:10
push 0
je 7:
call 100:
jmp 8:
:7
push [68]
push 0
jb 13:
push 0
jmp 14:
:13
push 1
:14
push 0
je 11:
call 101:
jmp 12:
:11
push 123456
out
:12
:8
:4
ret
:2
jmp 15:
:98
push [98]
push [98]
mul
push 4
push [97]
mul
push [99]
mul
sub
ret
:15
jmp 16:
:99
push 0
push [98]
sub
push [68]
sqrt
add
push 2
push [97]
mul
div
pop [120]
push 0
push [98]
sub
push [68]
sqrt
sub
push 2
push [97]
mul
div
pop [121]
push [120]
out
push [121]
out
ret
:16
jmp 17:
:100
push 0
push [98]
sub
push 2
push [97]
mul
div
pop [120]
push [120]
out
ret
:17
jmp 18:
:101
push 123456
out
ret
:18
hlt
