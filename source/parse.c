#include "parse.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check(Type type);
static Lexeme *advance(void);
static Lexeme *match(Type type, char *msg);
static void match_without_advance(Type type, char *msg);

static Lexeme *body(void);
static Lexeme *statements(void);

static Lexeme *variable_declaration(Lexeme *var);
static Lexeme *variable_definition(Lexeme *var);
static Lexeme *function_definition(Lexeme *var, Lexeme *list);

static Lexeme *statement_or_expression(int *is_expression);
static Lexeme *variable_or_expression_list(int *nonvariable, Type delimiter);

static Lexeme *expression(void);
static Lexeme *expression_finisher(Lexeme *u);

static Lexeme *expression_list(Type delimiter);
static Lexeme *variable_list(Type delimiter);

static Lexeme *invert_binary_operator(void);
static Lexeme *binary_operator(void);
static int binary_operator_pending(void);
static int precedence(Lexeme *op);
static int is_left_associative(Lexeme *op);

static int literal_pending(void);
static int unary_operator_pending(void);
static Lexeme *unary_operator(void);
static Lexeme *parenthesized_expression(void);
static Lexeme *variable(void);
static Lexeme *absolute_value(void);
static Lexeme *block_expression(void);
static Lexeme *if_expression(void);
static Lexeme *if_cases(void);
static Lexeme *if_case(void);
static Lexeme *else_case(void);
static Lexeme *while_expression(void);
static Lexeme *lambda_expression(void);
static Lexeme *unary(void);

static Lexeme *delay(void);
static Lexeme *force(void);
static Lexeme *scons(void);
static Lexeme *scar(void);
static Lexeme *scdr(void);
static char *make_copy(const char *src);

static Lexeme *return_expression(void);

static Lexeme *postfix_checker(Lexeme *unary_head);
static Lexeme *function_call(Lexeme *unary_head);
static Lexeme *array_access(Lexeme *unary_head);

static FILE *parse_fp;
static Lexeme *current_lexeme;

/*
    Note:
        - structure is such that earlier lexemes appear left of later lexemes
            - except for expressions, where precedence rules restructure the parse tree
        - All lists or sequences of lexemes are terminated by a NIL lexeme.
        - All list or sequence functions handle empty lists.
*/

Lexeme *parse_program(char *filename)
{
    parse_fp = lex_fopen(filename);
    if (parse_fp == NULL)
    {
        perror("Failed: ");
        exit(EXIT_FAILURE);
    }
    advance(); // get started
    Lexeme *parse_tree = statements();
    return parse_tree;
}

static int check(Type type)
{
    return current_lexeme->type == type;
}

static Lexeme *advance(void)
{
    Lexeme *temporary = current_lexeme;
    current_lexeme    = lex(parse_fp);
    return temporary;
}

static Lexeme *match(Type type, char *msg)
{
    match_without_advance(type,msg);
    return advance();
}

static void match_without_advance(Type type, char *msg)
{
    if(!check(type))
    {
        fprintf(stderr, "Line %d: %s does not match expected %s\n",
                current_lexeme->line_number,
                lexeme_info(current_lexeme),type_to_string(type));
        fprintf(stderr,"%s",msg);
        exit(EXIT_FAILURE);
    }
}


/*
    - a body differs from a list in that when evaluated, it returns the value of the last expression
    - a body is denoted by a head lexeme labeled BODY
*/
static Lexeme *body(void)
{
    Lexeme *body = new_Lexeme(BODY);
    body->right  = new_Lexeme(LINK);

    Lexeme *current = body->right;
    int is_expression = 0;
    current->left   = statement_or_expression(&is_expression);

    // go until read an expression that is not followed by a comma
    // (a comma signifies that an expression is a statement)
    while (is_expression == 0)
    {
        current->right = new_Lexeme(LINK);
        current = current->right;
        current->left  = statement_or_expression(&is_expression);
    }
    current->right = nil_Lexeme();

    // note: last item will be an expression, as intended
    return body;
}

