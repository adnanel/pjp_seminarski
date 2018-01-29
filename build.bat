@echo off

echo Building %1...


del b.exe
del Parser.cpp
del Scanner.cpp
del Scanner.h
del Parser.h

del example.asm
del example.obj


cococpp b.atg -frames "coco-cpp/src"

g++  main.cpp Scanner.cpp Parser.cpp -o b
b %1.b>%1.asm

nasm -f win32 %1.asm
gcc -o %1 %1.obj driver.c asm_io.obj

%1
echo %1 Exit Code is %errorlevel%