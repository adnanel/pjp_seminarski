%include "asm_io.inc"
segment .data;

; `"%d"`
_tmp_label_2   db 37
   db 100
   db 0 

; `"Unijeli ste broj %d"`
_tmp_label_4   db 85
   db 110
   db 105
   db 106
   db 101
   db 108
   db 105
   db 32
   db 115
   db 116
   db 101
   db 32
   db 98
   db 114
   db 111
   db 106
   db 32
   db 37
   db 100
   db 0 

_bmain_len EQU 4

segment .bss;


segment .text;

; Built-in functions
extern  _scanf, _printf, _getchar, _putchar, _fputs

 GLOBAL _bmain

_bmain:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_bmain_len
 %define _bmain_a ebp-4
 sub esp, dword _tmp_label_1
 lea eax, [_tmp_label_2]
 mov [esp + 0], eax 
 lea eax, [_bmain_a]
 mov [esp + 4], eax 
 _tmp_label_1 equ 8
 call _scanf
 sub esp, dword _tmp_label_3
 lea eax, [_tmp_label_4]
 mov [esp + 0], eax 
 LEA EBX, [_bmain_a]
 MOV EAX, [EBX]
 mov [esp + 4], eax 
 _tmp_label_3 equ 8
 call _printf
_bmain_end:
 MOV ESP,EBP
 POP EBP
 RET 0