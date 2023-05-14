# Trollanguage
My own language

## Overview
This work is the final project of the last semester, which includes most of the previous work.
It is an assembly from the compiler and the processor, which was made earlier.
The compiler is able to convert code written in MY programming language into assembler code, which is subsequently executed by MY processor.
To compile and execute YOUR program, you need to put it in a text file. (input.txt)
~~~
Trollanguage/frontend/files/input.txt
~~~

## Execute the programm:
~~~
COMPILATION - compile.bat (frontend.exe, backend.exe)
EXECUTION - proc.bat (asm.exe, cpu.exe)
~~~

## Comand line arguments

-absent-

## Supported code:
-simple operations: +, -, *, /, ^, sin, cos, ln, tg, sqrt;
-variable (ATTENTION!!! This version of project supports only single letter variables: a, x and others)
-assignment operator: 
~~~
x=100+56*y^3
~~~
-conditional operator: 
~~~
if(x+1221) y=56
~~~
-conditional operator with else: 
~~~
                                  if(x+1221) y=56
                                  else y=89
~~~                                  
-cycles: 
~~~
while(x) x=x-1
~~~
-comparison operators: >, <, ~ (this is ==):
~~~
if((x)>(20)) y=78
~~~
-"body" structure:   
 ~~~
                     if(x+1221){
                     z=y+90
                     m=78
                     }
~~~
~~~
                     while(u){
                     u=u+1
                     y=y+89
                     } 
~~~
-function initialization: 
~~~
                           funct a (){
                           t=32
                           z=989
                           m (this means, that this function RETURNS this value)
                           }
~~~

-function using: 
~~~
x=f()+78
~~~

-print and scan functions:
~~~
scan(x)
print(y+909)
~~~

-main function is a:
~~~
funct a (){
...
}
~~~

## Expression tree storage format (Trollanguage/COMMON/files/tree.txt)
~~~
{220
{0a}
{221
{3}
{217
{217
{217
{212
{3}
{0a}
}
{216
{0b}
{11}
}
}
{219
{213
{0a}
{10}
}
{217
{216
{0b}
{22
{0b}
{0a}
}
}
{216
{0a}
{21
{0a}
{11}
}
}
}
}
}
{211
{3}
{0b}
}
}
}
}
~~~
Storing notes in a text file is as follows:
-the first digit means its type,
-the rest are the value of the cell.
There are 4 different types of notes in total:
0-VAR, 1-NUM, 2-OP, 3-NUL (null note). If the note format is VAR, then the variable's ASCII code follows. If the format of the note is NUM, then the numeric value follows.
If the note format is OP, then the opcode follows.



### Unit tests
All supported operations are shown in the examples.
~~~
Trollanguage/frontend/files/...
~~~

## Documentation

-absent-

**Troll0ton corporation** 