static Lexeme *statements(void)
{
    if (check(END_OF_INPUT)) return nil_Lexeme();

    Lexeme *root = new_Lexeme(LINK);
    int is_expression = 0;
    root->left   = statement_or_expression(&is_expression);

    Lexeme *current = root; // used to traverse down list of statements
    
    while (!check(END_OF_INPUT) && (is_expression == 0))
    {
        current->right = new_Lexeme(LINK);
        current = current->right;
        current->left  = statement_or_expression(&is_expression);
    }
    if (is_expression == 1)
    {
        Lexeme *mark = current;
        while (mark->left != NULL) mark = mark->left;
        fprintf(stderr,"Statement at line %d missing a terminating comma.\n",
                mark->line_number);
        exit(EXIT_FAILURE);
    }
    current->right = nil_Lexeme();

    return root;
}

static Lexeme *variable_declaration(Lexeme *var)
{
    advance(); // discard semicolon
    Lexeme *d = new_Lexeme(DEFINE); // turn into a := lexeme with uninitialized 
    d->line_number = var->line_number;
    d->left  = var;
    d->right = new_Lexeme(UNINITIALIZED);
    return d;
}

static Lexeme *variable_definition(Lexeme *var)
{
    Lexeme *d = advance(); // use := lexeme
    d->left   = var;
    d->right  = body();
    match(END_DEFINE, "Variable declaration must be terminated by colon-semicolon :;\n");
    return d;
}

static Lexeme *function_definition(Lexeme *var, Lexeme *list)
{
    Lexeme *d = advance(); // use := lexeme
    d->left   = var;
    d->line_number = var->line_number;
    Lexeme *lambda = d->right = new_Lexeme(LAMBDA);
    lambda->left  = list; // set parameters
    lambda->right = body(); // set body
    match(END_DEFINE, "Function definition must be terminated by colon-semicolon :;\n");
    return d;
}

/*
    - an expression is a statement only if it is followed by a comma
    - long and complicated because statements and expressions are indistinguishable
        up until a certain point
*/
static Lexeme *statement_or_expression(int *is_expression)
{
    *is_expression = 1;

    if (check(VARIABLE)) // declarations must begin with a variable
    {
        Lexeme *var = advance(); // get variable
        if (check(SEMICOLON)) // variable declaration with no initialization
        {
            *is_expression = 0;
            return variable_declaration(var);
        }
        if (check(DEFINE)) // variable definition
        {
            *is_expression = 0;
            return variable_definition(var);
        }
        if (check(OPEN_PAREN))
        {
            advance(); // discard open parenthesis

            int nonvariable; // 1 if there is a nonvariable expression in the list
            Lexeme *list = variable_or_expression_list(&nonvariable,CLOSE_PAREN);

            if (check(DEFINE)) // must be declaration
            {
                if (nonvariable != 0)
                {
                    fprintf(stderr,"Line %d: To declare %s a function, parameters must be variables.\n",
                            var->line_number, lexeme_info(var));
                    exit(EXIT_FAILURE);
                }
                *is_expression = 0;
                return function_definition(var,list);
            }
            else // must be a function call even if next lexeme is a semicolon, because it may be a tail call
            {
                Lexeme *f = new_Lexeme(FUNCTION_CALL);
                f->line_number = var->line_number;
                f->left  = var; // set function name
                f->right = list; // set arguments
                // this is a postfix operable unary, now we must read the rest of the expression
                Lexeme *e = expression_finisher(f);

                if (check(COMMA))
                {
                    advance();
                    *is_expression = 0;
                }
                return e;
            }
        }
        else // not declaration, variable must be part of an expression
        {
            Lexeme *e = expression_finisher(var);
            if (check(COMMA))
            {
                advance();
                *is_expression = 0;
            }
            return e;
        }
    }
    else // not declaration
    {
        Lexeme *e = expression();
        if (check(COMMA))
        {
            advance();
            *is_expression = 0;
        }
        return e;
    }
}

