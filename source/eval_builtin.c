#include "eval_builtin.h"
#include "evaluate.h"
#include "lexeme.h"
#include "parse.h"
#include "eval_logic.h"
#include <stdio.h>
#include <stdlib.h>

// assuming arglist has already been evaluated

static int comma_separate = 0;

Lexeme *eval_print(Lexeme *arglist)
{
    nullcheck(arglist,"eval_print()");

    Lexeme *arg;
    while (arglist->type != NIL)
    {
        arg = arglist->left;

        switch (arg->type)
        {
            case INTEGER:
                printf("%d",arg->value.integer);
                break;
            case REAL:
                printf("%f",arg->value.real);
                break;
            case STRING:
                printf("%s",arg->value.string);
                break;
            case TRUE :
                printf("true");
                break;
            case FALSE :
                printf("false");
                break;
            case NIL:
                printf("nil");
                break;
            case LINK:
                // temp must be used to return to previous state
                // which may be 1 because of nested lists
                {
                int temp = comma_separate;
                comma_separate = 1;
                printf("(");
                eval_print(arg);
                printf(")");
                comma_separate = temp;
                }
                break;
            case CLOSURE:
                printf("<closure>");
                break;
            default:
                fprintf(stderr,"print: argument not of printable type: %s\n",
                        type_to_string(arg->type));
                exit(EXIT_FAILURE);
        }

        if (comma_separate == 1 && arglist->right->type != NIL)
            printf(", ");

        arglist = arglist->right;
    }

    return new_Lexeme(UNINITIALIZED);
}

Lexeme *eval_println(Lexeme *arglist)
{
    nullcheck(arglist,"eval_println()");

    Lexeme *temp = eval_print(arglist);
    printf("\n");
    return temp;
}

Lexeme *eval_allocate(Lexeme *arglist)
{
    nullcheck(arglist,"eval_allocate()");

    check_argcount(1,"allocate()",arglist);

    Lexeme *arg = arglist->left;

    if (arg->type != INTEGER)
    {
        fprintf(stderr,"allocate(): cannot allocate non-integer units of memory: %s\n",
                type_to_string(arg->type));
        exit(EXIT_FAILURE);
    }

    Lexeme *array = new_Lexeme(ARRAY);
    array->array_size  = arg->value.integer;
    array->value.array = malloc(array->array_size * sizeof(Lexeme *));

    for (int i = 0; i < array->array_size; ++i)
        array->value.array[i] = new_Lexeme(UNINITIALIZED);

    return array;
}

Lexeme *eval_real(Lexeme *arglist)
{
    nullcheck(arglist,"eval_real()");

    check_argcount(1,"real()",arglist);

    Lexeme *arg = arglist->left;
    if (arg->type == REAL)
        return arg;
    if (arg->type == INTEGER)
    {
        arg->type = REAL;
        arg->value.real = (double) arg->value.integer;
        return arg;
    }
    fprintf(stderr,"real(): argument cannot be coerced to type real: %s\n",
            type_to_string(arg->type));
    exit(EXIT_FAILURE);
}

Lexeme *eval_integer(Lexeme *arglist)
{
    nullcheck(arglist,"eval_integer()");

    check_argcount(1,"integer()",arglist);

    Lexeme *arg = arglist->left;
    if (arg->type == INTEGER)
        return arg;
    if (arg->type == REAL)
    {
        arg->type = INTEGER;
        arg->value.integer = (int) arg->value.real;
        return arg;
    }

    fprintf(stderr,"integer(): argument cannot be coerced to type integer: %s\n",
            type_to_string(arg->type));
    exit(EXIT_FAILURE);
}

Lexeme *eval_make_list(Lexeme *arglist)
{
    nullcheck(arglist,"eval_make_list()");
    return arglist;
}

Lexeme *eval_cons(Lexeme *arglist)
{
    nullcheck(arglist,"eval_cons()");

    check_argcount(2,"cons()",arglist);

    Lexeme *car = arglist->left;
    Lexeme *cdr = arglist->right->left;

    Lexeme *pair = new_Lexeme(LINK);
    pair->left  = car;
    pair->right = cdr;
    return pair;
}

