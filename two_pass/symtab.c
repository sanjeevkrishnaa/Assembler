#include <string.h>
#include "symtab.h"

static SYMTAB_ENTRY SYMTAB[SYMTAB_CAPACITY];
int N_ENTRIES = 0;

int add_symbol(const char* symbol, int address){
    for(int i=0;i<N_ENTRIES;i++){
        if(strcmp(SYMTAB[i].symbol,symbol)==0){
            return 1; // indicating symbol has already been defined
        }
    }

    memcpy(SYMTAB[N_ENTRIES].symbol, symbol, strlen(symbol) + 1); 
    SYMTAB[N_ENTRIES].address = address;
    N_ENTRIES++;
    return 0; // indicating success
}

int get_addr(const char* symbol, int* out_addr){
    for(int i=0;i<N_ENTRIES;i++){
        if(strcmp(SYMTAB[i].symbol,symbol)==0){
            *out_addr = SYMTAB[i].address;  // address outputted into out_addr
            return 0;   // indicating success
        }
    }
    return 1; // 1 indicating symbol has no definition
}