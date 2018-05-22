#include "lexeme.h"
#include "pretty.h"
#include <stdio.h>
#include <stdlib.h>
#define USUALTAB 7

static int spacecount = 0;
static int top_level_flag = 1;
static int breaklineflag = 1;
static int parenthesized = 0;

void breakline(void);

void pretty_integer(Lexeme *tree);
void pretty_real(Lexeme *tree);
void pretty_true(Lexeme *tree);
void pretty_false(Lexeme *tree);
void pretty_string(Lexeme *tree);
void pretty_string_aux(char *s);
char unescape_char(char ch, int *escaped);
void pretty_nil(Lexeme *tree);
void pretty_this(Lexeme *tree);
void pretty_else(Lexeme *tree);
void pretty_plus(Lexeme *tree);
void pretty_minus(Lexeme *tree);
void pretty_times(Lexeme *tree);
void pretty_divide(Lexeme *tree);
void pretty_pow(Lexeme *tree);
void pretty_remainder(Lexeme *tree);
void pretty_div(Lexeme *tree);
void pretty_mod(Lexeme *tree);
void pretty_unary_plus(Lexeme *tree);
void pretty_unary_minus(Lexeme *tree);
void pretty_abs(Lexeme *tree);
void pretty_variable(Lexeme *tree);
void pretty_array_access(Lexeme *tree);
void pretty_define(Lexeme *tree);
void pretty_assign(Lexeme *tree);
void pretty_func_call(Lexeme *tree);
void pretty_lambda(Lexeme *tree);
void pretty_equals(Lexeme *tree);
void pretty_neq(Lexeme *tree);
void pretty_lt(Lexeme *tree);
void pretty_gt(Lexeme *tree);
void pretty_lt_or_eq(Lexeme *tree);
void pretty_gt_or_eq(Lexeme *tree);
void pretty_not(Lexeme *tree);
void pretty_and(Lexeme *tree);
void pretty_or(Lexeme *tree);
void pretty_if(Lexeme *tree);
void pretty_case(Lexeme *tree);
void pretty_while(Lexeme *tree);
void pretty_list(Lexeme *tree);
void pretty_statements(Lexeme *tree);
void pretty_body(Lexeme *tree);
void pretty_paren(Lexeme *tree);
void pretty_block(Lexeme *tree);
void pretty_return(Lexeme *tree);

void pretty(Lexeme *tree)
{
    switch (tree->type)
    {
        // literals/self-evaluated 
        case INTEGER:        pretty_integer(tree); return;
        case REAL:           pretty_real(tree); return;
        case TRUE:           pretty_true(tree); return;
        case FALSE:          pretty_false(tree); return;
        case STRING:         pretty_string(tree); return;
        case NIL:            pretty_nil(tree); return;

        // pseudo-literals
        case THIS:           pretty_this(tree); return;
        case ELSE:           pretty_else(tree); return;

        // math
        case PLUS:        pretty_plus(tree); return;
        case MINUS:       pretty_minus(tree); return;
        case TIMES:       pretty_times(tree); return;
        case DIVIDED_BY:  pretty_divide(tree); return;
        case EXPONENT:    pretty_pow(tree); return;
        case REMAINDER:   pretty_remainder(tree); return;
        case DIV:         pretty_div(tree); return;
        case MOD:         pretty_mod(tree); return;
        case UNARY_PLUS:      pretty_unary_plus(tree); return;
        case UNARY_MINUS:     pretty_unary_minus(tree); return;
        case ABSOLUTE_VALUE:  pretty_abs(tree); return;

        // variables
        case VARIABLE:      pretty_variable(tree); return;
        case ARRAY_ACCESS:  pretty_array_access(tree); return;

        // definitions and assignment
        case DEFINE:  pretty_define(tree); return;
        case ASSIGN:  pretty_assign(tree); return;

        // function call
        case FUNCTION_CALL:  pretty_func_call(tree); return;

        // lambda
        case LAMBDA:  pretty_lambda(tree); return;

        // relational
        case EQUALS:                  pretty_equals(tree); return;
        case NOT_EQUALS:              pretty_neq(tree); return;
        case LESS_THAN:               pretty_lt(tree); return;
        case GREATER_THAN:            pretty_gt(tree); return;
        case LESS_THAN_OR_EQUALS:     pretty_lt_or_eq(tree); return;
        case GREATER_THAN_OR_EQUALS:  pretty_gt_or_eq(tree); return;

        // logical
        case NOT:  pretty_not(tree); return;
        case AND:  pretty_and(tree); return;
        case OR:   pretty_or(tree); return;

        // conditional
        case IF:  pretty_if(tree); return;

        // loop
        case WHILE:  pretty_while(tree); return;

        // compound
        case LINK:
        {
            if (top_level_flag)
            {
                top_level_flag = 0;
                pretty_statements(tree);
            }
            else
                pretty_list(tree);
            return; 
        }
        case BODY:          pretty_body(tree); return; 
        case PARENTHESIS:   pretty_paren(tree); return;
        case CURLY_BRACES:  pretty_block(tree); return;
        case RETURN:        pretty_return(tree); return;

        default: break;
    }

    fprintf(stderr,"pretty(): INVALID LEXEME : %s.\n",type_to_string(tree->type));
    exit(EXIT_FAILURE);
}

