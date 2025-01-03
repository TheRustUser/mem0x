#ifndef LIB_H
#define LIB_H

#include <stdint.h>

inline void write_volatile(uint8_t* addr, uint8_t val) {
    *(volatile uint8_t*)addr = val;
}



#endif
