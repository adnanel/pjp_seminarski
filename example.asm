%include "asm_io.inc"
segment .data;

_fun_len db 0
_bmain_len db 4
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
 %define _bmain_i dword ebp - 4
 mov [ebp - 4], dword 100
_tmp_label_1:
 mov eax, [_bmain_i]
 cmp eax, dword 25
 jg _tmp_label_3
 mov eax, dword 0
 jmp _tmp_label_4
 _tmp_label_3:
 mov eax, dword 1
 _tmp_label_4:
 jz _tmp_label_2
 lea ebx, [_bmain_i]
 mov eax, [ebx]
 dec eax
 mov [ebx], eax
 jmp _tmp_label_1
_tmp_label_2:
 mov eax, dword 53
 MOV [_bmain_i],EAX
 jmp label_label
 mov eax, dword [a]
 jmp _bmain_end
label_label:
 mov eax, dword [_bmain_i]
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
 %define _abmain_a dword ebp - 4
 %define _abmain_d dword ebp - 8
 MOV EAX,2
 ADD EAX,[b]
 MOV [_abmain_a],EAX
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
 %define _subr_c dword ebp - 4
 %define _subr_hahj dword ebp - 8
 MOV EAX, [a]
 MOV [_subr_c],EAX
 mov eax, [_subr_c]
 cmp eax, [a]
 jl _tmp_label_7
 mov eax, dword 0
 jmp _tmp_label_8
 _tmp_label_7:
 mov eax, dword 1
 _tmp_label_8:
 jz _tmp_label_6
 MOV DWORD [_subr_c],1
 jmp _tmp_label_5
_tmp_label_6:
 xor eax, eax
 mov [_subr_c], EAX
_tmp_label_5:
 lea eax, [a]
 MOV [_subr_c],EAX
 mov eax, [_subr_c]
 cmp eax, [a]
 jl _tmp_label_10
 mov eax, dword 0
 jmp _tmp_label_11
 _tmp_label_10:
 mov eax, dword 1
 _tmp_label_11:
 jz _tmp_label_9
 MOV DWORD [_subr_c],10
_tmp_label_9:
_tmp_label_12:
 mov eax, [_subr_c]
 cmp eax, [a]
 jl _tmp_label_14
 mov eax, dword 0
 jmp _tmp_label_15
 _tmp_label_14:
 mov eax, dword 1
 _tmp_label_15:
 jz _tmp_label_13
 MOV EAX, [c]
 ADD EAX,1
 MOV [_subr_c],EAX
 jmp _tmp_label_12
_tmp_label_13:
_subr_end:
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
;							(INT 25)
;						)
;					 
;						(BLOCK 
;							(PREDEC 
;								(VAR i)
;							)
;						)
;					 )
;				)
;			 
;				(MOV 
;					(VAR i)
;				
;					(CHAR `'5'`)
;				)
;			 
;				(GOTO label)
;			
;				(RETURNPARAM 
;					(VAR a)
;				)
;			
;				(DECLSTAT 
;					(LABEL label )
;				
;					(RETURNPARAM 
;						(VAR i)
;					)
;				)
;			 )
;		 )
;	 
;		(FUNCDEF abmain 
;			(FHEADER 
;				(FPARAM b )
;			)
;		 
;			(BLOCK 
;				(DECLSTAT 
;					(LVARDEF a )
;				 
;					(LVARDEF d )
;				 
;					(MOV 
;						(VAR a)
;					
;						(ADD 
;							(INT 2)
;						
;							(VAR b)
;						)
;					 )
;				 )
;			 )
;		 )
;	 
;		(FUNCDEF subr 
;			(FHEADER 
;				(FPARAM c )
;			)
;		 
;			(BLOCK 
;				(DECLSTAT 
;					(LVARDEF c )
;				 
;					(LVARDEF hahj )
;				 
;					(EXTRN a )
;				
;					(MOV 
;						(VAR c)
;					
;						(VAR a)
;					)
;				 )
;			 
;				(IFELSE 
;					(LESSTHAN 
;						(VAR c)
;					
;						(VAR a)
;					)
;				 
;					(BLOCK 
;						(MOV 
;							(VAR c)
;						
;							(INT 1)
;						)
;					 )
;				 
;					(BLOCK 
;						(MOV 
;							(VAR c)
;						
;							(ONUMBER 0)
;						)
;					 )
;				 )
;			
;				(MOV 
;					(VAR c)
;				
;					(ADDROF 
;						(VAR a)
;					)
;				)
;			 
;				(IF 
;					(LESSTHAN 
;						(VAR c)
;					
;						(VAR a)
;					)
;				 
;					(BLOCK 
;						(MOV 
;							(VAR c)
;						
;							(INT 10)
;						)
;					 )
;				 )
;			
;				(WHILE 
;					(LESSTHAN 
;						(VAR c)
;					
;						(VAR a)
;					)
;				 
;					(BLOCK 
;						(MOV 
;							(VAR c)
;						
;							(ADD 
;								(VAR c)
;							
;								(INT 1)
;							)
;						 )
;					 )
;				 )
;			)
;		 )
;	 )
;