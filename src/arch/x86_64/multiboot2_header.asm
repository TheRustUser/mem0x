MULTIBOOT2_MAGIC_NUMBER equ 0xE85250D6
MULTIBOOT2_ARCHITECTURE equ 0
MULTIBOOT2_HEADER_LENGTH equ multiboot2_header_end - multiboot2_header_start
MULTIBOOT2_CHECKSUM equ 0x100000000 - (MULTIBOOT2_MAGIC_NUMBER + MULTIBOOT2_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

SECTION .multiboot2_header

multiboot2_header_start:
    dd MULTIBOOT2_MAGIC_NUMBER
    dd MULTIBOOT2_ARCHITECTURE
    dd MULTIBOOT2_HEADER_LENGTH
    dd MULTIBOOT2_CHECKSUM

    dw 0
    dw 0
    dd 8
multiboot2_header_end: