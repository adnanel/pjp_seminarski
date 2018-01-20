%include "asm_io.inc"
segment .data;

_fun_len db 0
_bmain_len db 4

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
 %define i dword ebp - 4
 mov [ebp - 4], dword 100
_tmp_label_1:
 mov eax, [i]
 cmp eax, dword 0
 jg _tmp_label_3
 mov eax, dword 0
 jmp _tmp_label_4
 _tmp_label_3:
 mov eax, dword 1
 _tmp_label_4:
 jz _tmp_label_2
 jmp _tmp_label_1
_tmp_label_2:
_bmain_end:
 MOV ESP,EBP
 POP EBP
 RET 

;
;	(B 
;		(GVARDEF a )
;	 
;		(FUNCDEF fun 
;			(FHEADER )
;		 
;			(BLOCK 
;				(RETURNPARAM 
;					(INT 1)
;				)
;			)
;		 )
;	 
;		(FUNCDEF bmain 
;			(FHEADER )
;		 
;			(BLOCK 
;				(DECLSTAT 
;					(LVARDEF i 
;						(INIT 100)
;					)
;				 
;					(WHILE 
;						(GREATERTHAN 
;							(VAR i)
;						
;							(ONUMBER 0)
;						)
;					 
;						(BLOCK )
;					 )
;				)
;			 )
;		 )
;	 )
;