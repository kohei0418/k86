//
// Created by Kohei Shiraga on 2021/02/11.
//

#ifndef K86_EMULATOR_H
#define K86_EMULATOR_H

#include <stdio.h>

static const int MEMORY_SIZE = 1024 * 1024;
enum Register {
    EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, REGISTERS_COUNT,
    AL = EAX, CL = ECX, DL = EDX, BL = EBX,
    AH = AL + 4, CH = CL + 4, DH = DL + 4, BH = BL + 4
};
static char* register_names[] = {
        "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"
};

#define CARRY_FLAG (1)
#define ZERO_FLAG (1 << 6)
#define SIGN_FLAG (1 << 7)
#define OVERFLOW_FLAG (1 << 11)

typedef struct {
    uint32_t registers[REGISTERS_COUNT];
    uint32_t eflags;
    uint8_t* memory;
    uint32_t eip;
} Emulator;

static uint32_t get_code8(Emulator* emu, int index) {
    return emu->memory[emu->eip + index];
}

static int32_t get_signed_code8(Emulator* emu, int index) {
    return (int8_t) get_code8(emu, index);
}

static uint32_t get_code32(Emulator* emu, int index) {
    uint32_t ret = 0;
    for (int i = 0; i < 4; i++) {
        ret |= get_code8(emu, index + i) << (i * 8);
    }
    return ret;
}

static int32_t get_signed_code32(Emulator* emu, int index) {
    return (int32_t) get_code32(emu, index);
}

static uint32_t get_register32(Emulator* emu, uint8_t rm) {
    return emu->registers[rm];
}

static uint8_t get_register8(Emulator* emu, int index) {
    if (index < 4) {
        return emu->registers[index] & 0xff;
    } else {
        return (emu->registers[index - 4] >> 8) & 0xff;
    }
}

static void set_register32(Emulator* emu, uint8_t rm, uint32_t value) {
    emu->registers[rm] = value;
}

static void set_register8(Emulator* emu, int index, uint8_t value) {
    if (index < 4) {
        emu->registers[index] = (get_register32(emu, index) & 0xffffff00) | ((uint32_t) value);
    } else {
        emu->registers[index - 4] = (get_register32(emu, index) & 0xffff00ff) | ((uint32_t) value << 8);
    }
}

static uint32_t get_memory8(Emulator* emu, uint32_t address) {
    return emu->memory[address];
}

static uint32_t get_memory32(Emulator* emu, uint32_t address) {
    uint32_t ret = 0;
    for(int i = 0; i < 4; i++) {
        ret |= get_memory8(emu, address + i) << (i * 8);
    }
    return ret;
}

static void set_memory8(Emulator* emu, uint32_t address, uint32_t value) {
    emu->memory[address] = value & 0xFF;
}

static void set_memory32(Emulator* emu, uint32_t address, uint32_t value) {
    for (int i = 0; i < 4; i++) {
        set_memory8(emu, address + i, value >> (i * 8));
    }
}

static void push32(Emulator* emu, uint32_t value) {
    uint32_t address = get_register32(emu, ESP) - 4;
    set_register32(emu, ESP, address);
    set_memory32(emu, address, value);
}

static uint32_t pop32(Emulator* emu) {
    uint32_t address = get_register32(emu, ESP);
    uint32_t ret = get_memory32(emu, address);
    set_register32(emu, ESP, address + 4);
    return ret;
}

static void set_carry(Emulator* emu, int is_carry) {
    if (is_carry) {
        emu->eflags |= CARRY_FLAG;
    } else {
        emu->eflags &= ~CARRY_FLAG;
    }
}

static void set_zero(Emulator* emu, int is_zero) {
    if (is_zero) {
        emu->eflags |= ZERO_FLAG;
    } else {
        emu->eflags &= ~ZERO_FLAG;
    }
}

static void set_sign(Emulator* emu, int is_sign) {
    if (is_sign) {
        emu->eflags |= SIGN_FLAG;
    } else {
        emu->eflags &= ~SIGN_FLAG;
    }
}

static void set_overflow(Emulator* emu, int is_overflow) {
    if (is_overflow) {
        emu->eflags |= OVERFLOW_FLAG;
    } else {
        emu->eflags &= ~OVERFLOW_FLAG;
    }
}

static int is_carry(Emulator* emu) {
    return (emu->eflags & CARRY_FLAG) != 0;
}

static int is_zero(Emulator* emu) {
    return (emu->eflags & ZERO_FLAG) != 0;
}

static int is_sign(Emulator* emu) {
    return (emu->eflags & SIGN_FLAG) != 0;
}

static int is_overflow(Emulator* emu) {
    return (emu->eflags & OVERFLOW_FLAG) != 0;
}

static void update_eflags_sub(Emulator* emu, uint32_t v1, uint32_t v2, uint64_t result) {
    int sign1 = v1 >> 31;
    int sign2 = v2 >> 31;
    int signr = (result >> 31) & 1;

    set_carry(emu, result >> 32);
    set_zero(emu, result == 0);
    set_sign(emu, signr);
    set_overflow(emu, sign1 != sign2 && sign1 != signr);
}

static void dump_registers(Emulator* emu) {
    for (int i = 0; i < REGISTERS_COUNT; i++) {
        printf("%s = %08x\n", register_names[i], emu->registers[i]);
    }

    printf("EIP = %08x\n", emu->eip);
}

static Emulator* create_emulator(size_t size, uint32_t eip, uint32_t esp) {
    Emulator* emu = malloc(sizeof(Emulator));
    emu->memory = malloc(size);
    memset(emu->registers, 0, sizeof(emu->registers));
    emu->eip = eip;
    emu->registers[ESP] = esp;

    return emu;
}

static void destroy_emulator(Emulator* emu) {
    free(emu->memory);
    free(emu);
}

#endif //K86_EMULATOR_H