void breakline(void)
{
    if (breaklineflag)
    {
        int i;
        printf("\n");
        for (i = 0; i < spacecount; ++i)
            printf(" ");
    }
    else
        printf(" ");
}

void pretty_integer(Lexeme *tree)
{
    printf("%d",tree->value.integer);
}

void pretty_real(Lexeme *tree)
{
    printf("%f",tree->value.real);
}

void pretty_true(Lexeme *tree)
{
    printf("true");
}

void pretty_false(Lexeme *tree)
{
    printf("false");
}

void pretty_string(Lexeme *tree)
{
    printf("\"");
    pretty_string_aux(tree->value.string);
    printf("\"");
}

void pretty_string_aux(char *s)
{
    int i = 0;
    int escaped = 0;
    int ch = unescape_char(s[i++], &escaped);
    while (ch != '\0')
    {
        if (escaped)
            printf("\\%c",ch);
        else
            printf("%c",ch);
        ch = unescape_char(s[i++], &escaped);
    }
}

char unescape_char(char ch, int *escaped)
{
    *escaped = 1;
    switch (ch)
    {
        case '\n': return 'n';
        case '\t': return 't';
        case '\b': return 'b';
        case '\v': return 'v';
        case '\f': return 'f';
        case '\r': return 'r';
        case '\a': return 'a';
        case '\"': return '\"';
        case '\'': return '\'';
        case '\\': return '\\';
        case '\?': return '\?';
        default:
            *escaped = 0;
            return ch;
    }
}

void pretty_nil(Lexeme *tree)
{
    printf("nil");
}

void pretty_this(Lexeme *tree)
{
    printf("this");
}

void pretty_else(Lexeme *tree)
{
    printf("else");
}

