%include "asm_io.inc"
segment .data;

_fun_len db 0
_bmain_len db 8
_abmain_len db 8
_subr_len db 8

segment .bss;

a resd 1

segment .text;

 GLOBAL _fun

_fun:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_fun_len
 mov eax, dword 1
 jmp _fun_end
_fun_end:
 MOV ESP,EBP
 POP EBP
 RET 
 GLOBAL _bmain

_bmain:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_bmain_len
TODO - LARRDEF
 %define i dword ebp - 0
TODO - INIT
 %define j dword ebp - 4
 MOV DWORD [a],5
; add mov 
 add [a], dword 3
 mov eax, [a]
; add mov 
 MOV EAX, [a]
 ADD EAX,1
 add [a], eax
 mov eax, [a]
; add mov 
 add [a], [a]
 mov eax, [a]
; isequ mov 
 cmp [a], [a]
 sete [a]
 mov eax, [a]
 jmp label_label
 mov eax, dword [a]
 jmp _bmain_end
label_label:
 mov eax, dword 1
 jmp _bmain_end
_bmain_end:
 MOV ESP,EBP
 POP EBP
 RET 
 GLOBAL _abmain

_abmain:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_abmain_len
 %define b ebp + 8
 %define a dword ebp - 0
 %define d dword ebp - 4
 MOV EAX,2
 ADD EAX,[b]
 MOV [a],EAX
_abmain_end:
 MOV ESP,EBP
 POP EBP
 RET 
 GLOBAL _subr

_subr:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_subr_len
 %define c ebp + 8
 %define c dword ebp - 0
 %define hahj dword ebp - 4
 MOV EAX, [a]
 MOV [c],EAX
_subr_end:
 MOV ESP,EBP
 POP EBP
 RET 

;(B (GVARDEF a ) (FUNCDEF fun (FHEADER ) (BLOCK (RETURNPARAM (INT 1))) ) (FUNCDEF bmain (FHEADER ) (BLOCK (DECLSTAT (LARRDEF arr (ASIZE 10)) (LVARDEF i (INIT 5)) (LVARDEF j ) (MOV (VAR a)(INT 5)) ) (ADDMOV (VAR a)(INT 3)) (ADDMOV (VAR a)(ADD (VAR a)(INT 1)) ) (ADDMOV (VAR a)(VAR a)) (ISEQUMOV (VAR a)(VAR a)) (GOTO label)(RETURNPARAM (VAR a))(DECLSTAT (LABEL label )(RETURNPARAM (INT 1))) ) ) (FUNCDEF abmain (FHEADER (FPARAM b )) (BLOCK (DECLSTAT (LVARDEF a ) (LVARDEF d ) (MOV (VAR a)(ADD (INT 2)(VAR b)) ) ) ) ) (FUNCDEF subr (FHEADER (FPARAM c )) (BLOCK (DECLSTAT (LVARDEF c ) (LVARDEF hahj ) (EXTRN a )(MOV (VAR c)(VAR a)) ) ) ) )
