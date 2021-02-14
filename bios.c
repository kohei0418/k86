//
// Created by Kohei Shiraga on 2021/02/14.
//

#include <stdio.h>
#include "emulator.h"
#include "io.h"

static int bios_to_terminal[8] = {30, 34, 32, 36, 31, 35, 33, 37};

static void put_string(const char* s, size_t n) {
    for (size_t i = 0; i < n; i++) {
        io_out8(0x03f8, s[i]);
    }
}

void bios_video_teletype(Emulator* emu) {
    uint8_t color = get_register8(emu, BL) & 0x0f;
    uint8_t ch = get_register8(emu, AL);

    char buf[32];
    int terminal_color = bios_to_terminal[color & 0x07];
    int brightness = (color & 0x08) ? 1 : 0;
    int len = sprintf(buf, "\x1b[%d;%dm%c\x1b[0m", brightness, terminal_color, ch);
    put_string(buf, len);
}

void bios_video(Emulator* emu) {
    uint8_t func = get_register8(emu, AH);
    switch (func) {
        case 0x0e:
            bios_video_teletype(emu);
            break;
        default:
            printf("Unknown bios function: 0x%02x\n", func);
    }
}