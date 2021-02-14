//
// Created by Kohei Shiraga on 2021/02/11.
//

#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "io.h"
#include "modrm.h"
#include "bios.h"

instruction_func_t* instructions[256];

// move

void mov_r8_imm8(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0xB0;
    set_register8(emu, reg, get_code8(emu, 1));
    emu->eip += 2;
}

void mov_rm8_r8(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    uint32_t r8 = get_r8(emu, &modrm);
    set_rm8(emu, &modrm, r8);
}


void mov_r8_rm8(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    uint32_t rm8 = get_rm8(emu, &modrm);
    set_r8(emu, &modrm, rm8);
}

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
    uint64_t result = (uint64_t) rm32 - (uint64_t) imm8;
    emu->eip += 1;
    set_rm32(emu, modrm, rm32 - imm8);
    update_eflags_sub(emu, rm32, imm8, result);
}

void inc_r32(Emulator* emu, ModRM* modrm) {
    uint8_t reg = get_code8(emu, 0) - 0x40;
    set_register32(emu, reg, get_register32(emu, reg) + 1);
    emu->eip += 1;
}

void inc_rm32(Emulator* emu, ModRM* modrm) {
    uint32_t value = get_rm32(emu, modrm);
    set_rm32(emu, modrm, value + 1);
}

// cmp

void cmp_r32_rm32(Emulator* emu) {
    emu->eip += 1;
    ModRM modrm;
    parse_modrm(emu, &modrm);

    uint32_t r32 = get_r32(emu, &modrm);
    uint32_t rm32 = get_rm32(emu, &modrm);
    uint64_t result = (uint64_t) r32 - (uint64_t) rm32;
    update_eflags_sub(emu, r32, rm32, result);
}

void cmp_rm32_imm8(Emulator* emu, ModRM* modrm) {
    uint32_t rm32 = get_rm32(emu, modrm);
    uint32_t imm8 = (int32_t) get_signed_code8(emu, 0);
    uint64_t result = (uint64_t) rm32 - (uint64_t) imm8;
    update_eflags_sub(emu, rm32, imm8, result);
}

void cmp_al_imm8(Emulator* emu) {
    uint8_t value = get_code8(emu, 1);
    uint8_t al = get_register8(emu, AL);
    uint64_t result = (uint64_t) al - (uint64_t) value;
    update_eflags_sub(emu, al, value, result);
    emu->eip += 2;
}

void cmp_eax_imm32(Emulator* emu) {
    uint32_t value = get_code32(emu, 1);
    uint32_t eax = get_register32(emu, EAX);
    uint64_t result = (uint64_t) eax - (uint64_t) value;
    update_eflags_sub(emu, eax, value, result);
    emu->eip += 5;
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
        case 7:
            cmp_rm32_imm8(emu, &modrm);
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

#define DEFINE_JX(flag, is_flag) \
static void j ## flag(Emulator* emu) { \
  int diff = is_flag(emu) ? get_signed_code8(emu, 1) : 0; \
  emu->eip += diff + 2; \
} \
static void jn ## flag(Emulator* emu) { \
  int diff = !is_flag(emu) ? get_signed_code8(emu, 1) : 0; \
  emu->eip += diff + 2; \
} \

DEFINE_JX(c, is_carry)
DEFINE_JX(z, is_zero)
DEFINE_JX(s, is_sign)
DEFINE_JX(o, is_overflow)

void jl(Emulator* emu) {
    int diff = (is_sign(emu) != is_overflow(emu)) ? get_signed_code8(emu, 1) : 0;
    emu->eip += (diff + 2);
}

void jle(Emulator* emu) {
    int diff = (is_zero(emu) || (is_sign(emu) != is_overflow(emu))) ? get_signed_code8(emu, 1) : 0;
    emu->eip += (diff + 2);
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

// input / output

void in_al_dx(Emulator* emu) {
    uint16_t address = get_register32(emu, EDX) & 0xffff;
    uint8_t value = io_in8(address);
    set_register8(emu, AL, value);
    emu->eip += 1;
}

void out_dx_al(Emulator* emu) {
    uint16_t address = get_register32(emu, EDX) & 0xffff;
    uint8_t value = get_register8(emu, AL);
    io_out8(address, value);
    emu->eip += 1;
}

// interruption

void swi(Emulator* emu) {
    uint8_t int_index = get_code8(emu, 1);
    emu->eip += 2;

    switch (int_index) {
        case 0x10:
            bios_video(emu);
            break;
        default:
            printf("Unknown interruption: 0x%02x\n", int_index);
    }
}

void init_instructions(void) {
    memset(instructions, 0, sizeof(instructions));

    instructions[0x01] = add_rm32_r32;

    instructions[0x3B] = cmp_r32_rm32;
    instructions[0x3C] = cmp_al_imm8;
    instructions[0x3D] = cmp_eax_imm32;

    for (int i = 0; i < 8; i++) {
        instructions[0x40 + i] = inc_r32;
    }

    for (int i = 0; i < 8; i++) {
        instructions[0x50 + i] = push_r32;
    }

    for (int i = 0; i < 8; i++) {
        instructions[0x58 + i] = pop_r32;
    }

    instructions[0x68] = push_imm32;
    instructions[0x6A] = push_imm8;

    instructions[0x70] = jo;
    instructions[0x71] = jno;
    instructions[0x72] = jc;
    instructions[0x73] = jnc;
    instructions[0x74] = jz;
    instructions[0x75] = jnz;
    instructions[0x78] = js;
    instructions[0x79] = jns;
    instructions[0x7C] = jl;
    instructions[0x7E] = jle;

    instructions[0x83] = code_83;
    instructions[0x88] = mov_rm8_r8;
    instructions[0x89] = mov_rm32_r32;
    instructions[0x8A] = mov_r8_rm8;
    instructions[0x8B] = mov_r32_rm32;

    for (int i = 0; i < 8; i++) {
        instructions[0xB0 + i] = mov_r8_imm8;
    }
    for (int i = 0; i < 8; i++) {
        instructions[0xB8 + i] = mov_r32_imm32;
    }

    instructions[0xC3] = ret;
    instructions[0xCD] = swi;
    instructions[0xC7] = mov_rm32_imm32;
    instructions[0xC9] = leave;

    instructions[0xE8] = call_rel32;
    instructions[0xE9] = near_jump;
    instructions[0xEB] = short_jump;
    instructions[0xEC] = in_al_dx;
    instructions[0xEE] = out_dx_al;

    instructions[0xFF] = code_ff;
}