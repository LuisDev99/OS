;os-api.asm
;Luis Ponce, 2019

;os-api.asm whatever bro this wont work
;El orden de los global y de las funciones importan, si no, tira: 'error reading input' (PUEDE SER)
;Si no funciona lo anterior, dejar varias lineas en blanco al final de este archivo

    .global _syscall_readFile
    .global _syscall_readSector
    .global _syscall_printString
    .global _syscall_readString
    .global _syscall_executeProgram
    .global _syscall_terminate
    .global _syscall_clearScreen
    .global _syscall_deleteFile
    .global _syscall_writeSector
    .global _syscall_writeFile
    .global _interrupt
	.global _enableInterrupts

;void syscall_readSector(char *buffer, int sector)
_syscall_readSector:
    push bp
    mov bp, sp
    mov ax, #2
    mov bx, [bp+4]  ;str
    mov cx, [bp+6]  ;sector
    int #0x21
    leave
    ret


;void syscall_printString(char *str)
_syscall_printString:
    push bp
    mov bp, sp
    mov ax, #0
    mov bx, [bp+4]  ;str
    int #0x21
    leave
    ret


;void syscall_readString(char *str)
_syscall_readString:
    push bp
    mov bp, sp
    mov ax, #1
    mov bx, [bp+4]  ;str
    int #0x21
    leave
    ret

;void syscall_readFile(char *fileName, char *buffer)
_syscall_readFile:
    push bp
    mov bp, sp
    mov ax, #3
    mov bx, [bp+4]  ;fileName
    mov cx, [bp+6]  ;buffer
    int #0x21
    leave
    ret

;void syscall_executeProgram(char *name, int segment)
_syscall_executeProgram:
    push bp
    mov bp, sp
    mov ax, #4
    mov bx, [bp+4] ;name
    int #0x21
    leave
    ret

;void syscall_terminate()
_syscall_terminate:
    push bp
    mov bp, sp
    mov ax, #5
    int #0x21
    leave
    ret

;void syscall_clearScreen()
_syscall_clearScreen:
    push bp
    mov bp, sp
    mov ax, #9
    int #0x21
    leave
    ret

;void deleteFile(char *name)
_syscall_deleteFile:
    push bp
    mov bp, sp
    mov ax, #7
    mov bx, [bp+4]  ;name
    int #0x21
    leave
    ret

;void syscall_writeSector(char *buffer, int sector)
_syscall_writeSector:
    push bp
    mov bp, sp
    mov ax, #6
    mov bx, [bp+4]  ;str
    mov cx, [bp+6]  ;sector
    int #0x21
    leave
    ret

;void syscall_writeFile(char* name, char* buffer, int numberOfSectors)
_syscall_writeFile:
    push bp
    mov bp, sp
    mov ax, #8
    mov bx, [bp+4]  ;name
    mov cx, [bp+6]  ;buffer
    mov dx, [bp+8]  ;numberOfSectors
    int #0x21
    leave
    ret

;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,#intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:	int #0x00	;call the interrupt (00 will be changed above)

	mov ah,#0	;we only want AL returned
	pop bp
	ret

;void enableInterrupts()
;call at the beginning of programs.  allows timer preemption
_enableInterrupts:
	sti
	ret

