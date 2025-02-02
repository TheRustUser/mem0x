#ifndef __ISR_H
#define __ISR_H

#include <stdint.h>

#define pushaq() \
    __asm__ volatile( \
        "pushq %%rax\n\t" \
        "pushq %%rcx\n\t" \
        "pushq %%rdx\n\t" \
        "pushq %%rsi\n\t" \
        "pushq %%rdi\n\t" \
        "pushq %%r8\n\t" \
        "pushq %%r9\n\t" \
        "pushq %%r10\n\t" \
        "pushq %%r11" \
         : : : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11")

#define popaq() \
    __asm__ volatile( \
        "popq %%rax\n\t" \
        "popq %%rcx\n\t" \
        "popq %%rdx\n\t" \
        "popq %%rsi\n\t" \
        "popq %%rdi\n\t" \
        "popq %%r8\n\t" \
        "popq %%r9\n\t" \
        "popq %%r10\n\t" \
        "popq %%r11" \
         : : : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11")

typedef struct {
    uint64_t instruction_pointer;
    uint64_t code_segment;
    uint64_t cpu_flags;
    uint64_t stack_pointer;
    uint64_t stack_segment;
} exception_stack_frame_t;

#endif
