%include "asm_io.inc"
segment .data;

; `"GRESKA!!! %s. Ocekivao vrijednost %d, dobio %d  "`
_tmp_label_5   db 71
   db 82
   db 69
   db 83
   db 75
   db 65
   db 33
   db 33
   db 33
   db 32
   db 37
   db 115
   db 46
   db 32
   db 79
   db 99
   db 101
   db 107
   db 105
   db 118
   db 97
   db 111
   db 32
   db 118
   db 114
   db 105
   db 106
   db 101
   db 100
   db 110
   db 111
   db 115
   db 116
   db 32
   db 37
   db 100
   db 44
   db 32
   db 100
   db 111
   db 98
   db 105
   db 111
   db 32
   db 37
   db 100
   db 32
   db 32
   db 0 

_assert_len EQU 0
; `"Element niza nije postavljen na ispravnu vrijednost?"`
_tmp_label_11   db 69
   db 108
   db 101
   db 109
   db 101
   db 110
   db 116
   db 32
   db 110
   db 105
   db 122
   db 97
   db 32
   db 110
   db 105
   db 106
   db 101
   db 32
   db 112
   db 111
   db 115
   db 116
   db 97
   db 118
   db 108
   db 106
   db 101
   db 110
   db 32
   db 110
   db 97
   db 32
   db 105
   db 115
   db 112
   db 114
   db 97
   db 118
   db 110
   db 117
   db 32
   db 118
   db 114
   db 105
   db 106
   db 101
   db 100
   db 110
   db 111
   db 115
   db 116
   db 63
   db 0 

; `"A na kraju petlje nije -1?"`
_tmp_label_13   db 65
   db 32
   db 110
   db 97
   db 32
   db 107
   db 114
   db 97
   db 106
   db 117
   db 32
   db 112
   db 101
   db 116
   db 108
   db 106
   db 101
   db 32
   db 110
   db 105
   db 106
   db 101
   db 32
   db 45
   db 49
   db 63
   db 0 

_bmain_len EQU 48

segment .bss;

a resd 4
garr resd 40

segment .text;

; Built-in functions
extern  _scanf, _printf, _getchar, _putchar, _fputs

 GLOBAL _assert

_assert:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_assert_len
 %define _assert_expected ebp+8
 %define _assert_actual ebp+12
 %define _assert_poruka ebp+16
 mov eax, [_assert_expected]
 cmp eax, [_assert_actual]
 jne _tmp_label_2
 mov eax, dword 0
 jmp _tmp_label_3
 _tmp_label_2:
 mov eax, dword 1
 _tmp_label_3:
 jz _tmp_label_1
 sub esp, dword _tmp_label_4
 lea eax, [_tmp_label_5]
 mov [esp + 0], eax 
 LEA EBX, [_assert_poruka]
 MOV EAX, [EBX]
 mov [esp + 4], eax 
 LEA EBX, [_assert_expected]
 MOV EAX, [EBX]
 mov [esp + 8], eax 
 LEA EBX, [_assert_actual]
 MOV EAX, [EBX]
 mov [esp + 12], eax 
 _tmp_label_4 equ 16
 call _printf
_tmp_label_1:
_assert_end:
 MOV ESP,EBP
 POP EBP
 RET 12
 GLOBAL _bmain

_bmain:
 PUSH EBP
 MOV EBP,ESP
 SUB ESP,_bmain_len
 %define _bmain_broj ebp-4
 mov [ebp - 4], dword 5
 %define _bmain_niz ebp-44
 MOV DWORD [a],10
_tmp_label_6:
 lea ebx, [a]
 mov eax, [ebx]
 mov edx, eax
 dec eax
 mov [ebx], eax
 mov eax, edx
 cmp eax, dword 0
 jg _tmp_label_8
 mov eax, dword 0
 jmp _tmp_label_9
 _tmp_label_8:
 mov eax, dword 1
 _tmp_label_9:
 jz _tmp_label_7
 LEA EBX, [_bmain_broj]
 MOV EAX, [EBX]
 ADD EAX,[a]
 PUSH EAX       ; push expr result
 lea eax, [_bmain_niz]
 push eax
 LEA EBX, [a]
 MOV EAX, [EBX]
 sal eax, 2
 mov ebx, eax
 pop eax
 add eax, ebx
 mov ebx, eax
 mov eax, [eax]
 POP EAX        ; pop expr result 
 MOV [EBX], EAX
 sub esp, dword _tmp_label_10
 LEA EBX, [_bmain_broj]
 MOV EAX, [EBX]
 ADD EAX,[a]
 mov [esp + 0], eax 
 lea eax, [_bmain_niz]
 push eax
 LEA EBX, [a]
 MOV EAX, [EBX]
 sal eax, 2
 mov ebx, eax
 pop eax
 add eax, ebx
 mov ebx, eax
 mov eax, [eax]
 mov [esp + 4], eax 
 lea eax, [_tmp_label_11]
 mov [esp + 8], eax 
 _tmp_label_10 equ 12
 call _assert
 jmp _tmp_label_6
