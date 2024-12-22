arch ?= x86_64
kernel := build/kernel-$(arch).elf
os := build/os-$(arch).bin

linker_script := src/arch/$(arch)/linker.ld

AS := $(arch)-elf-as
LD := $(arch)-elf-ld

CC := $(arch)-elf-gcc
CFLAGS := -O2 -ffreestanding -Wall -Wextra -nostdlib -nostdinc -nostartfiles

LDFLAGS := -T $(linker_script) -n -nostdlib

OBJCOPY := $(arch)-elf-objcopy

assembly_source_files := $(wildcard src/arch/$(arch)/*.S)
assembly_object_files := $(patsubst src/arch/$(arch)/%.S, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

c_source_files := $(wildcard src/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))

.PHONY: all clean run

all: $(os)

clean:
	@rm -r build

run: $(os)
	@qemu-system-$(arch) -drive format=raw,file=$(os)

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@$(LD) $(LDFLAGS) -o $(kernel) $(assembly_object_files) $(c_object_files)

$(os): $(kernel)
	@$(OBJCOPY) $(kernel) -O binary $(os)

build/arch/$(arch)/%.o: src/arch/$(arch)/%.S
	@mkdir -p $(shell dirname $@)
	@$(AS) -c $< -o $@

build/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@