void pretty_plus(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" + ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_minus(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" - ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_times(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" * ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_divide(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" / ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_pow(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" ^ ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_remainder(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" %% ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_div(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" div ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_mod(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    if (parenthesized != 0) printf("(");
    pretty(tree->left);
    printf(" mod ");
    pretty(tree->right);
    if (parenthesized != 0) printf(")");
    
    breaklineflag = temp;
}

void pretty_unary_plus(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("+");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_unary_minus(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("-");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_abs(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("|");
    pretty(tree->right);
    printf("|");
    
    breaklineflag = temp;
}

void pretty_variable(Lexeme *tree)
{
    printf("%s",tree->value.string);
}

void pretty_array_access(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf("[");
    pretty(tree->right);
    printf("]");
    
    breaklineflag = temp;
}

void pretty_define(Lexeme *tree)
{
    // only indent for function definitions
    
    // if true, then function definition
    if (tree->right->type == LAMBDA)
    {
        pretty(tree->left);
        printf("(");
        if (tree->right->left->type != NIL)
            pretty(tree->right->left);
        printf(")");

        printf(" :=");

        // indent zone
        spacecount += USUALTAB;
        breakline();

        pretty(tree->right->right);

        breakline();
        // end indent zone
        spacecount -= USUALTAB;
        printf(":;");
    }
    else // variable definition
    {
        if (tree->right->type == UNINITIALIZED)
        {
            pretty(tree->left);
            printf(";");
        }
        else
        {
            int temp = breaklineflag;
            breaklineflag = 0;

            pretty(tree->left);
            printf(" := ");

            pretty(tree->right);
            printf(" :;");

            breaklineflag = temp;
        }
    }
}

void pretty_assign(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" == ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_func_call(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf("(");
    if (tree->right->type != NIL)
        pretty(tree->right);
    printf(")");

    breaklineflag = temp;
}

void pretty_lambda(Lexeme *tree)
{
    // disable linebreaking
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("lambda[ ");
    pretty(tree->left);
    printf(" : ");
    pretty(tree->right);
    printf(" ]");

    breaklineflag = temp;
}

void pretty_equals(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" = ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_neq(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" != ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_lt(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" < ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_gt(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" > ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_lt_or_eq(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" <= ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_gt_or_eq(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" >= ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_not(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("not ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_and(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" and ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_or(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    pretty(tree->left);
    printf(" or ");
    pretty(tree->right);
    
    breaklineflag = temp;
}

void pretty_if(Lexeme *tree)
{
    printf("if[ ");
    // get cases
    tree = tree->right;

    // indent zone
    spacecount += 2;

    while (tree->type != NIL)
    {
        pretty_case(tree->left);
        if (tree->right->type != NIL)
        {
            breakline();
            printf(": ");
        }
        tree = tree->right;
    }
    breakline();
    printf("]");

    // end indent zone
    spacecount -= 2;
}

void pretty_case(Lexeme *tree)
{
    // condition
    pretty(tree->left);
    if (tree->left->type != ELSE)
        printf(" ?");
    // body
    // indent zone
    spacecount += 2;
    breakline();
    pretty(tree->right);
    // end indent zone
    spacecount -= 2;
}

void pretty_while(Lexeme *tree)
{
    Lexeme *cond = tree->left;
    Lexeme *split = tree->right;
    Lexeme *precond = split->left;
    Lexeme *postcond = split->right;

    printf("while[ ");

    // indent zone
    spacecount += 7;
    pretty(precond);

    if (precond->right->type != NIL)
        breakline();

    // disable linebreaking
    int temp = breaklineflag;
    breaklineflag = 0;
    printf(": ");
    pretty(cond);
    printf(" :");
    breaklineflag = temp;

    breakline();
    pretty(postcond);

    // end indent zone
    spacecount -= 7;

    // indent zone
    spacecount += 5;
    breakline();
    printf("]");
    spacecount -= 5;
    // end indent zone
}

void pretty_list(Lexeme *tree) 
{
    while (tree->type != NIL)
    {
        pretty(tree->left);
        if (tree->left->type != DEFINE && tree->right->type != NIL)
            printf(", ");
        else if (tree->left->type == DEFINE)
            printf(" ");
        tree = tree->right;
    }
}

void pretty_statements(Lexeme *tree) 
{
    while (tree->type != NIL)
    {
        pretty(tree->left);
        if (tree->left->type != DEFINE)
            printf(",");
        printf("\n");
        if (tree->right->type != NIL)
            breakline(); // flag should be true at this level
        tree = tree->right;
    }
}

void pretty_body(Lexeme *tree) 
{
    tree = tree->right;
    while (tree->type != NIL)
    {
        pretty(tree->left);
        if (tree->left->type != DEFINE && tree->right->type != NIL)
        {
            printf(",");
            breakline();
        }
        else if (tree->left->type == DEFINE)
            breakline();
        tree = tree->right;
    }
}

void pretty_paren(Lexeme *tree)
{
    int temp = breaklineflag;
    breaklineflag = 0;

    printf("(");
    pretty(tree->right);
    printf(")");
    
    breaklineflag = temp;
}

void pretty_block(Lexeme *tree)
{
    printf("{");
    breakline();
    pretty(tree->right);
    breakline();
    printf("}");
}

void pretty_return(Lexeme *tree)
{
    printf("return");
    if (tree->right->type != UNINITIALIZED)
    {
        printf(" ");
        pretty(tree->right);
    }
}
