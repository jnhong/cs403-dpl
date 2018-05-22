#ifndef EVAL_MATH_H
#define EVAL_MATH_H
#include "lexeme.h"

Lexeme *eval_plus(Lexeme *tree, Lexeme *env);
Lexeme *eval_minus(Lexeme *tree, Lexeme *env);

Lexeme *eval_times(Lexeme *tree, Lexeme *env);
Lexeme *eval_divide(Lexeme *tree, Lexeme *env);
Lexeme *eval_pow(Lexeme *tree, Lexeme *env);
Lexeme *eval_remainder(Lexeme *tree, Lexeme *env);
Lexeme *eval_div(Lexeme *tree, Lexeme *env);
Lexeme *eval_mod(Lexeme *tree, Lexeme *env);
Lexeme *eval_unary_plus(Lexeme *tree, Lexeme *env);
Lexeme *eval_unary_minus(Lexeme *tree, Lexeme *env);
Lexeme *eval_abs(Lexeme *tree, Lexeme *env);

#endif
