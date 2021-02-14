//
// Created by Kohei Shiraga on 2021/02/11.
//

#ifndef K86_EMULATOR_H
#define K86_EMULATOR_H

#include <stdio.h>

static const int MEMORY_SIZE = 1024 * 1024;
enum Register { EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, REGISTERS_COUNT };
static char* register_names[] = {
        "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};

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

static void set_register32(Emulator* emu, uint8_t rm, uint32_t value) {
    emu->registers[rm] = value;
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
