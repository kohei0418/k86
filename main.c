#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator.h"
#include "instructions.h"

int main(int argc, char **argv) {
    FILE* binary;
    Emulator* emu;

    if(argc != 2) {
        printf("usage: k86 filename\n");
        return 1;
    }

    emu = create_emulator(MEMORY_SIZE, 0x7c00, 0x7c00);

    binary = fopen(argv[1], "rb");
    if(binary == NULL) {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }
    fread(emu->memory + 0x7c00, 1, 0x200, binary);
    fclose(binary);

    init_instructions();

    while (emu->eip < MEMORY_SIZE) {
        uint8_t code = get_code8(emu, 0);
        printf("EIP = %X, Code = %02X\n", emu->eip, code);

        if (instructions[code] == NULL) {
            printf("\nNULL instruction: %x\n", code);
            break;
        }

        instructions[code](emu);
        if (emu->eip == 0x00) {
            printf("\nHALT\n");
            break;
        }
    }

    dump_registers(emu);
    destroy_emulator(emu);
    return 0;
}
