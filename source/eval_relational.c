#include "lexeme.h"
#include "evaluate.h"
#include "eval_logic.h"
#include <stdlib.h>
#include <string.h>

// checked
Lexeme *eval_equals(Lexeme *tree, Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);
    Lexeme *r_operand = eval(tree->right,env);

    if (l_operand->type == INTEGER && r_operand->type == INTEGER)
    {
        if (l_operand->value.integer == r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == INTEGER && r_operand->type == REAL)
    {
        if ((double) l_operand->value.integer == r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == INTEGER)
    {
        if (l_operand->value.real == (double) r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == REAL)
    {
        if (l_operand->value.real == r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == r_operand->type)
    {
        switch (l_operand->type)
        {
            case TRUE:
                    return true_Lexeme();
            case FALSE:
                    return false_Lexeme();
            case STRING:
                if (strcmp(l_operand->value.string,r_operand->value.string) == 0)
                    return true_Lexeme();
                else
                    return false_Lexeme();
            case CLOSURE:
                //TODO structural equality
                if (l_operand == r_operand)
                    return true_Lexeme();
                else
                    return false_Lexeme();
            case ARRAY:
                if (l_operand->value.array == r_operand->value.array)
                    return true_Lexeme();
                else
                    return false_Lexeme();
            case NIL:
                return true_Lexeme();
            case UNINITIALIZED:
                fprintf(stderr,"equality: uninitialized variables.\n");
                exit(EXIT_FAILURE);
            default:
                return false_Lexeme();
        }
    }
    return false_Lexeme();
}

// checked
Lexeme *eval_neq(Lexeme *tree, Lexeme *env)
{
    Lexeme *val = eval_equals(tree,env);
    if (is_false(val))
        return true_Lexeme();
    else
        return false_Lexeme();
}

// checked
Lexeme *eval_lt(Lexeme *tree, Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);
    Lexeme *r_operand = eval(tree->right,env);

    if (l_operand->type == INTEGER && r_operand->type == INTEGER)
    {
        if (l_operand->value.integer < r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == INTEGER && r_operand->type == REAL)
    {
        if ((double) l_operand->value.integer < r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == INTEGER)
    {
        if (l_operand->value.real < (double) r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == REAL)
    {
        if (l_operand->value.real < r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == STRING && r_operand->type == STRING)
    {
        if (strcmp(l_operand->value.string,r_operand->value.string) < 0)
            return true_Lexeme();
        else
            return false_Lexeme();
    }

    fprintf(stderr,"eval_lt(): attempt to compare non-comparable objects: %s, %s\n",
            type_to_string(l_operand->type), type_to_string(r_operand->type));
    exit(EXIT_FAILURE);

}

// checked
Lexeme *eval_gt(Lexeme *tree, Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);
    Lexeme *r_operand = eval(tree->right,env);

    if (l_operand->type == INTEGER && r_operand->type == INTEGER)
    {
        if (l_operand->value.integer > r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == INTEGER && r_operand->type == REAL)
    {
        if ((double) l_operand->value.integer > r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == INTEGER)
    {
        if (l_operand->value.real > (double) r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == REAL)
    {
        if (l_operand->value.real > r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == STRING && r_operand->type == STRING)
    {
        if (strcmp(l_operand->value.string,r_operand->value.string) > 0)
            return true_Lexeme();
        else
            return false_Lexeme();
    }

    fprintf(stderr,"eval_gt(): attempt to compare non-comparable objects: %s, %s\n",
            type_to_string(l_operand->type), type_to_string(r_operand->type));
    exit(EXIT_FAILURE);

    return NULL;
}

// checked
Lexeme *eval_lt_or_eq(Lexeme *tree, Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);
    Lexeme *r_operand = eval(tree->right,env);

    if (l_operand->type == INTEGER && r_operand->type == INTEGER)
    {
        if (l_operand->value.integer <= r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == INTEGER && r_operand->type == REAL)
    {
        if ((double) l_operand->value.integer <= r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == INTEGER)
    {
        if (l_operand->value.real <= (double) r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == REAL)
    {
        if (l_operand->value.real <= r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == STRING && r_operand->type == STRING)
    {
        if (strcmp(l_operand->value.string,r_operand->value.string) <= 0)
            return true_Lexeme();
        else
            return false_Lexeme();
    }

    fprintf(stderr,"eval_lt_or_eq(): attempt to compare non-comparable objects: %s, %s\n",
            type_to_string(l_operand->type), type_to_string(r_operand->type));
    exit(EXIT_FAILURE);

}

// checked
Lexeme *eval_gt_or_eq(Lexeme *tree, Lexeme *env)
{
    Lexeme *l_operand = eval(tree->left,env);
    Lexeme *r_operand = eval(tree->right,env);

    if (l_operand->type == INTEGER && r_operand->type == INTEGER)
    {
        if (l_operand->value.integer >= r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == INTEGER && r_operand->type == REAL)
    {
        if ((double) l_operand->value.integer >= r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == INTEGER)
    {
        if (l_operand->value.real >= (double) r_operand->value.integer)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == REAL && r_operand->type == REAL)
    {
        if (l_operand->value.real >= r_operand->value.real)
            return true_Lexeme();
        else
            return false_Lexeme();
    }
    if (l_operand->type == STRING && r_operand->type == STRING)
    {
        if (strcmp(l_operand->value.string,r_operand->value.string) >= 0)
            return true_Lexeme();
        else
            return false_Lexeme();
    }

    fprintf(stderr,"eval_gt_or_eq(): attempt to compare non-comparable objects: %s, %s\n",
            type_to_string(l_operand->type), type_to_string(r_operand->type));
    exit(EXIT_FAILURE);

    return NULL;
}