/*
    - can be an empty list
    - consumes delimiter
*/
static Lexeme *variable_or_expression_list(int *nonvariable, Type delimiter)
{
    *nonvariable = 0; // default
    if (check(delimiter))
    {
        advance(); // discard
        return nil_Lexeme(); // empty list
    }
    // at this point, guaranteed at least one expression
    Lexeme *root = new_Lexeme(LINK);
    root->left   = expression();
    // used to tell if list contains any expressions
    if (root->left->type != VARIABLE) *nonvariable = 1;
    Lexeme *current = root; // used to traverse down list
    while (check(COMMA)) // list continues
    {
        advance(); // discard comma
        // create new list node
        current->right = new_Lexeme(LINK);
        // advance current pointer
        current = current->right;
        // set left child to hold expression
        current->left = expression();
        // set flag if any item is an expression
        if (current->left->type != VARIABLE) *nonvariable = 1;
    }
    match(delimiter,"Lists must be terminated by a delimiter.\n");  // must be close parenthesis
    current->right = nil_Lexeme(); // terminate list
    return root;
}

static Lexeme *expression(void)
{
    Lexeme *u = unary();

    if (binary_operator_pending())
    {
        Lexeme *op, *op1, *op2;

        Stack *operands  = new_Stack();
        Stack *operators = new_Stack();

        push_Stack(operands,u);

        while (binary_operator_pending()) 
        {
            if (is_empty_Stack(operators))
            {
                op = binary_operator();
                push_Stack(operators,op);
                
                u = unary();
                push_Stack(operands,u);
            }
            else 
            {
                // a op1 b op2 c
                op1 = peek_Stack(operators);
                op2 = binary_operator();

                // while op1 higher precedence than op2 and stack is non-empty
                while (    (precedence(op1) > precedence(op2))
                        || (    (precedence(op1) == precedence(op2))
                             && is_left_associative(op1)           ))
                {
                    op = pop_Stack(operators);
                    op->right = pop_Stack(operands);
                    op->left  = pop_Stack(operands);
                    push_Stack(operands,op);

                    if (is_empty_Stack(operators))
                        break;
                    else
                        op1 = peek_Stack(operators);
                }
                // op2 is finally a higher precedence or stack is empty

                push_Stack(operators,op2);
                
                // get next operand 
                u = unary();
                push_Stack(operands,u);
            }
        }
        // reduce stack to single lexeme and return that single lexeme after freeing stacks
        while (size_Stack(operands) != 1)
        {
            op = pop_Stack(operators);
            op->right = pop_Stack(operands);
            op->left  = pop_Stack(operands);
            push_Stack(operands,op);
        }
        u = pop_Stack(operands);
        free_Stack(operands);
        free_Stack(operators);
    }
    return u;
}

static Lexeme *return_expression(void)
{
    Lexeme *r = advance(); // get RETURN lexeme
    // check if end
    if (check(COMMA) || check(END_DEFINE))
        r->right = new_Lexeme(UNINITIALIZED);
    else
        r->right = expression(); // set return expression
    r->left = false_Lexeme(); // flag that says whether it has been evaluated
    return r;
}

// complete expression after reading a postfix operable unary 
static Lexeme *expression_finisher(Lexeme *v)
{
    Lexeme *u = postfix_checker(v);

    if (binary_operator_pending())
    {
        Lexeme *op, *op1, *op2;

        Stack *operands  = new_Stack();
        Stack *operators = new_Stack();

        push_Stack(operands,u);

        while (binary_operator_pending()) 
        {
            if (is_empty_Stack(operators))
            {
                op = binary_operator();
                push_Stack(operators,op);
                
                u = unary();
                push_Stack(operands,u);
            }
            else
            {
                // a op1 b op2 c
                op1 = peek_Stack(operators);
                op2 = binary_operator();

                // while op1 higher precedence than op2 and stack is non-empty
                while (    (precedence(op1) > precedence(op2))
                        || (    (precedence(op1) == precedence(op2))
                             && is_left_associative(op1)           ))
                {
                    op = pop_Stack(operators);
                    op->right = pop_Stack(operands);
                    op->left = pop_Stack(operands);
                    push_Stack(operands,op);

                    if (is_empty_Stack(operators))
                        break;
                    else
                        op1 = peek_Stack(operators);
                }
                // op2 is finally a higher precedence or stack is empty

                push_Stack(operators,op2);
                
                // get next operand 
                u = unary();
                push_Stack(operands,u);
            }
        }
        // reduce stack to single lexeme and return that single lexeme after freeing stacks
        while (size_Stack(operands) != 1)
        {
            op = pop_Stack(operators);
            op->right = pop_Stack(operands);
            op->left  = pop_Stack(operands);
            push_Stack(operands,op);
        }
        u = pop_Stack(operands);
        free_Stack(operands);
        free_Stack(operators);
    }
    return u;
}

