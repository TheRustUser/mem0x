SECTION .text
BITS 64
GLOBAL long_mode_start

EXTERN kernel_start

long_mode_start:
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_start

    hlt
.halt:
    jmp $
