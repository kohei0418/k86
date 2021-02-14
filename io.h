//
// Created by Kohei Shiraga on 2021/02/14.
//

#ifndef K86_IO_H
#define K86_IO_H

#include <stdint.h>

static uint8_t io_in8(uint16_t address) {
    switch (address) {
        case 0x03f8:
            return getchar();
        default:
            return 0;
    }
}

static void io_out8(uint16_t address, uint8_t value) {
    switch (address) {
        case 0x03f8:
            putchar(value);
            break;
    }
}

#endif //K86_IO_H
