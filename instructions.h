//
// Created by Kohei Shiraga on 2021/02/11.
//

#ifndef K86_INSTRUCTIONS_H
#define K86_INSTRUCTIONS_H

#include "emulator.h"

void init_instructions(void);
typedef void instruction_func_t(Emulator*);
extern instruction_func_t* instructions[256];

#endif //K86_INSTRUCTIONS_H
