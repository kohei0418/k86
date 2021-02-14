//
// Created by Kohei Shiraga on 2021/02/11.
//

#ifndef K86_MODRM_H
#define K86_MODRM_H

#include <stdint.h>

typedef struct {
    uint8_t mod;
    union {
        uint8_t opcode;
        uint8_t reg_index;
    };
    uint8_t rm;

    uint8_t sib;
    union {
        uint8_t disp8;
        uint32_t disp32;
    };
} ModRM;

void parse_modrm(Emulator* emu, ModRM* modRm);

uint8_t get_r8(Emulator* emu, ModRM* modRm);
void set_r8(Emulator*, ModRM*, uint8_t);
uint8_t get_rm8(Emulator*, ModRM*);
void set_rm8(Emulator*, ModRM*, uint8_t);

uint32_t get_r32(Emulator* emu, ModRM* modrm);
void set_r32(Emulator* emu, ModRM* modrm, uint32_t value);
uint32_t get_rm32(Emulator* emu, ModRM* modrm);
void set_rm32(Emulator* emu, ModRM* modrm, uint32_t value);

#endif //K86_MODRM_H
