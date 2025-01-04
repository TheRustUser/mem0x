#include "vga.h"
#include "lib.h"

#include <stdbool.h>
#include "multiboot2.h"



void kernel_start(unsigned long magic, unsigned long addr) {

    struct multiboot_tag* tag;
    unsigned size;

    vga_init();

    if (magic != MULTIBOOT2_MAGIC) {
        kprintf("ERROR: Invalid Multiboot2 Magic: 0x%x\n", magic);
        while (true);
    }

    kprintf("Multiboot2 Magic: 0x%x\n", magic);

    if (addr & 7) {
        kprintf("Unaligned Multiboot Information: 0x%x\n", addr);
        while (true);
    }

    kprintf("Multiboot Information Address: 0x%x\n", addr);

    size = *(unsigned*) addr;
    kprintf("Multiboot Information Size: 0x%x\n", size);
    for (tag = (struct multiboot_tag*)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
        switch(tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            kprintf("Command Line: %s\n", ((struct multiboot_tag_string*)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            kprintf("Bootloader Name: %s\n", ((struct multiboot_tag_string*)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            kprintf("Module at: 0x%x-0x%x | Command Line: %s\n", ((struct multiboot_tag_module*)tag)->mod_start, ((struct multiboot_tag_module*)tag)->mod_end, ((struct multiboot_tag_module*)tag)->cmdline);
            break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            kprintf("mem_lower: %uKB, mem_upper: %uKB\n", ((struct multiboot_tag_basic_meminfo*)tag)->mem_lower, ((struct multiboot_tag_basic_meminfo*)tag)->mem_upper);
            break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
            kprintf("Boot Device: 0x%x, %u, %u\n", ((struct multiboot_tag_bootdev*)tag)->biosdev, ((struct multiboot_tag_bootdev*)tag)->slice, ((struct multiboot_tag_bootdev*)tag)->part);
            break;
        case MULTIBOOT_TAG_TYPE_MMAP:
            multiboot_memory_map_t* mmap;
            
            kprintf("Memory Areas:\n");

            for (mmap = ((struct multiboot_tag_mmap*)tag)->entries; (multiboot_uint8_t*)mmap < (multiboot_uint8_t*)tag + tag->size; mmap = (multiboot_memory_map_t*)((unsigned long)mmap + ((struct multiboot_tag_mmap*)tag)->entry_size)) {
                kprintf("Base Addr: 0x%x%x | Length: 0x%x%x | Type: 0x%x\n", (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xFFFFFFFF), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xFFFFFFFF), (unsigned)mmap->type);
            } 
            break;
        }
    }

    tag = (struct multiboot_tag*) ((multiboot_uint8_t*)tag + ((tag->size + 7) & ~7));
    kprintf("Total Multiboot Information Size: 0x%x\n", (unsigned) tag - addr);
    

    kprintf("Hello, World!\n");
    kprintf("Newline!");

    while (true);
}