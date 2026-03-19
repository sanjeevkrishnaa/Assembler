#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "optab.h"
#include "symtab.h"
#include "tokenize.h"
#include "buffer.h"

#define LINE_BUF_SIZE 128
#define MAX_LINES 1024

int LOCCTR = 0;
int start_addr = 0; // used to maintain the program start address
int program_started = 0;    // set to 1 after START has been found
int program_length = 0;
char* program_length_ptr; // pointer to program_length to update at the end of the program
char program_name[7];

int main(){
    // ========== SINGLE PASS ==============
    FILE *infile, *outfile;
    infile = fopen("sample_input.txt","r");
    outfile = fopen("output.txt","w");

    char line[LINE_BUF_SIZE]; // used to take input from file, line by line
    
    struct line_data lines[MAX_LINES];  // each entry in this array indicates one line in the output file
    for(int i=0;i<MAX_LINES;i++){
        lines[i].line_len = 0;
        lines[i].line_start_addr = 0;
        lines[i].nodes = NULL;
    }
    int out_lines = 0;  // maintain number of output lines

    int locctr_addr;

    struct buffer_node *buffer; // used to convert each line of assembly code into the output format

    while(fgets(line, sizeof(line), infile) != NULL){
        // Initialize the buffer for processing one line of assembly code
        buffer = (struct buffer_node*) malloc(sizeof(struct buffer_node)); // FREED LATER
        buffer->buf = (char*) malloc(70*sizeof(char)); // FREED LATER
        buffer->buf_len = 0;
        buffer->next = NULL;    

        int force_flush = 0;
        trim(line); // trim the line of all leading and trailing whitespaces
        if(line[0]=='\0' || line[0]=='.') continue; // ignore comments and empty lines

        locctr_addr = LOCCTR;

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
            sprintf(buffer->buf + buffer->buf_len, "%02X", opcode);
            buffer->buf_len += 2;
            buffer->buf[buffer->buf_len] = '\0';
            
            // handle the address acquired from operand(if it exists)
            int addr = 0;
            if(strcmp(operand,"-")!=0){ // if the operand field is not empty
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

                // if forward reference addr = 0x0000, else addr = actual address
                get_addr(symbol, &addr, buffer->buf + buffer->buf_len);
                if(indexed) addr |= 0x8000;
            }
            sprintf(buffer->buf + buffer->buf_len, "%04X", addr);
            buffer->buf_len += 4;
            buffer->buf[buffer->buf_len] = '\0';
            LOCCTR += 3;
        }
        else{   
            // Handle assembler directives
            if(strcmp(mnemonic, "START")==0){
                if(program_started) return -1; // double START errors out
                start_addr = (int) strtol(operand, NULL, 16);
                LOCCTR = start_addr;
                program_started = 1;
                
                // Handle the header line in output file
                lines[out_lines].nodes = (struct buffer_node*) malloc(sizeof(struct buffer_node));
                lines[out_lines].nodes->buf = (char*) malloc(70*sizeof(char)); 
                sprintf(lines[out_lines].nodes->buf, "H%-6s%06X%06X", program_name, start_addr, program_length);

                program_length_ptr = lines[out_lines].nodes->buf + 1 + 6 + 6;   // to be updated at the end of the program
                out_lines++;
                continue;
            }
            else if(strcmp(mnemonic, "END")==0){
                force_flush = 1;
                program_length = LOCCTR - start_addr;  
            }
            else if(strcmp(mnemonic, "WORD")==0){
                int op = strtol(operand, NULL, 10);
                sprintf(buffer->buf + buffer->buf_len, "%06X", op);
                buffer->buf_len += 6;
                buffer->buf[buffer->buf_len] = '\0';

                LOCCTR += 3;
            }
            else if(strcmp(mnemonic, "RESW")==0){
                force_flush = 1;

                LOCCTR += 3 * strtol(operand, NULL, 10);
            }
            else if(strcmp(mnemonic, "RESB")==0){
                force_flush = 1;

                LOCCTR += strtol(operand, NULL, 10);
            }
            else if(strcmp(mnemonic, "BYTE")==0){
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

                s = operand + 2;
                if(operand[0]=='C'){
                    while(*s != '\''){
                        sprintf(buffer->buf + buffer->buf_len, "%02X", *s);
                        buffer->buf_len += 2;
                        buffer->buf[buffer->buf_len] = '\0';
                        s++;
                    }
                }
                else if(operand[0]=='X'){
                    while(*s != '\''){
                        sprintf(buffer->buf + buffer->buf_len, "%c", *s);
                        buffer->buf_len++;
                        buffer->buf[buffer->buf_len] = '\0';
                        s++;
                    }
                }
            }
        }

        // Handle the buffer being added to the line records
        if(force_flush){
            if(lines[out_lines].line_len != 0){
                out_lines++;
            }
        }
        else{
            if(lines[out_lines].line_len + buffer->buf_len > 60){
                out_lines++;
                lines[out_lines].nodes = buffer;
                lines[out_lines].line_len = buffer->buf_len;
                lines[out_lines].line_start_addr = locctr_addr;
            }
            else{
                if(lines[out_lines].line_len==0){
                    lines[out_lines].line_start_addr = locctr_addr;
                    lines[out_lines].nodes = buffer;
                } 
                else{
                    struct buffer_node* cur = lines[out_lines].nodes;
                    while(cur->next != NULL) cur = cur->next;

                    cur->next = buffer;
                }
                lines[out_lines].line_len += buffer->buf_len;
            }
        }
        // Free unused allocated buffers
        if(buffer->buf_len==0){
            free(buffer->buf);
            free(buffer);
        }
    }

    // Handle the END line in output file
    lines[out_lines].nodes = (struct buffer_node*) malloc(sizeof(struct buffer_node));
    lines[out_lines].nodes->buf = (char*) malloc(70*sizeof(char)); 
    sprintf(lines[out_lines].nodes->buf, "E%06X", start_addr);

    sprintf(program_length_ptr, "%06X", program_length);    // update the program length

    fprintf(outfile, "%s\n", lines[0].nodes->buf);  // print the header line

    // Print line by line 
    for(int i=1;i<out_lines;i++){
        fprintf(outfile, "T%06X%02X", lines[i].line_start_addr, lines[i].line_len/2);
        struct buffer_node* cur = lines[i].nodes;
        while(cur != NULL){
            fprintf(outfile, "%s", cur->buf);
            struct buffer_node* temp = cur->next;
            // Free buffers after extracting all required information
            free(cur->buf);
            free(cur);
            cur = temp;
        }
        fprintf(outfile, "\n");
    }

    fprintf(outfile, "%s\n", lines[out_lines].nodes->buf);  // print the end line

    // Free all the remaining allocated memory
    free(lines[0].nodes->buf);
    free(lines[0].nodes);
    free(lines[out_lines].nodes->buf);
    free(lines[out_lines].nodes);

    fclose(infile);
    fclose(outfile);
    return 0;
}