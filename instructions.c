//
// Created by Kohei Shiraga on 2021/02/11.
//

#include <stdlib.h>
#include <string.h>

#include "instructions.h"

instruction_func_t* instructions[256];

void mov_r32_imm32(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0xB8;
    uint32_t value = get_code32(emu, 1);
    emu->registers[reg] = value;
    emu->eip += 5;
}

void short_jump(Emulator* emu) {
    int8_t diff = get_signed_code8(emu, 1);
    emu->eip += diff + 2;
}

void near_jump(Emulator *emu) {
    int32_t diff = get_signed_code32(emu, 1);
    emu->eip += diff + 5;
}

void init_instructions(void) {
    memset(instructions, 0, sizeof(instructions));
    for (int i = 0; i < 8; i++) {
        instructions[0xB8 + i] = mov_r32_imm32;
    }
    instructions[0xE9] = near_jump;
    instructions[0xEB] = short_jump;
}