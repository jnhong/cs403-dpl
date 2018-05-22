#include "eval_compound.h"
#include "evaluate.h"
#include "lexeme.h"
#include <stdlib.h>
#include <stdio.h>

// NOTE: returns do not short circuit lists 
Lexeme *eval_list(Lexeme *tree,Lexeme *env) 
{
    nullcheck(tree,"eval_list()");

    if (tree->type == NIL)
        return nil_Lexeme();
    // now there is at least one

    // handle to top of resulting list
    Lexeme *result_root = new_Lexeme(LINK); 

    // construct downward usine result to iterate
    Lexeme *result = result_root;
    Lexeme *current = tree;

    result->left = eval(current->left,env);
//    if(result->left->type == RETURN)
//        return_error("list");

    current = current->right;

    while (current->type != NIL)
    {
        result->right = new_Lexeme(LINK);
        result = result->right;

        result->left = eval(current->left,env);
//        if(result->left->type == RETURN)
//            return_error("list");

        current = current->right;
    }
    result->right = nil_Lexeme();

    return result_root;
}

Lexeme *eval_body(Lexeme *tree,Lexeme *env) 
{
    nullcheck(tree,"eval_body()");

    Lexeme *result = NULL;
    Lexeme *current = tree->right;
    // stop if return->type == RETURN
    while (current->type != NIL && (result == NULL || result->type != RETURN))
    {
        result = eval(current->left,env);
        current = current->right;
    }
    return result;
}

Lexeme *eval_paren(Lexeme *tree,Lexeme *env)
{
    nullcheck(tree,"eval_paren()");
    return eval(tree->right,env);
}

Lexeme *eval_block(Lexeme *tree,Lexeme *env)
{
    nullcheck(tree,"eval_block()");
    // private environment
    Lexeme *block_env = extend_env(env,nil_Lexeme(),nil_Lexeme());
    return eval(tree->right,block_env);
}

