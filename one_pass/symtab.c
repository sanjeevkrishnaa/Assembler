#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtab.h"

static SYMTAB_ENTRY SYMTAB[SYMTAB_CAPACITY];
int N_ENTRIES = 0;

int add_symbol(const char* symbol, int address){
    for(int i=0;i<N_ENTRIES;i++){
        if(strcmp(SYMTAB[i].symbol,symbol)==0){
            if(SYMTAB[i].defined==1) return 1; // indicating symbol has already been defined
            else{
                SYMTAB[i].defined = 1;
                SYMTAB[i].address = address; // set address

                // Go back and fix up all forward references to this symbol
                struct chain* cur = SYMTAB[i].refs;
                while(cur != NULL){
                    if(*cur->loc=='8') address |= 0x8000; // indexed addressing 
                    char addr[5];
                    snprintf(addr, sizeof(addr), "%04X", address);
                    memcpy(cur->loc, addr, 4);
                    cur = cur->next;
                }
                SYMTAB[i].refs = NULL;

                return 2;   // indicating successful address allotment and fixing up of all forward references
            }
        }
    }

    memcpy(SYMTAB[N_ENTRIES].symbol, symbol, strlen(symbol) + 1); 
    SYMTAB[N_ENTRIES].address = address;
    SYMTAB[N_ENTRIES].defined = 1;
    SYMTAB[N_ENTRIES].refs = NULL;
    N_ENTRIES++;
    return 0;
}

int get_addr(const char* symbol, int* out_addr, char* buf_addr){
    for(int i=0;i<N_ENTRIES;i++){
        if(strcmp(SYMTAB[i].symbol,symbol)==0){
            if(SYMTAB[i].defined==1){   // if already defined, get address
                *out_addr = SYMTAB[i].address;
                return 0;
            }
            else{   // if undefined, return address as 0, and add to forward reference list
                struct chain* cur = SYMTAB[i].refs;
                while(cur->next != NULL) cur = cur->next;

                cur->next = (struct chain *) malloc(sizeof(struct chain));
                cur->next->loc = buf_addr;
                cur->next->next = NULL;
                return 1;
            }
        }
    }
    
    // Add new undefined SYMTAB entry, and add to forward reference list
    memcpy(SYMTAB[N_ENTRIES].symbol, symbol, strlen(symbol) + 1); 
    SYMTAB[N_ENTRIES].address = 0;
    SYMTAB[N_ENTRIES].defined = 0;
    SYMTAB[N_ENTRIES].refs = (struct chain *) malloc(sizeof(struct chain));
    SYMTAB[N_ENTRIES].refs->loc = buf_addr;
    SYMTAB[N_ENTRIES].refs->next = NULL;

    N_ENTRIES++;
    return 2;
}