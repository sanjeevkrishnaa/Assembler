#ifndef TOKENIZE_H
#define TOKENIZE_H

void trim(char *s);

int next_token(const char **p, char *out);

int is_mnemonic(const char *token);

#endif