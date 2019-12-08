;kernel.asm
;Michael Black, 2007

;kernel.asm contains assembly functions that you can use in your kernel
;Consideraciones para este proyecto:
;	- Poner el numeral en las constantes
;	- Balancear bien la pila
;	- Exportar bien las funciones
;	- Fijarse bien en los outputs de los compilers y assemblers


	.global _putInMemory
	.global _interrupt
	.global _makeInterrupt21
	.global _launchProgram
	.extern _handleInterrupt21
	.global _printhex
	.global _makeTimerInterrupt
	.extern _handleTimerInterrupt
	.global _returnFromTimer
	.global _initializeProgram
	.global _setKernelDataSegment
	.global _restoreDataSegment
        .global _loadProgram    ;here
        .global _printChar
	.global _readChar
	.global _readSector
    .global _compareFileNameStrings
	.global _loadFileIntoBuffer
	.global _binToDec
	.global _power
	.global _clearScreen
	.global _writeSector

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

_clearScreen:
	mov ax, #0x0003
	int #0x10
	ret

;void printChar(char ch)
_printChar:
	push bp
	mov bp, sp
	mov al, [bp+4]
	mov ah, #0x0e
	int #0x10
	leave
	ret

;char readChar();
_readChar:
	;set interrupt parameter
	mov ah, #0x00
	int #0x16
	;set the return register to whatever the interrupt returned
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
	;Save the result in the stack
	mov [bp-4], dl

	;track number = (sector / 36)
	mov ax, [bp+6]
	mov bx, #0x24
	mov dx, #0x00
	div bx
	mov ch, al
	;Save the result in the stack
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
	;Save the result in the stack
	mov [bp-8], dl

	;setting up all other registers
	mov cl, [bp-4]	;Read result from the stack
	mov ch, [bp-6]	;Read result from the stack
	mov dh, [bp-8]	;Read result from the stack
	mov ah, #0x02
	mov al, #0x01
	mov bx, [bp+4]
	mov dl, #0x00
	int #0x13
	leave
	ret

;writeSector(char* buffer, int sector)
_writeSector:
	push bp
	mov bp, sp

	;relative_sector = sector MOD 18
	mov ax, [bp+6]
	mov bx, #0x12
	mov dx, #0x00
	div bx
	mov cl, dl
	add dl, #1	; relative_sector + 1
	;Save the result in the stack
	mov [bp-4], dl

	;track number = (sector / 36)
	mov ax, [bp+6]
	mov bx, #0x24
	mov dx, #0x00
	div bx
	mov ch, al
	;Save the result in the stack
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
	;Save the result in the stack
	mov [bp-8], dl

	;setting up all other registers
	mov cl, [bp-4]	;Read result from the stack
	mov ch, [bp-6]	;Read result from the stack
	mov dh, [bp-8]	;Read result from the stack
	mov ah, #0x03
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
	sti
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx

	iret

;this is called to start a program that is loaded into memory
;void launchProgram(int segment)
_launchProgram:
	mov bp,sp
	mov bx,[bp+2]	;get the segment into bx

	mov ax,cs	;modify the jmp below to jump to our segment
	mov ds,ax	;this is self-modifying code
	mov si,#jump
	mov [si+3],bx	;change the first 0000 to the segment

	mov ds,bx	;set up the segment registers
	mov ss,bx
	mov es,bx

	mov sp,#0xfff0	;set up the stack pointer
	mov bp,#0xfff0

	sti
jump:	jmp #0x0000:0x0000	;and start running (the first 0000 is changed above)


;void makeTimerInterrupt()
;sets up the timer's interrupt service routine
_makeTimerInterrupt:
        cli
        mov dx,#timer_ISR ;get address of timerISR in dx

        push ds
        mov ax,#0       ;interrupts are at lowest memory
        mov ds,ax
        mov si,#0x20    ;timer interrupt vector (8 * 4)
        mov ax,cs       ;have interrupt go to the current segment
        mov [si+2],ax
        mov [si],dx     ;address of our vector
        pop ds

        ;start the timer
        mov al,#0x36
        out #0x43,al
        mov ax,#0xFF
        out #0x40,al
        mov ax,#0xFF
        out #0x40,al

	sti
        ret

