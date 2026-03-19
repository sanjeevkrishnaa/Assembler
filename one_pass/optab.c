#include <string.h>
#include "optab.h"

static const OPTAB_ENTRY OPTAB[] = {
    {"ADD",   0x18},
    {"SUB",   0x1C},
    {"MUL",   0x20},
    {"DIV",   0x24},
    {"COMP",  0x28},

    {"J",     0x3C},
    {"JLT",   0x38},
    {"JEQ",   0x30},
    {"JGT",   0x34},

    {"JSUB",  0x48},
    {"RSUB",  0x4C},
    {"TIX",   0x2C},

    {"LDA",   0x00},
    {"LDX",   0x04},
    {"LDL",   0x08},

    {"STA",   0x0C},
    {"STX",   0x10},
    {"STL",   0x14},

    {"LDCH",  0x50},
    {"STCH",  0x54},

    {"RD",    0xD8},
    {"WD",    0xDC},
    {"TD",    0xE0}
};

static const int N_OPCODES = (int)(sizeof(OPTAB) / sizeof(OPTAB[0]));

int optab_lookup(const char *mnemonic, int *opcode_out) {
    for (int i = 0; i < N_OPCODES; i++) {
        if (strcmp(OPTAB[i].mnemonic, mnemonic) == 0) {
            *opcode_out = OPTAB[i].opcode;
            return 1;
        }
    }
    return 0;
}
