#ifndef EVAL_BUILTIN_H
#define EVAL_BUILTIN_H
#include "lexeme.h"

Lexeme *eval_print(Lexeme *arglist);
Lexeme *eval_println(Lexeme *arglist);
Lexeme *eval_allocate(Lexeme *arglist);
Lexeme *eval_real(Lexeme *arglist);
Lexeme *eval_integer(Lexeme *arglist);

Lexeme *eval_make_list(Lexeme *arglist);
Lexeme *eval_cons(Lexeme *arglist);
Lexeme *eval_car(Lexeme *arglist);
Lexeme *eval_cdr(Lexeme *arglist);

Lexeme *eval_sizeof(Lexeme *arglist);

Lexeme *eval_is_uninitialized(Lexeme *arglist);
Lexeme *eval_is_array(Lexeme *arglist);
Lexeme *eval_is_closure(Lexeme *arglist);
Lexeme *eval_is_boolean(Lexeme *arglist);
Lexeme *eval_is_pair(Lexeme *arglist);
Lexeme *eval_is_list(Lexeme *arglist);
Lexeme *eval_is_nil(Lexeme *arglist);
Lexeme *eval_is_string(Lexeme *arglist);
Lexeme *eval_is_integer(Lexeme *arglist);
Lexeme *eval_is_real(Lexeme *arglist);
Lexeme *eval_is_return(Lexeme *arglist);

//Lexeme *builtin_eval(Lexeme *arglist);

#endif
