#ifndef __REGISTERS_H
#define __REGISTERS_H

#include <stdint.h>

static inline uintptr_t cr2(void) {
    uintptr_t address;
    __asm__ volatile("movq %%cr2, %0" : "=r"(address));
    return address;
}

#endif
