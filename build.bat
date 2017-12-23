@echo off

echo Building %1...


del b.exe
del Parser.cpp
del Scanner.cpp
del Scanner.h
del Parser.h

cococpp b.atg -frames "coco-cpp/src"

"C:\Program Files (x86)\mingw-w64\i686-7.2.0-posix-dwarf-rt_v5-rev1\mingw32\bin\g++" -I "C:\Users\adnan\Desktop\pjp" main.cpp Scanner.cpp Parser.cpp -o b
b %1.b>%1.asm

nasm -f win32 %1.asm
"C:\Program Files (x86)\mingw-w64\i686-7.2.0-posix-dwarf-rt_v5-rev1\mingw32\bin\g++" -o %1 %1.obj driver.c asm_io.obj

