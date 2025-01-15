#ifndef __STDIO_H
#define __STDIO_H

#include <dri/vga.h>
#include <string.h>

extern void vga_write_string(const char* fmt, size_t len, ...);

#define printf(fmt, ...) vga_write_string(fmt, strlen(fmt), ##__VA_ARGS__)

#endif
