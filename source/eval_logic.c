#include "eval_logic.h"
#include "lexeme.h"
#include "evaluate.h"
#include <stdlib.h>
#include <stdio.h>

int is_true(Lexeme *l)
{
    return l->type == TRUE;
}

int is_false(Lexeme *l)
{
    return l->type == FALSE;
}

Lexeme *eval_not(Lexeme *tree,Lexeme *env)
{
    Lexeme *operand = eval(tree->right,env);

    if (is_false(operand)) 
        return true_Lexeme();
    else
        return false_Lexeme();
}

// short circuiting

Lexeme *eval_and(Lexeme *tree,Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);

    if (is_false(l_operand))
        return false_Lexeme();

    Lexeme *r_operand = eval(tree->left,env);

    if (is_false(r_operand))
        return false_Lexeme();

    return true_Lexeme();
}

Lexeme *eval_or(Lexeme *tree,Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);

    if (is_true(l_operand))
        return true_Lexeme();

    Lexeme *r_operand = eval(tree->left,env);

    if (is_true(r_operand))
        return true_Lexeme();

    return false_Lexeme();
}
