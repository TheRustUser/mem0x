#include "vga.h"
#include "lib.h"

#define VGA_TEXT_MODE_HEIGHT 25
#define VGA_TEXT_MODE_WIDTH 80

#define VGA_BUFFER_ADDR 0xB8000

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
    vga_character_t characters[VGA_TEXT_MODE_HEIGHT][VGA_TEXT_MODE_WIDTH];
} vga_buffer_t;

#define vga_color_code(foreground, background) (background << 4 | foreground)

vga_buffer_t* VGA_BUFFER = (vga_buffer_t*) VGA_BUFFER_ADDR;
size_t VGA_COLUMN_POSITION = 0;
vga_color_t VGA_COLOR_CODE = vga_color_code(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

void vga_clear_row(size_t row) {
    for (size_t col = 0; col < VGA_TEXT_MODE_WIDTH; col++) {
        write_volatile(&VGA_BUFFER->characters[row][col].ascii_code, ' ');
        write_volatile(&VGA_BUFFER->characters[row][col].color_code, VGA_COLOR_CODE);
    }
};

void vga_new_line(void) {
    for (size_t row = 1; row < VGA_TEXT_MODE_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_TEXT_MODE_WIDTH; col++) {
            write_volatile(&VGA_BUFFER->characters[row - 1][col].ascii_code, VGA_BUFFER->characters[row][col].ascii_code);
            write_volatile(&VGA_BUFFER->characters[row - 1][col].color_code, VGA_BUFFER->characters[row][col].color_code);
        }
    }

    vga_clear_row(VGA_TEXT_MODE_HEIGHT - 1);
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
        if (VGA_COLUMN_POSITION >= VGA_TEXT_MODE_WIDTH) {
            vga_new_line();
        }

        size_t row = VGA_TEXT_MODE_HEIGHT - 1;
        size_t col = VGA_COLUMN_POSITION;

        vga_color_t color = VGA_COLOR_CODE;

        write_volatile(&VGA_BUFFER->characters[row][col].ascii_code, byte);
        write_volatile(&VGA_BUFFER->characters[row][col].color_code, color);
        ++VGA_COLUMN_POSITION;
        break;
    };
};

void vga_write_string(const char* s) {
    while (*s) {
        if (0x20 <= *s || *s <= 0x7E) {
            vga_write_byte(*s);
        } else {
            vga_write_byte(0xFE);
        }
        s++;
    }
}

void vga_init(void) {
    for (size_t row = 0; row < VGA_TEXT_MODE_HEIGHT; row++) {
        vga_clear_row(row);
    }
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    const char* current = fmt;
    while (*current) {
        if (*current == '%' && *(current + 1)) {
            current++;
            switch (*current)
            {
            case 's':
                const char* s = va_arg(args, const char*);
                vga_write_string(s);
                break;
            case 'c':
                char c = (uint8_t)va_arg(args, unsigned int);
                vga_write_byte(c);
                break;
            case 'd':
                break;
            default:
                vga_write_byte('%');
                vga_write_byte(*current);
                break;
            }
        } else {
            vga_write_byte(*current);
        }

        current++;
    }

    va_end(args);
}
