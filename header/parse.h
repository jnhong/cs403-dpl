#ifndef PARSE_H
#define PARSE_H
#include "lexeme.h"

Lexeme *parse_program(char *filename);
void print_tree(Lexeme *tree,int d);
void check_argcount(int argcount, char *func, Lexeme *arglist);

#endif
