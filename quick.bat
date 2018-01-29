nasm -f win32 example.asm
gcc -o example example.obj driver.c asm_io.obj
example
echo %1 Exit Code is %errorlevel%
