#include <dri/vga.h>
#include <stdio.h>
#include <multiboot2.h>
#include <elf.h>
#include <interrupt.h>

#define MULTIBOOT2_MAGIC 0x36D76289

void kernel_start(unsigned long multiboot2_magic_number, unsigned long multiboot2_information_address) {

    vga_init();

    if (multiboot2_magic_number != MULTIBOOT2_MAGIC) {
        printf("ERROR: Invalid Multiboot2 Magic! 0x%x\n", multiboot2_magic_number);
        __asm__ volatile("cli");
        __asm__ volatile("hlt");
    }

    printf("Multiboot2 Magic Number: 0x%x\n", multiboot2_magic_number);

    if (multiboot2_information_address & 7) {
        printf("ERROR: Unaligned Multiboot Information 0x%x\n", multiboot2_information_address);
    }

    size_t size = *(size_t*)multiboot2_information_address;


    printf("Multiboot2 Information  Address: 0x%x | Size: 0x%x\n", multiboot2_information_address, size);
    struct multiboot_tag* tag;
    for (tag = (struct multiboot_tag*)(multiboot2_information_address + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *)((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
        switch(tag->type) {
        case MULTIBOOT_TAG_TYPE_MMAP:
            multiboot_memory_map_t* mmap;

            printf("Memory Areas:\n");
            for (mmap = ((struct multiboot_tag_mmap*)tag)->entries; (multiboot_uint8_t*)mmap < (multiboot_uint8_t*)tag + tag->size; mmap = (multiboot_memory_map_t*)((unsigned long)mmap + ((struct multiboot_tag_mmap*)tag)->entry_size)) {
                printf("Base Addr: 0x%x%x | Length: 0x%x%x | Type: 0x%x\n", (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xFFFFFFFF), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xFFFFFFFF), (unsigned)mmap->type);
            }
            break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            struct multiboot_tag_elf_sections* elf_sections_tag = ((struct multiboot_tag_elf_sections*)tag);

            size_t num_sections = elf_sections_tag->num;
            size_t section_size = elf_sections_tag->entsize;

            void *sections = (void*)((uint8_t*)elf_sections_tag + sizeof(struct multiboot_tag_elf_sections));

            printf("Kernel ELF Sections:\n");

            for (size_t i = 0; i < num_sections; i++) {
                uint8_t *current_section = (uint8_t*)sections + i * section_size;

                Elf64_Shdr* section_header = (Elf64_Shdr*)current_section;

                printf("    Addr: 0x%x%x | Size: 0x%x%x | Flags: 0x%x\n"
                    , (unsigned)(section_header->sh_addr >> 32)
                    , (unsigned)(section_header->sh_addr & 0xFFFFFFFF)
                    , (unsigned)(section_header->sh_size >> 32)
                    , (unsigned)(section_header->sh_size & 0xFFFFFFFF)
                    , (unsigned)(section_header->sh_flags));
            }
        }
    }

    __asm__ volatile("cli");

    idt_init();

    printf("Hello, World!\n%s\n", "Hello, VGA!");
}
