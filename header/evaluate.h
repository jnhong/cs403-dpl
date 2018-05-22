#ifndef EVALUATE_H
#define EVALUATE_H
#include "lexeme.h"

void nullcheck(void *p, char *caller);
Lexeme *create_env(void);
Lexeme *extend_env(Lexeme *env, Lexeme *vars, Lexeme *vals);
Lexeme *eval(Lexeme *tree, Lexeme *env);
void init_builtins(Lexeme *env);
Lexeme *display_env(Lexeme *env);
Lexeme *display_local_env(Lexeme *env);
void return_error(char *calling_location);

#endif
