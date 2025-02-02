#include <cpu/idt.h>
#include <stdbool.h>
#include <dri/io.h>
#include <cpu/isr.h>

#define KERNEL_CS 0x08
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

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_register_t;

__attribute__((aligned(0x10)))
static idt_entry_t IDT[IDT_MAX_DESCRIPTORS];

static idt_register_t IDT_REGISTER;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t *descriptor = &IDT[vector];

    descriptor->isr_low = (uint64_t)isr & 0xffff;
    descriptor->kernel_cs = KERNEL_CS;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xffff;
    descriptor->isr_high = ((uint64_t)isr >> 32) & 0xffffffff;
    descriptor->reserved = 0;
}

static bool vectors[IDT_MAX_DESCRIPTORS];


extern void divide_by_zero_handler_wrapper(void);
extern void breakpoint_exception_handler_wrapper(void);
extern void invalid_opcode_handler_wrapper(void);
extern void double_fault_handler_wrapper(void);
extern void page_fault_handler_wrapper(void);

void idt_init(void) {
    IDT_REGISTER.base = (uintptr_t)&IDT[0];
    IDT_REGISTER.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    idt_set_descriptor(0, divide_by_zero_handler_wrapper, 0x8E);
    vectors[0] = true;

    idt_set_descriptor(3, breakpoint_exception_handler_wrapper, 0x8E);
    vectors[3] = true;

    idt_set_descriptor(6, invalid_opcode_handler_wrapper, 0x8E);
    vectors[6] = true;

    idt_set_descriptor(8, double_fault_handler_wrapper, 0x8E);
    vectors[8] = true;

    // idt_set_descriptor(14, page_fault_handler_wrapper, 0x8E);
    // vectors[14] = true;

    outb(0x21, 0xff);
    outb(0xa1, 0xff);

    __asm__ volatile("lidtq (%0)" : : "r"(&IDT_REGISTER));
    __asm__ volatile("sti");
}

