#ifndef EVAL_LOGIC_H
#define EVAL_LOGIC_H
#include "lexeme.h"

int is_false(Lexeme *l);
int is_true(Lexeme *l);
Lexeme *eval_not(Lexeme *tree,Lexeme *env);
Lexeme *eval_and(Lexeme *tree,Lexeme *env);
Lexeme *eval_or(Lexeme *tree,Lexeme *env);

#endif
