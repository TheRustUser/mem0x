SECTION .text
BITS 64
GLOBAL long_mode_start

EXTERN kernel_start

long_mode_start:
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_start

    hlt
.halt:
    jmp $
