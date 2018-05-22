#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>

typedef enum type
{
    // just-type Lexemes (20)
    TRUE, FALSE, 
    NIL,

    THIS,
    VERTICAL_BAR,
    OPEN_PAREN, CLOSE_PAREN,
    OPEN_BRACKET, CLOSE_BRACKET,
    OPEN_BRACE, CLOSE_BRACE,
    COMMA, SEMICOLON, COLON, END_DEFINE,
    QUESTION_MARK, ELSE,
    UNKNOWN,
    END_OF_INPUT,

    // stream Lexemes
    DELAY, // replaced: delay[body] = lambda[:body]
    FORCE, // replaced: force[body] = {body}()
    // the following use exprs instead of bodies because
    // scons accepts expression arguments, and it is desirable
    // that scar and scdr be structurally consistent with scons
    SCONS, // replaced: scons[expr1,expr2] = cons(expr1,lambda[:body])
    SCAR,  // replaced: scar[expr] = car(expr)
    SCDR,  // replaced: scdr[expr] = cdr(expr)()

    // lexer terminals
    DEFINE, ASSIGN,
    VARIABLE,
    INTEGER, REAL, STRING, 
    PLUS, MINUS, TIMES, DIVIDED_BY, EXPONENT, REMAINDER,
    DIV, MOD,
    EQUALS, NOT_EQUALS,
    LESS_THAN, GREATER_THAN, LESS_THAN_OR_EQUALS, GREATER_THAN_OR_EQUALS,
    NOT, AND, OR,
    IF, WHILE,

    // parser Lexemes
    LINK,
    FUNCTION_CALL,
    UNARY_PLUS, UNARY_MINUS,
    ARRAY_ACCESS,
    PARENTHESIS,
    ABSOLUTE_VALUE,
    CURLY_BRACES,
    CASE,
    WHILE_SPLIT,
    LAMBDA, 
    BODY,

    // evaluator Lexemes
    CLOSURE,
    ARRAY,
    ENV,
    TABLE,
    BUILTIN,
    UNINITIALIZED,

    RETURN

} Type;

typedef struct lexeme
{
    int line_number;
    Type type;
    int array_size;
    union 
    {
        char *string;
        char character;
        int integer;
        double real;
        struct lexeme **array;
        struct lexeme *(*fp)(struct lexeme *);
    } value;
    struct lexeme *left;
    struct lexeme *right;
} Lexeme;

FILE *lex_fopen(char *filename);
Lexeme *new_Lexeme(Type type);
Lexeme *lex(FILE *fp);
void display_Lexeme_value(Lexeme *l);
char *lexeme_info(Lexeme *l);
char *type_to_string(Type type);
Lexeme *true_Lexeme(void);
Lexeme *false_Lexeme(void);
Lexeme *nil_Lexeme(void);

#endif