static Lexeme *expression_list(Type delimiter)
{
    int x; // this value doesn't matter, since expressions are a superset of variables
    return variable_or_expression_list(&x,delimiter);
}

static Lexeme *variable_list(Type delimiter)
{
    int nonvariable; 
    Lexeme *list = variable_or_expression_list(&nonvariable,delimiter);
    if (nonvariable == 1)
    {
        // note: list->left is not null since nonvariable was detected
        fprintf(stderr,"List of variables beginning at line %d: Non-variable item in parameter list.\n",
                list->left->line_number);
        exit(EXIT_FAILURE);
    }
    return list;
}

static Lexeme *invert_binary_operator(void)
{
    advance(); // consume NOT lexeme
    Lexeme *l;
    if (check(EQUALS)) l = new_Lexeme(NOT_EQUALS);
    else if (check(LESS_THAN)) l = new_Lexeme(GREATER_THAN_OR_EQUALS);
    else if (check(GREATER_THAN)) l = new_Lexeme(LESS_THAN_OR_EQUALS);
    else if (check(LESS_THAN_OR_EQUALS)) l = new_Lexeme(GREATER_THAN);
    else if (check(GREATER_THAN_OR_EQUALS)) l = new_Lexeme(LESS_THAN);
    else
    {
        fprintf(stderr,"Line %d: NOT does not apply to %s\n",
            current_lexeme->line_number,
            lexeme_info(current_lexeme));
        exit(EXIT_FAILURE);
    }
    l->line_number = current_lexeme->line_number;
    advance(); // discard original operator
    return l;
}

static Lexeme *binary_operator(void)
{
    if (check(NOT)) // invert comparison operator
        return invert_binary_operator();
    return advance(); // operator already packaged into lexeme
}

/*
    Used when determining if expression has terminated.
    - NOT will be expected as part of a binary operator
        because it can be an operator inverter
*/
static int binary_operator_pending(void)
{
    switch (current_lexeme->type)
    {
        case ASSIGN :
        case PLUS :
        case TIMES :
        case MINUS :
        case DIVIDED_BY :
        case REMAINDER :
        case EXPONENT :
        case DIV :
        case MOD :
        case AND :
        case OR :
        case EQUALS :
        case NOT_EQUALS :
        case LESS_THAN :
        case GREATER_THAN :
        case LESS_THAN_OR_EQUALS :
        case GREATER_THAN_OR_EQUALS :
        case NOT :
            return 1;
        default : return 0;
    }
}

static int precedence(Lexeme *op)
{
    switch (op->type)
    {
        case EXPONENT :
            return 8;
        case TIMES :
        case DIVIDED_BY :
        case REMAINDER :
            return 7;
        case MOD :
        case DIV :
            return 6;
        case PLUS :
        case MINUS :
            return 5;
        case LESS_THAN :
        case GREATER_THAN :
        case LESS_THAN_OR_EQUALS :
        case GREATER_THAN_OR_EQUALS :
            return 4;
        case EQUALS :
        case NOT_EQUALS :
            return 3;
        case AND :
            return 2;
        case OR :
            return 1;
        case ASSIGN :
            return 0;
        default :
            fprintf(stderr,"requested precedence of non binary operator.\n");
            exit(EXIT_FAILURE);
    }
}

static int is_left_associative(Lexeme *op)
{
    switch (op->type)
    {
        case EXPONENT :
        case ASSIGN :
            return 0;
        case TIMES :
        case DIVIDED_BY :
        case REMAINDER :
        case MOD :
        case DIV :
        case PLUS :
        case MINUS :
        case LESS_THAN :
        case GREATER_THAN :
        case LESS_THAN_OR_EQUALS :
        case GREATER_THAN_OR_EQUALS :
        case EQUALS :
        case NOT_EQUALS :
        case AND :
        case OR :
            return 1;
        default :
            fprintf(stderr,"requested associativity of non binary operator.\n");
            exit(EXIT_FAILURE);
    }
}

