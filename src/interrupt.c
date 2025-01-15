#include <interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <dri/io.h>

#define IDT_MAX_DESCRIPTORS 256

typedef struct {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10)))
static idt_entry_t IDT[256];

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

static idtr_t IDTR;

__attribute__((noreturn))
void exception_handler(void);
void exception_handler(void) {
    __asm__ volatile("cli; hlt");
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &IDT[vector];

    descriptor->isr_low = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];

void idt_init(void) {
    IDTR.base = (uintptr_t)&IDT[0];
    IDTR.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    outb(0x21, 0xFF);

    __asm__ volatile("lidt %0" : : "m"(IDTR));
    __asm__ volatile("sti");
}
