#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#define MULTIBOOT2_MAGIC 0x36D76289

inline void write_volatile(uint8_t* addr, uint8_t val) {
    *(volatile uint8_t*)addr = val;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline char* itoa(int value, char* buffer, int radix) {
    if (radix < 2 || radix > 36) {
        *buffer = '\0';
        return buffer;
    }

    char* ptr = buffer;
    char* end = buffer;
    int is_negative = 0;

    if (value < 0 && radix == 10) {
        is_negative = 1;
        value = -value;
    }

    do {
        int digit = value % radix;
        *end++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= radix;
    } while(value);

    if (is_negative) {
        *end++ = '-';
    }

    *end = '\0';

    for (--end; ptr < end; ++ptr, --end) {
        char temp = *ptr;
        *ptr = *end;
        *end = temp;
    }

    return buffer;
}

static inline char* utoa(unsigned int value, char* buffer, int radix) {
    if (radix < 2 || radix > 36) {
        *buffer = '\0';
        return buffer;
    }

    char* ptr = buffer;
    char* end = buffer;

    do {
        int digit = value % radix;
        *end++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= radix;
    } while(value);

    *end = '\0';

    for (--end; ptr < end; ++ptr, --end) {
        char temp = *ptr;
        *ptr = *end;
        *end = temp;
    }

    return buffer;
}

#endif
