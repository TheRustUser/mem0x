#include <dri/vga.h>
#include <stdio.h>
#include <multiboot2.h>
#include <elf.h>

void kernel_start(unsigned long multiboot2_information_address) {

    vga_init();

    if (multiboot2_information_address & 7) {
        printf("ERROR: Unaligned Multiboot Information 0x%x\n", multiboot2_information_address);
    }

    for (struct multiboot_tag *tag = (struct multiboot_tag *) (multiboot2_information_address + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
        switch(tag->type) {
        case MULTIBOOT_TAG_TYPE_MMAP:
            multiboot_memory_map_t *mmap;

            printf("Memory Areas:\n");
            for (mmap = ((struct multiboot_tag_mmap *)tag)->entries; (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size; mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size)) {
                if (mmap->type == 0x1) {
                    printf("    Start: 0x%x%x | Length: 0x%x%x\n", (unsigned)(mmap->addr >> 32), (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), mmap->type);
                }
            }
            break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            struct multiboot_tag_elf_sections *elf_sections_tag = ((struct multiboot_tag_elf_sections *)tag);

            size_t num_sections = elf_sections_tag->num;
            size_t section_size = elf_sections_tag->entsize;

            printf("Kernel Sections:\n");

            for (size_t i = 1; i < num_sections; i++) {

                Elf64_Shdr *section_header = (Elf64_Shdr *)(elf_sections_tag->sections + i * sizeof(Elf64_Shdr));

                printf("    Addr: 0x%x | Size: 0x%x | Flags: 0x%x\n", section_header->sh_addr, section_header->sh_size, section_header->sh_flags);
            }

            break;
        }
    }

    printf("Hello, World!\n%s\n", "Hello, VGA!");
}