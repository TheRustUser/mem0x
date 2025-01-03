ENTRY(_start)

SECTIONS {
    . = 1M;

    .boot : {
        *(.multiboot2_header)
    }

    .text : {
        *(.text)
    }

    .bss : {
        *(.bss)
    }

    .rodata : {
        *(.rodata)
    }
}