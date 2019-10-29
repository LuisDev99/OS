;kernel.asm
;Michael Black, 2007

;kernel.asm contains assembly functions that you can use in your kernel

	.global _putInMemory
	.global _interrupt
	.global _makeInterrupt21
	.extern _handleInterrupt21
	.global _loadProgram
	.global _printChar
	.global _readChar
	.global _readSector

;void putInMemory (int segment, int address, char character)
_putInMemory:
	push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret

;void printChar(char ch)
_printChar:
	push bp
	mov bp, sp
	mov al, 4[bp]
	mov ah, #0x0e
	int #0x10
	pop bp
	ret

;char readChar();
_readChar:
	mov ah, #0x00
	int #0x16
	mov ah, al
	ret

;readSector(char* buffer, int sector)
_readSector:
	push bp
	mov bp, sp

	;relative_sector = sector MOD 18
	mov ax, [bp+6]
	mov bx, #0x12
	mov dx, #0x00
	div bx
	mov cl, dl
	add dl, #1	; relative_sector + 1
	mov [bp-4], dl

	;track number = (sector / 36)
	mov ax, [bp+6]
	mov bx, #0x24
	mov dx, #0x00
	div bx
	mov ch, al
	mov [bp-6], al

	;head = (sector / 18) MOD 2
	mov ax, [bp+6]
	mov bx, #0x12
	mov dx, #0x00
	div bx		;ax = sector / 18
	mov bx, #0x02
	mov dx, #0x00	
	div bx     ;ax = ax % 2
	mov dh, dl
	mov [bp-8], dl

	;setting up all other registers
	mov cl, [bp-4]
	mov ch, [bp-6]
	mov dh, [bp-8]
	mov ah, #0x02
	mov al, #0x01
	mov bx, [bp+4]
	mov dl, #0x00
	int #0x13
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

;void makeInterrupt21()
;this sets up the interrupt 0x21 vector
;when an interrupt 0x21 is called in the future, 
;_interrupt21ServiceRoutine will run

_makeInterrupt21:
	;get the address of the service routine
	mov dx,#_interrupt21ServiceRoutine
	push ds
	mov ax, #0	;interrupts are in lowest memory
	mov ds,ax
	mov si,#0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx

	iret

; Load a program from sector 11 into segment 0x20000
_loadProgram:
	mov ax, #0x2000
	mov ds, ax
	mov ss, ax
	mov es, ax
;let's have the stack start at 0x2000:fff0
	mov ax, #0xfff0
	mov sp, ax
	mov bp, ax
; Read the program from the floppy
	mov cl, #12
;cl holds sector number
	mov dh, #0
;dh holds head number - 0
	mov ch, #0
;ch holds track number - 0
	mov ah, #2
;absolute disk read
	mov al, #1
;read 1 sector
	mov dl, #0
;read from floppy disk A
	mov bx, #0
;read into offset 0 (in the segment)
	int #0x13
;call BIOS disk read function
; Switch to program
	jmp #0x2000:#0
