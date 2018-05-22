#ifndef EVAL_CONTROL_H
#define EVAL_CONTROL_H
#include "lexeme.h"

Lexeme *eval_if(Lexeme *tree,Lexeme *env);
Lexeme *eval_while(Lexeme *tree,Lexeme *env);

#endif
