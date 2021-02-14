//
// Created by Kohei Shiraga on 2021/02/11.
//

#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "modrm.h"

instruction_func_t* instructions[256];

// move

void mov_r32_imm32(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0xB8;
    uint32_t value = get_code32(emu, 1);
    emu->eip += 5;
    set_register32(emu, reg, value);
}

void mov_rm32_imm32(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    uint32_t value = get_code32(emu, 0);
    emu->eip += 4;
    set_rm32(emu, &modrm, value);
}

void mov_rm32_r32(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t r32 = get_r32(emu, &modrm);
    set_rm32(emu, &modrm, r32);
}

void mov_r32_rm32(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);
    uint32_t rm32 = get_rm32(emu, &modrm);
    set_r32(emu, &modrm, rm32);
}

// arithmetic

void add_rm32_r32(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    uint32_t r32 = get_r32(emu, &modrm);
    uint32_t rm32 = get_rm32(emu, &modrm);
    set_rm32(emu, &modrm, rm32 + r32);
}

static void add_rm32_imm8(Emulator* emu, ModRM* modrm)
{
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t) get_signed_code8(emu, 0);
    emu->eip += 1;
    set_rm32(emu, modrm, rm32 + imm8);
}

void sub_rm32_imm8(Emulator* emu, ModRM* modrm) {
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t) get_signed_code8(emu, 0);
    emu->eip += 1;
    set_rm32(emu, modrm, rm32 - imm8);
}

void inc_rm32(Emulator* emu, ModRM* modrm) {
    uint32_t value = get_rm32(emu, modrm);
    set_rm32(emu, modrm, value + 1);
}

void code_83(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    switch (modrm.opcode) {
        case 0:
            add_rm32_imm8(emu, &modrm);
            break;
        case 5:
            sub_rm32_imm8(emu, &modrm);
            break;
        default:
            printf("Not implemented yet: code=83/%d", modrm.opcode);
            exit(1);
    }
}

void code_ff(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    switch (modrm.opcode) {
        case 0:
            inc_rm32(emu, &modrm);
            break;
        default:
            printf("Not implemented yet: code=ff/%d", modrm.opcode);
            exit(1);
    }
}

// jump

void short_jump(Emulator* emu) {
    int8_t diff = get_signed_code8(emu, 1);
    emu->eip += diff + 2;
}

void near_jump(Emulator *emu) {
    int32_t diff = get_signed_code32(emu, 1);
    emu->eip += diff + 5;
}

void call_rel32(Emulator* emu) {
    int32_t diff = get_signed_code32(emu, 1);
    push32(emu, emu->eip + 5);
    emu->eip += diff + 5;
}

void ret(Emulator* emu) {
    emu->eip = pop32(emu);
}

// stack

void push_imm32(Emulator* emu) {
    uint32_t value = get_code32(emu, 1);
    push32(emu, value);
    emu->eip += 5;
}

void push_imm8(Emulator* emu) {
    uint8_t value = get_code8(emu, 1);
    push32(emu, value);
    emu->eip += 2;
}

void push_r32(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0x50;
    push32(emu, get_register32(emu, reg));
    emu->eip += 1;
}

void pop_r32(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0x58;
    set_register32(emu, reg, pop32(emu));
    emu->eip += 1;
}

void leave(Emulator* emu) {
    uint32_t ebp = get_register32(emu, EBP);
    set_register32(emu, ESP, ebp);
    set_register32(emu, EBP, pop32(emu));
    emu->eip += 1;
}

void init_instructions(void) {
    memset(instructions, 0, sizeof(instructions));

    instructions[0x01] = add_rm32_r32;

    for (int i = 0; i < 8; i++) {
        instructions[0x50 + i] = push_r32;
    }

    for (int i = 0; i < 8; i++) {
        instructions[0x58 + i] = pop_r32;
    }

    instructions[0x68] = push_imm32;
    instructions[0x6A] = push_imm8;

    instructions[0x83] = code_83;
    instructions[0x89] = mov_rm32_r32;
    instructions[0x8B] = mov_r32_rm32;

    for (int i = 0; i < 8; i++) {
        instructions[0xB8 + i] = mov_r32_imm32;
    }

    instructions[0xC3] = ret;
    instructions[0xC7] = mov_rm32_imm32;
    instructions[0xC9] = leave;

    instructions[0xE8] = call_rel32;
    instructions[0xE9] = near_jump;
    instructions[0xEB] = short_jump;

    instructions[0xFF] = code_ff;
}