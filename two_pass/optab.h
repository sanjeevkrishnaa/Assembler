#ifndef OPTAB_H
#define OPTAB_H

typedef struct {
    char mnemonic[5];
    int opcode;  // 1 byte opcode stored in int
} OPTAB_ENTRY;

// Returns 1 if found and writes opcode into *opcode_out; else returns 0.
int optab_lookup(const char *mnemonic, int *opcode_out);

#endif