Lexeme *eval_car(Lexeme *arglist)
{
    nullcheck(arglist,"eval_car()");

    check_argcount(1,"car()",arglist);

    Lexeme *pair = arglist->left;

    if (pair->type != LINK)
    {
        fprintf(stderr,"car(): argument not a pair: %s\n",
                type_to_string(pair->type));
        exit(EXIT_FAILURE);
    }

    return pair->left;
}

Lexeme *eval_cdr(Lexeme *arglist)
{
    nullcheck(arglist,"eval_cdr()");

    check_argcount(1,"cdr()",arglist);

    Lexeme *pair = arglist->left;

    if (pair->type != LINK)
    {
        fprintf(stderr,"cdr(): argument not a pair: %s\n",
                type_to_string(pair->type));
        exit(EXIT_FAILURE);
    }

    return pair->right;
}

Lexeme *eval_sizeof(Lexeme *arglist)
{
    nullcheck(arglist,"sizeof()");

    check_argcount(1,"sizeof()",arglist);

    Lexeme *array = arglist->left;

    if (array->type != ARRAY)
    {
        fprintf(stderr,"sizeof(): argument not a array: %s\n",
                type_to_string(array->type));
        exit(EXIT_FAILURE);
    }

    Lexeme *size = new_Lexeme(INTEGER);
    size->value.integer = array->array_size;

    return size;
}


Lexeme *eval_is_real(Lexeme *arglist)
{
    nullcheck(arglist,"is_real()");

    check_argcount(1,"is_real()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == REAL)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_integer(Lexeme *arglist)
{
    nullcheck(arglist,"is_integer()");

    check_argcount(1,"is_integer()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == INTEGER)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_string(Lexeme *arglist)
{
    nullcheck(arglist,"is_string()");

    check_argcount(1,"is_string()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == STRING)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_pair(Lexeme *arglist)
{
    nullcheck(arglist,"is_pair()");

    check_argcount(1,"is_pair()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == LINK)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_list(Lexeme *arglist)
{
    nullcheck(arglist,"is_list()");

    check_argcount(1,"is_list()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == LINK || object->type == NIL)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_nil(Lexeme *arglist)
{
    nullcheck(arglist,"is_nil()");

    check_argcount(1,"is_nil()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == NIL)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_boolean(Lexeme *arglist)
{
    nullcheck(arglist,"is_boolean()");

    check_argcount(1,"is_boolean()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == TRUE || object->type == FALSE)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_closure(Lexeme *arglist)
{
    nullcheck(arglist,"is_closure()");

    check_argcount(1,"is_closure()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == CLOSURE)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_array(Lexeme *arglist)
{
    nullcheck(arglist,"is_array()");

    check_argcount(1,"is_array()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == ARRAY)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_uninitialized(Lexeme *arglist)
{
    nullcheck(arglist,"is_uninitialized()");

    check_argcount(1,"is_uninitialized()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == UNINITIALIZED)
        return true_Lexeme();
    else
        return false_Lexeme();
}

Lexeme *eval_is_return(Lexeme *arglist)
{
    nullcheck(arglist,"is_return()");

    check_argcount(1,"is_return()",arglist);

    Lexeme *object = arglist->left;

    if (object->type == RETURN)
        return true_Lexeme();
    else
        return false_Lexeme();
}

/*
Lexeme *builtin_eval(Lexeme *arglist)
{
    nullcheck(arglist,"eval_builtin()");

    if (arglist->type == NIL || arglist->right->type == NIL)
    {
        fprintf(stderr,"eval(): not enough arguments!\n");
        exit(EXIT_FAILURE);
    }

    if (arglist->right->right->type != NIL)
    {
        fprintf(stderr,"eval(): too many arguments!\n");
        exit(EXIT_FAILURE);
    }
    //TODO
}
*/
