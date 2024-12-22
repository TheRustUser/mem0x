.globl _start
.section .text._start
.code16

_start:
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    cld

    mov $0x7C00, %sp

    movw $stage_msg, %si
    call write_string
    movb $'1', %al
    call write_char
    call write_line

    call check_a20_line
    call enable_a20_line

    movw $success, %si
    call write_string
    call write_line

.L_start.halt:
    hlt
    jmp .L_start.halt

write_char:
    pusha
    movw $7, %bx
    movb $0x0E, %ah
    int $0x10
    popa
    ret

write_line:
    movb $10, %al
    call write_char
    movb $13, %al
    call write_char
    ret

write_hex:
    movw $4, %cx
.Lwrite_hex.loop:
    movb %bh, %al
    shrb $4, %al

    cmpb $0x0A, %al
    jb .Lwrite_hex.below_0xA

    addb $('A' - 0xA - '0'), %al
.Lwrite_hex.below_0xA:
    addb $'0', %al

    call write_char

    shlw $4, %bx
    loop .Lwrite_hex.loop

    ret

write_string:
    pushfw
    cld
.Lwrite_string.loop:
    lodsb
    testb %al, %al
    jz .Lwrite_string.done
    call write_char
    jmp .Lwrite_string.loop
.Lwrite_string.done:
    popfw
    ret

check_a20_line:
    pushfw
    pushw %ds
    pushw %es
    pushw %di
    pushw %si

    cli

    xorw %ax, %ax
    movw %ax, %es

    notw %ax
    movw %ax, %ds

    movw $0x0500, %di
    movw $0x0510, %si

    movb %es:(%di), %al
    pushw %ax

    movb %ds:(%si), %al
    pushw %ax

    movb $0x00, %es:(%di)
    movb $0xFF, %ds:(%si)

    cmpb $0xFF, %es:(%di)

    popw %ax
    movb %al, %ds:(%si)

    popw %ax
    movb %al, %es:(%di)

    movw $0, %ax
    je .Lcheck_a20_line.exit

    movw $1, %ax
.Lcheck_a20_line.exit:
    sti
    popw %si
    popw %di
    popw %es
    popw %ds
    popfw

    ret

enable_a20_line:
    inb $0x92, %al
    testb $2, %al
    jnz .Lenable_a20_line.after
    orb $2, %al
    andb $0xFE, %al
    outb %al, $0x92
.Lenable_a20_line.after:
    ret

stage_msg:
    .asciz "Running Mem0x Stage "

success:
    .asciz "Success"

.org 510
.word 0xAA55
