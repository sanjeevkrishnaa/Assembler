#ifndef SYMTAB_H
#define SYMTAB_H

#define MAX_LABEL 31
#define SYMTAB_CAPACITY 1024 

// forward reference maintenance
struct chain{
    char* loc;  // address of forward reference output
    struct chain* next; // next forward reference
};

typedef struct{
    char symbol[MAX_LABEL + 1];
    int address;
    int defined; // indicates whether the symbol has been defined and allotted an address yet
    struct chain* refs; // linked list storing all the forward references to the symbol
} SYMTAB_ENTRY;

int add_symbol(const char* symbol, int address);
int get_addr(const char* symbol, int* out_addr, char* buf_addr);

#endif