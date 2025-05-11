;
; crt0_setjmp.asm
;
; Custom implementations of `setjmp` and `longjmp`, based on MSVC compiler binaries.
; These functions are part of the CRT0 project but cannot be implemented directly due to
; compiler restrictions requiring them to be intrinsic.
;
; If you need to use `setjmp` and `longjmp` in a no-CRT environment, you must inject
; direct calls to these functions by modifying the code. There are no safer or more
; convenient alternatives.
;
; [!] WARNING: These functions do not perform stack unwinding - destructors for stack-
;     allocated objects will NOT be called. This behavior is standard in pure C, but
;     in C++ environments, you must be extremely cautious. Avoid placing class instances
;     on the stack when using these functions.
;
; [*] NOTE: Full support would require integration with `RtlUnwind`. However, since these
;     functions are not intended for use in a complete C++ environment, such functionality
;     is not currently planned. This is provided as a reference for those who may need
;     to extend the implementation.
;

.CODE
	crt0_setjmp PROC
		lea rax, [rsp + 8]
		mov r8, [rsp]
		mov qword ptr [rcx + 0], 0
		mov [rcx + 8], rbx
		mov [rcx + 24], rbp
		mov [rcx + 32], rsi
		mov [rcx + 40], rdi
		mov [rcx + 48], r12
		mov [rcx + 56], r13
		mov [rcx + 64], r14
		mov [rcx + 72], r15
		mov [rcx + 16], rax
		mov [rcx + 80], r8
		stmxcsr dword ptr [rcx + 88]
		fnstcw  word  ptr [rcx + 92]
		movdqa [rcx + 96], xmm6
		movdqa [rcx + 112], xmm7
		movdqa [rcx + 128], xmm8
		movdqa [rcx + 144], xmm9
		movdqa [rcx + 160], xmm10
		movdqa [rcx + 176], xmm11
		movdqa [rcx + 192], xmm12
		movdqa [rcx + 208], xmm13
		movdqa [rcx + 224], xmm14
		movdqa [rcx + 240], xmm15
		xor rax, rax
		ret
	crt0_setjmp ENDP

	crt0_longjmp PROC
		mov rbx, [rcx + 8]
		mov rbp, [rcx + 24]
		mov rsi, [rcx + 32]
		mov rdi, [rcx + 40]
		mov r12, [rcx + 48]
		mov r13, [rcx + 56]
		mov r14, [rcx + 64]
		mov r15, [rcx + 72]
		mov rsp, [rcx + 16]
		mov r8,  [rcx + 80]
		ldmxcsr dword ptr [rcx + 88]
		fnclex
		fldcw   word  ptr [rcx + 92]
		movdqa xmm6,  [rcx + 96]
		movdqa xmm7,  [rcx + 112]
		movdqa xmm8,  [rcx + 128]
		movdqa xmm9,  [rcx + 144]
		movdqa xmm10, [rcx + 160]
		movdqa xmm11, [rcx + 176]
		movdqa xmm12, [rcx + 192]
		movdqa xmm13, [rcx + 208]
		movdqa xmm14, [rcx + 224]
		movdqa xmm15, [rcx + 240]
		mov rax, rdx
		test rax, rax
		jnz skip_inc
		inc rax
	skip_inc:
		jmp r8
	crt0_longjmp ENDP

END