;os-api.asm
;Luis Ponce, 2019

;os-api.asm whatever bro this wont work
;El orden de los global y de las funciones importan, si no, tira: 'error reading input'

    .global _syscall_readSector
    .global _syscall_printString
    .global _syscall_readString
    

;syscall_readSector(char *buffer, int sector)
_syscall_readSector:
    push bp
    mov bp, sp
    mov ax, #2
    mov bx, [bp+4]  ;str
    mov cx, [bp+6]  ;sector
    int #0x21
    leave
    ret


;syscall_printString(char *str)
_syscall_printString:
    push bp
    mov bp, sp
    mov ax, #0
    mov bx, [bp+4]  ;str
    int #0x21
    leave
    ret


;syscall_readString(char *str)
_syscall_readString:
    push bp
    mov bp, sp
    mov ax, #1
    mov bx, [bp+4]  ;str
    int #0x21
    leave
    ret


