#ifndef EVAL_RELATIONAL_H
#define EVAL_RELATIONAL_H
#include "lexeme.h"

Lexeme *eval_equals(Lexeme *tree, Lexeme *env);
Lexeme *eval_neq(Lexeme *tree, Lexeme *env);
Lexeme *eval_lt(Lexeme *tree, Lexeme *env);
Lexeme *eval_gt(Lexeme *tree, Lexeme *env);
Lexeme *eval_lt_or_eq(Lexeme *tree, Lexeme *env);
Lexeme *eval_gt_or_eq(Lexeme *tree, Lexeme *env);

#endif
