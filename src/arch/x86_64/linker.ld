ENTRY(_start)

SECTIONS {
    . = 0x7C00;

    .text : {
        KEEP(*(.text._start))
        *(.text .text.* .gnu.linkonce.t*)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss.stack)
        *(.bss)
    }
}