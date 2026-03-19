#ifndef SYMTAB_H
#define SYMTAB_H

#define MAX_LABEL 31
#define SYMTAB_CAPACITY 1024 

typedef struct{
    char symbol[MAX_LABEL + 1];
    int address;
} SYMTAB_ENTRY;

int add_symbol(const char* symbol, int address);
int get_addr(const char* symbol, int* out_addr);
#endif