static int literal_pending(void)
{
    return (check(INTEGER) || check(REAL) || check(STRING) ||
            check(TRUE) || check(FALSE) || check(THIS) || check(NIL));
}

static int unary_operator_pending(void)
{
    return (check(PLUS) || check(MINUS) || check(NOT));
}

static Lexeme *unary_operator(void)
{
    if (check(PLUS)) // must be unary plus
    {
        int line_number = current_lexeme->line_number;
        advance(); // discard and replace with unary operator
        Lexeme *l = new_Lexeme(UNARY_PLUS);
        l->line_number = line_number; // inherit line_number
        l->right = unary();
        return l;
    }
    if (check(MINUS)) // must be unary minus
    {
        int line_number = current_lexeme->line_number;
        advance(); // discard and replace with unary operator
        Lexeme *l = new_Lexeme(UNARY_MINUS);
        l->line_number = line_number; // inherit line_number
        l->right = unary();
        return l;
    }
    //if (check(NOT)) // not is already unary
    else
    {
        Lexeme *l = advance();
        l->right  = unary();
        return l;
    }
}

static Lexeme *parenthesized_expression(void)
{
    // must preserve braces/parenthesis for precedence purposes
    Lexeme *p = new_Lexeme(PARENTHESIS);
    p->line_number = current_lexeme->line_number; //inherit line number before discard
    advance();
    p->right = expression();
    match(CLOSE_PAREN,"Parenthetical expression: '(' requires a matching ')'.\n");
    return postfix_checker(p);
}

static Lexeme *variable(void)
{
    Lexeme *var = advance();
    return postfix_checker(var);
}

static Lexeme *absolute_value(void)
{
    Lexeme *a = new_Lexeme(ABSOLUTE_VALUE);
    a->line_number = current_lexeme->line_number;
    advance();
    a->right = expression();
    match(VERTICAL_BAR,"Absolute value: '|' requires a matching '|'.\n");
    return a;
}

static Lexeme *block_expression(void)
{
    Lexeme *b = new_Lexeme(CURLY_BRACES);
    b->line_number = current_lexeme->line_number;
    advance();
    b->right = body();
    match(CLOSE_BRACE,"Block: '{' requires a matching '}'.\n");
    return postfix_checker(b);
}

static Lexeme *if_expression(void)
{
    Lexeme *f = advance(); // get IF lexeme
    // if lexeme only has a right child, which is a list of cases
    f->right = if_cases();
    return postfix_checker(f);
}

static Lexeme *if_cases(void)
{
    match(OPEN_BRACKET,"If expression: 'if' must be followed by an '['.\n");
    Lexeme *root = new_Lexeme(LINK);
    root->left = if_case();
    Lexeme *current = root;
    int found_else_case = 0;
    while (check(COLON) && (found_else_case == 0))
    {
        advance(); // discard colon
        current->right = new_Lexeme(LINK);
        current = current->right;
        if (check(ELSE))
        {
            current->left = else_case();
            found_else_case = 1;
        }
        else
            current->left = if_case();
    }
    match(CLOSE_BRACKET,"'[' requires a matching ']'.\n");
    current->right = nil_Lexeme();
    return root;
}

static Lexeme *if_case(void)
{
    Lexeme *c = new_Lexeme(CASE);
    c->left  = body();
    match(QUESTION_MARK,"If expression case: 'if' case conditional must be followed by a '?'.\n");
    c->right = body();
    return c;
}

static Lexeme *else_case(void)
{
    Lexeme *c = new_Lexeme(CASE);
    c->left   = advance(); // must be else, no check required
    c->right  = body();
    return c;
}

