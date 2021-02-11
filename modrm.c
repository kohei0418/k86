//
// Created by Kohei Shiraga on 2021/02/11.
//

#include <string.h>
#include <stdlib.h>
#include "emulator.h"
#include "modrm.h"

void parse_modrm(Emulator* emu, ModRM* modrm) {
    memset(modrm, 0, sizeof(ModRM));

    uint8_t code = get_code8(emu, 0);
    modrm->mod = ((code & 0xC0) >> 6);
    modrm->opcode = ((code & 0x38) >> 3);
    modrm->rm = code & 0x07;
    emu->eip += 1;

    if (modrm->mod != 3 && modrm->rm == 4) {
        modrm->sib = get_code8(emu, 0);
        emu->eip += 1;
    }

    if ((modrm->mod == 0 && modrm->rm == 5) || modrm->mod == 2) {
        modrm->disp32 = get_signed_code32(emu, 0);
        emu->eip += 4;
    } else if (modrm->mod == 1) {
        modrm->disp8 = get_signed_code8(emu, 0);
        emu->eip += 1;
    }
}

uint32_t get_register32(Emulator* emu, uint8_t rm) {
    return emu->registers[rm];
}

void set_register32(Emulator* emu, uint8_t rm, uint32_t value) {
    emu->registers[rm] = value;
}

uint32_t get_memory8(Emulator* emu, uint32_t address) {
    return emu->memory[address];
}

uint32_t get_memory32(Emulator* emu, uint32_t address) {
    uint32_t ret = 0;
    for(int i = 0; i < 4; i++) {
        ret |= get_memory8(emu, address + i) << (i * 8);
    }
    return ret;
}

void set_memory8(Emulator* emu, uint32_t address, uint32_t value) {
    emu->memory[address] = value & 0xFF;
}

void set_memory32(Emulator* emu, uint32_t address, uint32_t value) {
    for (int i = 0; i < 4; i++) {
        set_memory8(emu, address + i, value >> (i * 8));
    }
}

uint32_t calc_memory_address(Emulator* emu, ModRM* modrm) {
    if (modrm->mod == 0) {
        if (modrm->rm == 4) {
            printf("Not implemented yet: mod=%d, rm=%d", modrm->mod, modrm->rm);
            exit(1);
        } else if (modrm->rm == 5) {
            return modrm->disp32;
        } else {
            return get_register32(emu, modrm->rm);
        }
    } else if (modrm->mod == 1) {
        if (modrm->rm == 4) {
            printf("Not implemented yet: mod=%d, rm=%d", modrm->mod, modrm->rm);
            exit(1);
        } else {
            return get_register32(emu, modrm->rm) + modrm->disp8;
        }
    } else if (modrm->mod == 2) {
        if (modrm->rm == 4) {
            printf("Not implemented yet: mod=%d, rm=%d", modrm->mod, modrm->rm);
            exit(1);
        } else {
            return get_register32(emu, modrm->rm) + modrm->disp32;
        }
    } else {
        printf("Not implemented yet: mod=%d, rm=%d", modrm->mod, modrm->rm);
        exit(1);
    }
}

uint32_t get_r32(Emulator* emu, ModRM* modrm) {
    return get_register32(emu, modrm->reg_index);
}

void set_r32(Emulator* emu, ModRM* modrm, uint32_t value) {
    set_register32(emu, modrm->reg_index, value);
}

uint32_t get_rm32(Emulator* emu, ModRM* modrm) {
    if (modrm->mod == 3) {
        return get_register32(emu, modrm->rm);
    } else {
        uint32_t address = calc_memory_address(emu, modrm);
        return get_memory32(emu, address);
    }
}

void set_rm32(Emulator* emu, ModRM* modrm, uint32_t value) {
    if (modrm->mod == 3) {
        set_register32(emu, modrm->rm, value);
    } else {
        uint32_t address = calc_memory_address(emu, modrm);
        set_memory32(emu, address, value);
    }
}

