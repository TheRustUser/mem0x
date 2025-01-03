#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

void vga_init(void);

void kprintf(const char* fmt, ...);

#endif