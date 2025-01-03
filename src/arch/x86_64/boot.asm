SECTION .text
BITS 32
GLOBAL _start

EXTERN long_mode_start

_start:
    mov esp, stack_top

    call check_multiboot2
    call check_cpuid
    call check_long_mode

    call setup_paging
    call enable_paging

    lgdt [long_mode_global_descriptor_table.pointer]
    jmp long_mode_global_descriptor_table.code:long_mode_start

    hlt

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

check_multiboot2:
    cmp eax, 0x36D76289
    jne .no_multiboot2
    ret
.no_multiboot2:
    mov al, '0'
    jmp error

check_cpuid:
    pushfd
    pop eax

    mov ecx, eax

    xor eax, 1 << 21
    
    push eax
    popfd

    pushfd
    pop eax

    push ecx
    popfd

    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, '1'
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, '2'
    jmp error

setup_paging:
    mov eax, page_directory_pointer_table
    or eax, 0b11
    mov [page_map_level_4_table], eax

    mov eax, page_directory_table
    or eax, 0b11
    mov [page_directory_pointer_table], eax

    mov ecx, 0
.map_page_directory_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [page_directory_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_page_directory_table

    ret

enable_paging:
    mov eax, page_map_level_4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ret

SECTION .bss
align 4096
page_map_level_4_table:
    resb 4096
page_directory_pointer_table:
    resb 4096
page_directory_table:
    resb 4096
page_table:
    resb 4096
stack_bottom:
    resb 64
stack_top:

SECTION .rodata
long_mode_global_descriptor_table:
    dq 0
.code: equ $ - long_mode_global_descriptor_table
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - long_mode_global_descriptor_table - 1
    dq long_mode_global_descriptor_table