;this routine runs on timer interrupts
timer_ISR:

        ;disable interrupts
        cli

        ;save all regs for the old process on the old process's stack
        push bx
        push cx
        push dx
        push si
        push di
        push bp
        push ax
        push ds
        push es

        ;reset interrupt controller so it performs more interrupts
        mov al,#0x20
        out #0x20,al

        ;get the segment (ss) and the stack pointer (sp) - we need to keep these
        mov bx,ss
        mov cx,sp

        ;set all segments to the kernel
        mov ax,#0x1000
        mov ds,ax
        mov es,ax
        mov ss,ax
        ;set the kernel's stack
        mov ax,#0xdff0
        mov sp,ax
        mov bp,ax

        ;call handle interrupt with 2 parameters: the segment, the stack pointer.
	mov ax,#0
        push cx
        push bx
        call _handleTimerInterrupt

;void returnFromTimer(int segment, int sp)
;returns from a timer interrupt to a different process
_returnFromTimer:
        ;pop off the local return address - don't need it
        pop ax
        ;get the segment and stack pointer
        pop bx
        pop cx

        ;get rid of the junk from the two calls and no returns
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax
        pop ax

        ;set up the stack
        mov sp,cx
        ;set up the stack segment
        mov ss,bx

        ;now we're back to the program's area
        ;reload the registers (if this is it's first time running, these will be zeros)
        pop es
        pop ds
        pop ax
        pop bp
        pop di
        pop si
        pop dx
        pop cx
        pop bx

        ;enable interrupts and return
        sti
        iret

;void initializeProgram(int segment)
;this initializes a new program but doesn't start it running
;the scheduler will take care of that
;the program will be located at the beginning of the segment at [sp+2]
_initializeProgram:
        ;bx=new segment
		push	bp
        mov     bp,sp
        mov     bx,[bp+4]

;make a stack image so that the timer interrupt can start this program

        ;save the caller's stack pointer and segment
        mov     cx,sp
        mov     dx,ss
        mov     ax,#0xff18      ;this allows an initial sp of 0xff00
        mov     sp,ax
        mov     ss,bx

        mov     ax,#0   ;IP
        push    ax
        mov     ax,bx   ;CS
        push    ax
        mov     ax,#0x0         ;a normal flag setting
        push    ax
        mov     ax,#0           ;set all the general registers to 0
        push    ax      ;bx
        push    ax      ;cx
        push    ax      ;dx
        push    ax      ;si
        push    ax      ;di
        push    ax      ;bp
        push    ax      ;ax
        mov     ax,bx
        push    ax      ;ds
        push    ax      ;es

        ;restore the stack to the caller
        mov     sp,cx
        mov     ss,dx
		pop	bp
        ret

;void setKernelDataSegment()
;sets the data segment to the kernel, saving the current ds on the stack
_setKernelDataSegment:
        pop bx
        push ds
        push bx
        mov ax,#0x1000
        mov ds,ax
        ret

;void restoreDataSegment()
;restores the data segment
_restoreDataSegment:
        pop bx
        pop ds
        push bx
        ret

;printhex is used for debugging only
;it prints out the contents of ax in hexadecimal
_printhex:
        push bx
        push ax
        push ax
        push ax
        push ax
        mov al,ah
        mov ah,#0xe
        mov bx,#7
        shr al,#4
        and al,#0xf
        cmp al,#0xa
        jb ph1
        add al,#0x7
ph1:    add al,#0x30
        int 0x10

        pop ax
        mov al,ah
        mov ah,#0xe
        and al,#0xf
        cmp al,#0xa
        jb ph2
        add al,#0x7
ph2:    add al,#0x30
        int 0x10

        pop ax
        mov ah,#0xe
        shr al,#4
        and al,#0xf
        cmp al,#0xa
        jb ph3
        add al,#0x7
ph3:    add al,#0x30
        int 0x10

        pop ax
        mov ah,#0xe
        and al,#0xf
        cmp al,#0xa
        jb ph4
        add al,#0x7
ph4:    add al,#0x30
        int 0x10

        pop ax
        pop bx
        ret

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



