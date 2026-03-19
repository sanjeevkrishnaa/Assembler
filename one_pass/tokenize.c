#include <ctype.h>
#include <string.h>
#include "symtab.h"
#include "optab.h"

void trim(char *s){
    char *start = s;
    char *end;

    while(*start==' ' || *start=='\t') start++;

    if(*start=='\0'){
        *s = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while(end>start && (*end==' ' || *end=='\t' || *end=='\n')) end--;

    *(end+1) = '\0';

    // Move trimmed string to beginning 
    if(start != s) memmove(s, start, end - start + 2);
}

int next_token(const char **p, char *out){
    const char *s = *p;
    int i = 0;

    // Skip leading whitespace 
    while (*s && isspace((unsigned char)*s)) s++;

    if(*s=='\0') {          // no more tokens
        *p = s;
        strcpy(out,"-");
        return 0;
    }

    // Copy token until next whitespace or end
    while(*s && !isspace((unsigned char)*s)){
        if(i+1<MAX_LABEL) {
            out[i++] = *s;
        }
        s++;
    }
    out[i]='\0';

    *p = s;
    return 1;
}

int is_mnemonic(const char *token){
    int temp;
    return (optab_lookup(token, &temp) || 
            !strcmp(token, "START") || 
            !strcmp(token, "END") ||
            !strcmp(token, "WORD") ||
            !strcmp(token, "RESW") ||
            !strcmp(token, "RESB") ||
            !strcmp(token, "BYTE") );
}