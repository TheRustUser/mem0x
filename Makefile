arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

c_source_files := $(wildcard src/*.c) $(wildcard src/**/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))

CFLAGS := -std=gnu99 -ffreestanding -Wall -Wextra -nostdlib -nostdinc -nostartfiles -g
INCLUDE := -I./include -I./src/lib -I./src

.PHONY: all clean run debug iso

all: $(iso)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

debug: $(iso)
	@qemu-system-x86_64 -cdrom $(iso) -s -S

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@x86_64-elf-ld --gc-sections -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)

build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

build/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@x86_64-elf-gcc $(CFLAGS) $(INCLUDE) -c $< -o $@
