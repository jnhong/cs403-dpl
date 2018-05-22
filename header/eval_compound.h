#ifndef EVAL_COMPOUND_H
#define EVAL_COMPOUND_H
#include "lexeme.h"
#include "evaluate.h"
#include <stdlib.h>
#include <stdio.h>

Lexeme *eval_list(Lexeme *tree,Lexeme *env); 
Lexeme *eval_paren(Lexeme *tree,Lexeme *env);
Lexeme *eval_block(Lexeme *tree,Lexeme *env);
Lexeme *eval_body(Lexeme *tree,Lexeme *env);

#endif