static Lexeme *while_expression(void)
{
    // left child is test
    // right child is a parent node whose
    // left child is the body before the conditional
    // right child is the body after the conditional
    Lexeme *w = advance(); // get WHILE lexeme
    match(OPEN_BRACKET,"While expression: 'while' must be followed by an '['.\n");

    w->right = new_Lexeme(WHILE_SPLIT);

    // use expression_list() instead of body() to outlaw define
    // need define to not affect list
    w->right->left = new_Lexeme(BODY);
    w->right->left->right = expression_list(COLON);

    w->left = expression(); // while conditional 
    match(COLON,"'while' conditional must be encapsulated within ':'.\n");
    w->right->right = new_Lexeme(BODY);
    w->right->right->right = expression_list(CLOSE_BRACKET);

    return postfix_checker(w);
}

static Lexeme *lambda_expression(void)
{
    Lexeme *l = advance();
    match(OPEN_BRACKET,"'lambda' must be followed by an '['.\n");
    l->left  = variable_list(COLON);
    l->right = body();
    match(CLOSE_BRACKET,"Lambda expression: '[' requires a matching ']'.\n");
    return postfix_checker(l);
}

static Lexeme *unary(void)
{
    if (literal_pending())
    {
        Lexeme *l = advance(); // lexeme already packaged
        if (check(OPEN_PAREN) || check(OPEN_BRACKET))
        {
            fprintf(stderr,"type %s is not postfix operable!\n",
                    type_to_string(l->type));
            exit(EXIT_FAILURE);
        }
        return l;
    }
    if (unary_operator_pending())
        return unary_operator();
    if (check(VARIABLE)) // variable, function call, or array access
        return variable();
    if (check(OPEN_PAREN))
        return parenthesized_expression();
    if (check(VERTICAL_BAR))
        return absolute_value();
    if (check(OPEN_BRACE))
        return block_expression();
    if (check(IF))
        return if_expression();
    if (check(WHILE))
        return while_expression();
    if (check(LAMBDA))
        return lambda_expression();
    if (check(DELAY))
        return delay();
    if (check(FORCE))
        return force();
    if (check(SCONS))
        return scons();
    if (check(SCAR))
        return scar();
    if (check(SCDR))
        return scdr();
    if (check(RETURN))
        return return_expression();
    fprintf(stderr,"Line %d: %s is not a unary.\n",
        current_lexeme->line_number,
        lexeme_info(current_lexeme));
    exit(EXIT_FAILURE);
}

static Lexeme *postfix_checker(Lexeme *unary_head)
{
    if (check(OPEN_PAREN)) // function call
        return function_call(unary_head);
    else if (check(OPEN_BRACKET)) // array acess
        return array_access(unary_head);
    else // no postfix_operator
        return unary_head;
}

static Lexeme *function_call(Lexeme *unary_head)
{
    // left child is the thing that should evaluate to a closure
    // right child is the argument list
    Lexeme *f = new_Lexeme(FUNCTION_CALL);
    f->line_number = current_lexeme->line_number;
    advance(); // discard open parenthesis
    f->left  = unary_head;
    f->right = expression_list(CLOSE_PAREN); // arguments
    return postfix_checker(f);
}

static Lexeme *array_access(Lexeme *unary_head)
{
    // left child of array_access is the variable name to access
    // right child of array_access is the body which computes the index to access
    Lexeme *a = new_Lexeme(ARRAY_ACCESS);
    a->line_number = current_lexeme->line_number;
    advance(); // discard open bracket
    a->left  = unary_head;
    a->right = body();
    match(CLOSE_BRACKET,"Array access: '[' requires a matching ']'.\n");
    return postfix_checker(a);
}

static Lexeme *delay(void)
{
    // delay[body] = lambda[:body]
    Lexeme *l = new_Lexeme(LAMBDA); 
    l->line_number = current_lexeme->line_number;
    match(DELAY,"Discarding DELAY Lexeme.\n");
    match(OPEN_BRACKET,"'delay' must be followed by an '['.\n");
    l->left  = nil_Lexeme(); // no argument lexeme
    l->right = body();
    match(CLOSE_BRACKET,"delay: '[' requires a matching ']'.\n");
    return postfix_checker(l);
}

