#include "eval_control.h"
#include "eval_logic.h"
#include "evaluate.h"
#include "lexeme.h"
#include <stdio.h>
#include <stdlib.h>

Lexeme *eval_if(Lexeme *tree,Lexeme *env)
{
    nullcheck(tree,"eval_if()");

    Lexeme *case_list = tree->right;

    Lexeme *current_case;
    Lexeme *condition, *truth, *body;

    while (case_list->type != NIL)
    {
        current_case = case_list->left;
        condition = current_case->left;
        body      = current_case->right;
        
        truth = eval(condition,env);
        if (!is_false(truth) && truth->type != RETURN)
            return eval(body,env);
        else if(truth->type == RETURN)
            return_error("if-condition");

        case_list = case_list->right;
    }

    return new_Lexeme(UNINITIALIZED);
}

Lexeme *eval_while(Lexeme *tree,Lexeme *env)
{
    nullcheck(tree,"eval_while()");

    Lexeme *precond   = tree->right->left;
    Lexeme *condition = tree->left;
    Lexeme *postcond  = tree->right->right;

    Lexeme *val = new_Lexeme(UNINITIALIZED);

    if (precond->right->type != NIL) 
        val = eval(precond,env);

    Lexeme *truth = eval(condition,env);
    while (!is_false(truth) && truth->type != RETURN)
    {
        if (truth->type == RETURN)
            return_error("while-condition");
        if (postcond->right->type != NIL)
            val = eval(postcond,env);
        if (precond->right->type  != NIL) 
            val = eval(precond,env);
        truth = eval(condition,env);
    }

    return val;
}