_tmp_label_7:
 sub esp, dword _tmp_label_12
 MOV EAX, dword 1
 neg eax
 mov [ebx], eax
 mov [esp + 0], eax 
 LEA EBX, [a]
 MOV EAX, [EBX]
 mov [esp + 4], eax 
 lea eax, [_tmp_label_13]
 mov [esp + 8], eax 
 _tmp_label_12 equ 12
 call _assert
 %define _bmain_b ebp-48
 LEA EBX, [a]
 MOV EAX, [EBX]
 mov eax, dword [a]
 jmp _bmain_end
 lea ebx, [a]
 mov eax, [ebx]
 inc eax
 mov [ebx], eax
 lea ebx, [a]
 mov eax, [ebx]
 mov edx, eax
 inc eax
 mov [ebx], eax
 mov eax, edx
 lea ebx, [a]
 mov eax, [ebx]
 dec eax
 mov [ebx], eax
 lea ebx, [a]
 mov eax, [ebx]
 mov edx, eax
 dec eax
 mov [ebx], eax
 mov eax, edx
 xor eax, eax
 mov [a], EAX
 mov eax, [a]
 cmp eax, dword 1
 lea ebx, [a]
 sete [ebx]
 mov eax, [a]
 cmp eax, dword 1
 lea ebx, [a]
 setne [ebx]
 mov eax, [a]
 cmp eax, dword 1
 lea ebx, [a]
 setle [ebx]
 mov eax, [a]
 cmp eax, dword 1
 lea ebx, [a]
 setge [ebx]
; opmov 
 lea ebx, [a]
 mov eax, [ebx]
 push ebx
 and eax, dword 1
 pop ebx
 mov [ebx], eax
; end of opmov
; opmov 
 lea ebx, [a]
 mov eax, [ebx]
 push ebx
 or eax, dword 1
 pop ebx
 mov [ebx], eax
; end of opmov
; opmov 
 lea ebx, [a]
 mov eax, [ebx]
 push ebx
 cdq
 mov eax, dword 1
 idiv eax
 pop ebx
 mov [ebx], eax
; end of opmov
; opmov 
 lea ebx, [a]
 mov eax, [ebx]
 push ebx
 imul eax, dword 1
 pop ebx
 mov [ebx], eax
; end of opmov
_bmain_end:
 MOV ESP,EBP
 POP EBP
 RET 0

;
;	(B 
;		(GVARDEF a )
;	 
;		(GARRDEF garr 
;			(ASIZE 10)
;		)
;	 
;		(FUNCDEF assert 
;			(FHEADER 
;				(FPARAM expected )
;			
;				(FPARAM actual )
;			
;				(FPARAM poruka )
;			)
;		 
;			(BLOCK 
;				(IF 
;					(NEQU 
;						(VAR expected)
;					
;						(VAR actual)
;					)
;				 
;					(BLOCK 
;						(FUNCCALL 
;							(VAR printf)
;						
;							(ARG 
;								(STRING `"GRESKA!!! %s. Ocekivao vrijednost %d, dobio %d  "`)
;							)
;						 
;							(ARG 
;								(VAR poruka)
;							)
;						 
;							(ARG 
;								(VAR expected)
;							)
;						 
;							(ARG 
;								(VAR actual)
;							)
;						 )
;					 )
;				 )
;			)
;		 )
;	 
;		(FUNCDEF bmain 
;			(FHEADER )
;		 
;			(BLOCK 
;				(DECLSTAT 
;					(EXTRN a )
;				
;					(LVARDEF broj 
;						(INIT 5)
;					)
;				 
;					(LARRDEF niz 
;						(ASIZE10)
;					)
;				 
;					(MOV 
;						(VAR a)
;					
;						(INT 10)
;					)
;				 )
;			 
;				(WHILE 
;					(GREATERTHAN 
;						(POSTDEC 
;							(VAR a)
;						)
;					 
;						(ONUMBER 0)
;					)
;				 
;					(BLOCK 
;						(MOV 
;							(INDEX 
;								(VAR niz)
;							
;								(VAR a)
;							)
;						 
;							(ADD 
;								(VAR broj)
;							
;								(VAR a)
;							)
;						 )
;					 
;						(FUNCCALL 
;							(VAR assert)
;						
;							(ARG 
;								(ADD 
;									(VAR broj)
;								
;									(VAR a)
;								)
;							 )
;						 
;							(ARG 
;								(INDEX 
;									(VAR niz)
;								
;									(VAR a)
;								)
;							 )
;						 
;							(ARG 
;								(STRING `"Element niza nije postavljen na ispravnu vrijednost?"`)
;							)
;						 )
;					 )
;				 )
;			
;				(FUNCCALL 
;					(VAR assert)
;				
;					(ARG 
;						(UMINUS 
;							(INT 1)
;						)
;					)
;				 
;					(ARG 
;						(VAR a)
;					)
;				 
;					(ARG 
;						(STRING `"A na kraju petlje nije -1?"`)
;					)
;				 )
;			 
;				(DECLSTAT 
;					(LVARDEF b )
;				 
;					(VAR a)
;				)
;			 
;				(RETURNPARAM 
;					(VAR a)
;				)
;			
;				(PREINC 
;					(VAR a)
;				)
;			
;				(POSTINC 
;					(VAR a)
;				)
;			 
;				(PREDEC 
;					(VAR a)
;				)
;			
;				(POSTDEC 
;					(VAR a)
;				)
;			 
;				(MOV 
;					(VAR a)
;				
;					(ONUMBER 0)
;				)
;			 
;				(ISEQUMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(ISNEQUMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(ISLESSEQUMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(ISGREATEREQUMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(ANDMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(ORMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(DIVMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 
;				(MULTMOV 
;					(VAR a)
;				
;					(INT 1)
;				)
;			 )
;		 )
;	 )
;