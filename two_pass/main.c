#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "optab.h"
#include "symtab.h"
#include "tokenize.h"

#define LINE_BUF_SIZE 128

int LOCCTR = 0;
int start_addr = 0; // used to maintain the program start address
int program_started = 0; // set to 1 after START has been found
int program_length = 0; 
char program_name[7];

int main(){
    // ========== PASS 1 ==============
    FILE *infile, *intfile, *outfile;
    infile = fopen("sample_input.txt","r");
    intfile = fopen("intermediate.txt","w");
    char line[LINE_BUF_SIZE];   // used to take input from input file, line by line

    while(fgets(line, sizeof(line), infile) != NULL){
        trim(line); // trims the line of leading and trailing whitespaces
        if(line[0]=='\0' || line[0]=='.'){  // if empty line or a comment line, print as it is in intermediate file
            fprintf(intfile, "%s\n", line); 
            continue;
        }

        // TOKEN PARSING
        char t1[MAX_LABEL], t2[MAX_LABEL], t3[MAX_LABEL];   // possible tokens from line
        char label[MAX_LABEL], mnemonic[MAX_LABEL], operand[MAX_LABEL]; // mapped tokens from line

        const char* p = line;
        next_token(&p, t1);
        next_token(&p, t2);
        next_token(&p, t3);

        if(is_mnemonic(t1)){
            strcpy(label, "-");
            strcpy(mnemonic, t1);
            strcpy(operand, t2); 
        }
        else{
            strcpy(label, t1);
            if(program_started) add_symbol(label, LOCCTR);
            else strcpy(program_name, label);
            strcpy(mnemonic, t2);
            strcpy(operand, t3); 
        }
        
        int opcode;
        if(optab_lookup(mnemonic, &opcode)){    // if the mnemonic is present in the OPTAB
            fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
            LOCCTR += 3;
        }
        else{
            // Handle assembler directives
            if(strcmp(mnemonic, "START")==0){   
                if(program_started){
                    return -1;
                }
                start_addr = (int) strtol(operand, NULL, 16); // get the program start address
                LOCCTR = start_addr;
                program_started = 1;
                
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
            }
            else if(strcmp(mnemonic, "END")==0){
                program_length = LOCCTR - start_addr;
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
                break;    
            }
            else if(strcmp(mnemonic, "WORD")==0){
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
                LOCCTR += 3;
            }
            else if(strcmp(mnemonic, "RESW")==0){
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
                LOCCTR += 3 * strtol(operand, NULL, 10);
            }
            else if(strcmp(mnemonic, "RESB")==0){
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
                LOCCTR += strtol(operand, NULL, 10);
            }
            else if(strcmp(mnemonic, "BYTE")==0){
                fprintf(intfile, "%04X\t%s\t%s\t%s\n", LOCCTR, label, mnemonic, operand);
                char* s = operand + 2;
                int len = 0;
                while(*s != '\''){
                    len++;
                    s++;
                }
                if(operand[0]=='C'){
                    LOCCTR += len;
                }
                else if(operand[0]=='X'){
                    LOCCTR += (len+1)/2;
                }
            }
        }
    }
    fclose(infile);
    fclose(intfile);

    // ========== PASS 2 ==============
    intfile = fopen("intermediate.txt", "r");
    outfile = fopen("output.txt","w");

    // HEADER
    fprintf(outfile, "H%-6s%06X%06X\n", program_name, start_addr, program_length);  

    // TEXT
    char line_buffer[70];
    int line_len = 0;
    int line_start_addr = 0;

    char buffer[70];
    int buf_len = 0;

    while(fgets(line, sizeof(line), intfile) != NULL){
        int force_flush = 0;
        trim(line);
        if(line[0]=='\0' || line[0]=='.') continue;

        // TOKEN PARSING
        char addr[MAX_LABEL], label[MAX_LABEL], mnemonic[MAX_LABEL], operand[MAX_LABEL];

        const char* p = line;
        next_token(&p, addr);
        next_token(&p, label);
        next_token(&p, mnemonic);
        next_token(&p, operand);

        int opcode;
        if(optab_lookup(mnemonic,&opcode)){ // if the mnemonic belongs to OPTAB
            snprintf(buffer + buf_len, sizeof(buffer) - buf_len, "%02X", opcode);
            buf_len += 2;

            int addr = 0;
            if(strcmp(operand,"-")!=0){
                // Handle indexed addressing
                int indexed = 0;
                char symbol[MAX_LABEL];

                char *comma = strchr(operand,',');
                if(comma != NULL){
                    indexed = 1;
                    int len = comma - operand;
                    strncpy(symbol, operand, len);
                    symbol[len] = '\0';
                }
                else{
                    strcpy(symbol, operand);
                }

                get_addr(symbol, &addr);
                if(indexed) addr |= 0x8000;
            }

            snprintf(buffer + buf_len, sizeof(buffer) - buf_len, "%04X", addr);
            buf_len += 4;
        }
        else{
            // Handle assembler directives
            if(strcmp(mnemonic, "START")==0){
                continue;
            }
            else if(strcmp(mnemonic, "END")==0){
                force_flush = 1;

                get_addr(operand, &start_addr);
            }
            else if(strcmp(mnemonic, "WORD")==0){
                int op = strtol(operand, NULL, 10);
                snprintf(buffer + buf_len, sizeof(buffer) - buf_len, "%06X", op);
                buf_len += 6;
            }
            else if(strcmp(mnemonic, "RESW")==0){
                force_flush = 1;
            }
            else if(strcmp(mnemonic, "RESB")==0){
                force_flush = 1;
            }
            else if(strcmp(mnemonic, "BYTE")==0){
                char* s = operand + 2;
                if(operand[0]=='C'){
                    while(*s != '\''){
                        snprintf(buffer + buf_len, sizeof(buffer) - buf_len, "%02X", *s);
                        buf_len += 2;
                        s++;
                    }
                }
                else if(operand[0]=='X'){
                    while(*s != '\''){
                        snprintf(buffer + buf_len, sizeof(buffer) - buf_len, "%c", *s);
                        buf_len++;
                        s++;
                    }
                }
            }
        }

        // Handle the line buffer being outputted
        if(force_flush || line_len + buf_len > 60){
            if(line_len != 0){
                fprintf(outfile, "T%06X%02X%s\n", line_start_addr, line_len/2, line_buffer);
                line_len = 0;
            }
        }
        if(!force_flush){
            if(line_len==0) line_start_addr = strtol(addr, NULL, 16);
            
            strncpy(line_buffer + line_len, buffer, (size_t)buf_len);
            line_len += buf_len;
            line_buffer[line_len] = '\0';
        }
        buf_len = 0;
    }

    // THE END
    fprintf(outfile, "E%06X\n", start_addr);

    fclose(intfile);
    fclose(outfile);
    return 0;
}