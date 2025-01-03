#include "vga.h"

void kernel_start(unsigned long magic, unsigned long addr) {


    vga_init();

    kprintf("Hello, World!\n");
    kprintf("Newline!");

    while (1);
}