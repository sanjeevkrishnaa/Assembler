#ifndef BUFFER_H
#define BUFFER_H

// One node represents one line of assembly code, in the required output format
struct buffer_node{
    char* buf;  // text output
    int buf_len;    // length of text output
    struct buffer_node* next; // next encoding present in the same line of output file
};

// Struct stores all the necessary information for each line in output file
struct line_data{ 
    int line_len;   // length of line in output file
    int line_start_addr;    // starting address of line
    struct buffer_node* nodes;  // linked list of assembly lines encoded in this output line
};

#endif