static Lexeme *force(void)
{
    // force[body] = {body}()

    Lexeme *b = new_Lexeme(CURLY_BRACES);
    b->line_number = current_lexeme->line_number;
    match(FORCE,"Discarding FORCE Lexeme.\n");
    match(OPEN_BRACKET,"'force' must be followed by an '['.\n");
    b->right = body();
    match(CLOSE_BRACKET,"force: '[' requires a matching ']'.\n");

    // {body} -> {body}()

    Lexeme *f = new_Lexeme(FUNCTION_CALL);
    f->line_number = b->line_number;
    f->left  = b;
    f->right = nil_Lexeme(); // NO ARGUMENTS 
    return postfix_checker(f);
}

static char *make_copy(const char *src)
{
    char *dest = malloc((1 + strlen(src)) *sizeof(char));
    strcpy(dest, src);
    return dest;
}

static Lexeme *scons(void)
{
    // scons[expr1,expr2] = cons(expr1,lambda[:body])

    // make "scons"
    Lexeme *var = new_Lexeme(VARIABLE);
    var->line_number = current_lexeme->line_number;
    match(SCONS,"Discarding SCONS Lexeme.\n");
    var->value.string = make_copy("cons");
    
    // convert to function call car(args)
    Lexeme *f = new_Lexeme(FUNCTION_CALL);
    f->line_number = var->line_number;
    advance(); // discard open bracket (instead of parenthesis)
    f->left  = var;
    f->right = expression_list(CLOSE_BRACKET); // arguments

    check_argcount(2, "scons", f->right);
    /*
                     f
            var             link
                       arg1       link
                               arg2   nil
    */

    Lexeme *l = new_Lexeme(LAMBDA); 
    l->line_number = f->line_number;
    l->left  = nil_Lexeme(); // no parameter lexeme
    l->right = f->right->right->left; // expression arg2 becomes the body

    // set arg2 to this lambda
    f->right->right->left = l;

    return postfix_checker(f);
}

static Lexeme *scar(void)
{
    // scar[argument] = car(argument)

    // make "car"
    Lexeme *var = new_Lexeme(VARIABLE);
    var->line_number = current_lexeme->line_number;
    match(SCAR,"Discarding SCAR Lexeme.\n");
    var->value.string = make_copy("car");
    
    // convert to function call car(args)
    Lexeme *f = new_Lexeme(FUNCTION_CALL);
    f->line_number = current_lexeme->line_number;
    advance(); // discard open bracket (instead of parenthesis)
    f->left  = var;
    f->right = expression_list(CLOSE_BRACKET); // arguments
    return postfix_checker(f);
}

static Lexeme *scdr(void)
{
    // scdr[argument] = cdr(argument)()

    // make "cdr"
    Lexeme *var = new_Lexeme(VARIABLE);
    var->line_number = current_lexeme->line_number;
    match(SCDR,"Discarding SCDR Lexeme.\n");
    var->value.string = make_copy("cdr");
    
    // convert to function call cdr(args)
    Lexeme *f = new_Lexeme(FUNCTION_CALL);
    f->line_number = current_lexeme->line_number;
    advance(); // discard open bracket (instead of parenthesis)
    f->left  = var;
    f->right = expression_list(CLOSE_BRACKET); // arguments

    // cdr(args) -> cdr(args)()
    Lexeme *g = new_Lexeme(FUNCTION_CALL);
    g->line_number = f->line_number;
    g->left  = f;
    g->right = nil_Lexeme(); // NO ARGUMENTS 
    return postfix_checker(g);
}


void print_tree(Lexeme *tree,int d)
{
    if (tree == NULL) return;
    print_tree(tree->left,d+1);
    printf("depth %d: type %s\n",d,lexeme_info(tree));
    print_tree(tree->right,d+1);
}

void check_argcount(int argcount, char *func, Lexeme *arglist)
{
    int i = 0;
    while (i < argcount)
    {
        if (arglist->type == NIL)
        {
            fprintf(stderr,"%s: not enough arguments!\n", func);
            exit(EXIT_FAILURE);
        }
        arglist = arglist->right;
        ++i;
    }

    if (arglist->type != NIL)
    {
        fprintf(stderr,"%s: too many arguments!\n", func);
        exit(EXIT_FAILURE);
    }
}
