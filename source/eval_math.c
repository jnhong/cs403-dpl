#include "evaluate.h"
#include "eval_math.h"
#include "lexeme.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// checked
Lexeme *eval_plus(Lexeme *tree, Lexeme *env)
{
    Lexeme *augend = eval(tree->left,env);
    if (augend->type == TRUE) return true_Lexeme();
    Lexeme *addend = eval(tree->right,env);
    if (addend->type == TRUE) return true_Lexeme();
    if (augend->type == FALSE && addend->type == FALSE) return false_Lexeme();

    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (augend->type == INTEGER && addend->type == INTEGER)
    {
        result->type = INTEGER;
        result->value.integer = augend->value.integer + addend->value.integer;
        return result;
    }
    if (augend->type == REAL && addend->type == INTEGER)
    {
        result->type = REAL;
        result->value.real = augend->value.real + (double) addend->value.integer;
        return result;
    }
    if (augend->type == INTEGER && addend->type == REAL)
    {
        result->type = REAL;
        result->value.real = (double) augend->value.integer + addend->value.real;
        return result;
    }
    if (augend->type == REAL && addend->type == REAL)
    {
        result->type = REAL;
        result->value.real = augend->value.real + addend->value.real;
        return result;
    }
    if (augend->type == STRING && addend->type == STRING)
    {
        Lexeme *s = new_Lexeme(STRING);
        int length = strlen(augend->value.string) + strlen(addend->value.string) + 1;
        s->value.string = malloc(length * sizeof(char));
        s->value.string[0] = '\0';
        s->value.string = strcat(s->value.string,augend->value.string);
        s->value.string = strcat(s->value.string,addend->value.string);
        return s;
    }
    fprintf(stderr,"+: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(augend->type),type_to_string(addend->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_minus(Lexeme *tree, Lexeme *env)
{
    Lexeme *minuend    = eval(tree->left,env);
    Lexeme *subtrahend = eval(tree->right,env);
    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (minuend->type == INTEGER && subtrahend->type == INTEGER)
    {
        result->type = INTEGER;
        result->value.integer = minuend->value.integer - subtrahend->value.integer;
        return result;
    }
    if (minuend->type == REAL && subtrahend->type == INTEGER)
    {
        result->type = REAL;
        result->value.real = minuend->value.real - (double) subtrahend->value.integer;
        return result;
    }
    if (minuend->type == INTEGER && subtrahend->type == REAL)
    {
        result->type = REAL;
        result->value.real = (double) minuend->value.integer - subtrahend->value.real;
        return result;
    }
    if (minuend->type == REAL && subtrahend->type == REAL)
    {
        result->type = REAL;
        result->value.real = minuend->value.real - subtrahend->value.real;
        return result;
    }
    fprintf(stderr,"-: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(minuend->type),type_to_string(subtrahend->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_times(Lexeme *tree, Lexeme *env)
{
    Lexeme *multiplicand = eval(tree->left,env);
    if (multiplicand->type == FALSE) return false_Lexeme();
    Lexeme *multiplier   = eval(tree->right,env);
    if (multiplier->type == FALSE) return false_Lexeme();
    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (multiplicand->type == TRUE && multiplier->type == TRUE) return true_Lexeme();


    if (multiplicand->type == INTEGER && multiplier->type == INTEGER)
    {
        result->type = INTEGER;
        result->value.integer = multiplicand->value.integer * multiplier->value.integer;
        return result;
    }
    if (multiplicand->type == REAL && multiplier->type == INTEGER)
    {
        result->type = REAL;
        result->value.real = multiplicand->value.real * (double) multiplier->value.integer;
        return result;
    }
    if (multiplicand->type == INTEGER && multiplier->type == REAL)
    {
        result->type = REAL;
        result->value.real = (double) multiplicand->value.integer * multiplier->value.real;
        return result;
    }
    if (multiplicand->type == REAL && multiplier->type == REAL)
    {
        result->type = REAL;
        result->value.real = multiplicand->value.real * multiplier->value.real;
        return result;
    }
    fprintf(stderr,"*: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(multiplicand->type),type_to_string(multiplier->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_divide(Lexeme *tree, Lexeme *env)
{
    Lexeme *dividend = eval(tree->left,env);
    Lexeme *divisor  = eval(tree->right,env);
    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (dividend->type == INTEGER && divisor->type == INTEGER)
    {
        if (divisor->value.integer == 0)
        {
            fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
            exit(EXIT_FAILURE);
        }
        result->type = INTEGER;
        result->value.integer = dividend->value.integer / divisor->value.integer;
        return result;
    }
    if (dividend->type == REAL && divisor->type == INTEGER)
    {
        if (divisor->value.integer == 0)
        {
            fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
            exit(EXIT_FAILURE);
        }
        result->type = REAL;
        result->value.real = dividend->value.real / (double) divisor->value.integer;
        return result;
    }
    if (dividend->type == INTEGER && divisor->type == REAL)
    {
        if (divisor->value.real == 0)
        {
            fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
            exit(EXIT_FAILURE);
        }
        result->type = REAL;
        result->value.real = (double) dividend->value.integer / divisor->value.real;
        return result;
    }
    if (dividend->type == REAL && divisor->type == REAL)
    {
        if (divisor->value.real == 0)
        {
            fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
            exit(EXIT_FAILURE);
        }
        result->type = REAL;
        result->value.real = dividend->value.real / divisor->value.real;
        return result;
    }
    fprintf(stderr,"/: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(dividend->type),type_to_string(divisor->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_pow(Lexeme *tree, Lexeme *env)
{
    Lexeme *base     = eval(tree->left,env);
    Lexeme *exponent = eval(tree->right,env);
    Lexeme *result = new_Lexeme(REAL);
    if (base->type == INTEGER && exponent->type == INTEGER)
    {
        result->value.real = pow((double) base->value.integer,(double) exponent->value.integer);
        return result;
    }
    if (base->type == INTEGER && exponent->type == REAL)
    {
        result->value.real = pow((double) base->value.integer,exponent->value.real);
        return result;
    }
    if (base->type == REAL && exponent->type == INTEGER)
    {
        result->value.real = pow(base->value.real,(double) exponent->value.integer);
        return result;
    }
    if (base->type == REAL && exponent->type == REAL)
    {
        result->value.real = pow(base->value.real,exponent->value.real);
        return result;
    }
    fprintf(stderr,"^: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(base->type),type_to_string(exponent->type));
    exit(EXIT_FAILURE);
    return NULL;
}

// checked
Lexeme *eval_remainder(Lexeme *tree, Lexeme *env)
{
    Lexeme *dividend = eval(tree->left,env);
    Lexeme *divisor =  eval(tree->right,env);
    Lexeme *result = new_Lexeme(INTEGER);
    if (dividend->type == INTEGER && divisor->type == INTEGER)
    {
        if (divisor->value.integer == 0)
        {
            fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
            exit(EXIT_FAILURE);
        }
        result->value.integer = dividend->value.integer % divisor->value.integer;
        return result;
    }
    fprintf(stderr,"/: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(dividend->type),type_to_string(divisor->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_div(Lexeme *tree, Lexeme *env)
{
    Lexeme *dividend = eval(tree->left,env);
    Lexeme *divisor =  eval(tree->right,env);
    Lexeme *result = new_Lexeme(INTEGER);
    if (dividend->type == INTEGER && divisor->type == INTEGER)
    {
        if (divisor->value.integer > 0)
        {
            result->value.integer = (int) floor(((double) dividend->value.integer)/divisor->value.integer);
            return result;
        }
        if (divisor->value.integer < 0)
        {
            result->value.integer = (int) ceil(((double) dividend->value.integer)/divisor->value.integer);
            return result;
        }
        fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"/: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(dividend->type),type_to_string(divisor->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_mod(Lexeme *tree, Lexeme *env)
{
    Lexeme *dividend = eval(tree->left,env);
    Lexeme *divisor =  eval(tree->right,env);
    Lexeme *result = new_Lexeme(INTEGER);
    if (dividend->type == INTEGER && divisor->type == INTEGER)
    {
        int quotient;
        if (divisor->value.integer > 0)
        {
            quotient = (int) floor(((double) dividend->value.integer)/divisor->value.integer);
            result->value.integer = dividend->value.integer - (divisor->value.integer * quotient);
            return result;
        }
        if (divisor->value.integer < 0)
        {
            quotient = (int) ceil(((double) dividend->value.integer)/divisor->value.integer);
            result->value.integer = dividend->value.integer - (divisor->value.integer * quotient);
            return result;
        }
        fprintf(stderr,"DIVIDE-BY-ZERO ERROR\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"/: invalid operand types: \"%s\" and \"%s\"\n",
            type_to_string(dividend->type),type_to_string(divisor->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_unary_plus(Lexeme *tree, Lexeme *env)
{
    Lexeme *operand = eval(tree->right,env);
    if (operand->type == INTEGER || operand->type == REAL)
    {
        return operand;
    }
    fprintf(stderr,"unary +: invalid operand type: \"%s\"\n",
            type_to_string(operand->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_unary_minus(Lexeme *tree, Lexeme *env)
{
    Lexeme *operand = eval(tree->right,env);
    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (operand->type == INTEGER)
    {
        result->type = INTEGER;
        result->value.integer = -1 * operand->value.integer;
        return result;
    }
    if (operand->type == REAL)
    {
        result->type = REAL;
        result->value.real = -1 * operand->value.real;
        return result;
    }
    fprintf(stderr,"unary -: invalid operand type: \"%s\"\n",
            type_to_string(operand->type));
    exit(EXIT_FAILURE);
}

// checked
Lexeme *eval_abs(Lexeme *tree, Lexeme *env)
{
    Lexeme *operand = eval(tree->right,env);
    Lexeme *result = new_Lexeme(UNINITIALIZED);
    if (operand->type == INTEGER)
    {
        result->type = INTEGER;
        result->value.integer = abs(operand->value.integer);
        return result;
    }
    if (operand->type == REAL)
    {
        result->type = REAL;
        result->value.real = fabs(operand->value.real);
        return result;
    }
    fprintf(stderr,"absolute value: invalid operand type: \"%s\"\n",
            type_to_string(operand->type));
    exit(EXIT_FAILURE);
}
