#include "vga.h"

#include <stdint.h>
#include <stddef.h>
#include <dri/io.h>
#include <string.h>
#include <stdarg.h>

#define VGA_TEXT_MODE
#ifdef VGA_TEXT_MODE

#define VGA_ADDRESS 0xB8000

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define vga_color_code(foreground, background) (background << 4 | foreground)

void write_volatile(uintptr_t address, uint8_t value) {
    *(volatile uintptr_t *)address = value;
}

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_DARK_GRAY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_PINK = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15
} vga_color_t;

typedef struct {
    uint8_t ascii_code;
    uint8_t color_code;
} vga_character_t;

typedef struct {
    vga_character_t buffer[VGA_HEIGHT][VGA_WIDTH];
} vga_buffer_t;

vga_buffer_t *VGA = (vga_buffer_t *)VGA_ADDRESS;
size_t VGA_COLUMN_POSITION = 0;
vga_color_t VGA_COLOR_CODE = vga_color_code(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_enable_cursor(uint8_t start, uint8_t end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | end);
}

void vga_update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_clear_row(size_t row) {
    for (size_t col = 0; col < VGA_WIDTH; col++) {
        write_volatile((uintptr_t)&VGA->buffer[row][col].ascii_code, ' ');
        write_volatile((uintptr_t)&VGA->buffer[row][col].color_code, VGA_COLOR_CODE);
    }
}

void vga_new_line(void) {
    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            write_volatile((uintptr_t)&VGA->buffer[row - 1][col].ascii_code, VGA->buffer[row][col].ascii_code);
            write_volatile((uintptr_t)&VGA->buffer[row - 1][col].color_code, VGA->buffer[row][col].color_code);
        }
    }

    vga_clear_row(VGA_HEIGHT - 1);
    VGA_COLUMN_POSITION = 0;
}

void vga_write_byte(uint8_t byte) {
    switch (byte) {
    case '\n':
        vga_new_line();
        break;
    case '\0':
        break;
    default:
        if (VGA_COLUMN_POSITION >= VGA_WIDTH) {
            vga_new_line();
        }

        size_t row = VGA_HEIGHT - 1;
        size_t col = VGA_COLUMN_POSITION;

        vga_color_t color = VGA_COLOR_CODE;

        write_volatile((uintptr_t)&VGA->buffer[row][col].ascii_code, byte);
        write_volatile((uintptr_t)&VGA->buffer[row][col].color_code, color);
        ++VGA_COLUMN_POSITION;
        break;
    }
}

void vga_write_string(const char* fmt, size_t len, ...) {
    va_list args;
    va_start(args, len);

    for (size_t i = 0; i < len; i++) {
        if (fmt[i] == '%' && fmt[i + 1]) {
            i++;
            if (fmt[i] == 's') {
                const char *buf = va_arg(args, char *);
                
                while (*buf) {
                    vga_write_byte(*buf++);
                }
            } else if (fmt[i] == 'c') {
                const char buf = (char)va_arg(args, int);
                vga_write_byte(buf);
            } else if (fmt[i] == 'x') {
                int value = va_arg(args, int);
                char buf[256];
                itoa(buf, fmt[i], value);

                for (size_t i = 0; i < strlen(buf); i++) {
                    vga_write_byte(buf[i]);
                }
            } else if (fmt[i] == 'd') {
                int value = va_arg(args, int);
                char buf[256];
                itoa(buf, fmt[i], value);

                for (size_t i = 0; i < strlen(buf); i++) {
                    vga_write_byte(buf[i]);
                }
            }
        } else {
            vga_write_byte(fmt[i]);
        }
    }

    va_end(args);
}

void vga_init(void) {
    vga_disable_cursor();

    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        vga_clear_row(row);
    }
}

#endif
