#include "lexeme.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
// #define JTL_COUNT 24 // number of just type lexemes
#define JTL_COUNT 3 // number of just type lexemes

/*
Notes:
    - No test for integer overflow in lex_number().
    - Need to implement arrays, and display arrays
    - No symbols
*/

// CONSTRUCTORS ///////////////////////////////////////////////
static Lexeme *new_Lexeme_string(Type type, char *string);
static Lexeme *new_Lexeme_integer(Type type, int integer);
static Lexeme *new_Lexeme_real(Type type, double real);

// LEX HELPERS ////////////////////////////////////////////////
static Lexeme *lex_not_or_not_equals(FILE *fp);
static Lexeme *lex_colon_or_define_or_enddefine(FILE *fp);
static Lexeme *lex_equals_or_assign(FILE *fp);
static Lexeme *lex_less_than(FILE *fp);
static Lexeme *lex_greater_than(FILE *fp);
static Lexeme *lex_number(FILE *fp);
static Lexeme *lex_variable_or_keyword(FILE *fp);
static Lexeme *lex_string(FILE *fp);

// OTHER //////////////////////////////////////////////////////
char escape_char(char ch);
static char skip_whitespace(FILE *fp);
char *lexeme_info_helper(Lexeme *l);

Lexeme *new_just_type_Lexeme(Type type);
void initialize_just_type_Lexemes(void);

// GLOBAL VARIALBLES //////////////////////////////////////////
static int line_number = 1;

// OTHER //////////////////////////////////////////////////////

/* this is needed to reset the line number counter and initialize jtLs */
FILE *lex_fopen(char *filename)
{
    line_number = 1;
    initialize_just_type_Lexemes();
    return fopen(filename,"r");
}

// CONSTRUCTORS ///////////////////////////////////////////////

Lexeme **just_type_Lexemes;

void initialize_just_type_Lexemes(void)
{
    just_type_Lexemes = malloc(JTL_COUNT * sizeof(Lexeme *));
    Lexeme **jtl = just_type_Lexemes;

    jtl[TRUE] = new_just_type_Lexeme(TRUE);
    jtl[FALSE] = new_just_type_Lexeme(FALSE);
    jtl[NIL] = new_just_type_Lexeme(NIL);

    /*
    jtl[THIS] = new_just_type_Lexeme(THIS);
    jtl[VERTICAL_BAR] = new_just_type_Lexeme(VERTICAL_BAR);
    jtl[OPEN_PAREN] = new_just_type_Lexeme(OPEN_PAREN);
    jtl[CLOSE_PAREN] = new_just_type_Lexeme(CLOSE_PAREN);
    jtl[OPEN_BRACKET] = new_just_type_Lexeme(OPEN_BRACKET);
    jtl[CLOSE_BRACKET] = new_just_type_Lexeme(CLOSE_BRACKET);
    jtl[OPEN_BRACE] = new_just_type_Lexeme(OPEN_BRACE);
    jtl[CLOSE_BRACE] = new_just_type_Lexeme(CLOSE_BRACE);
    jtl[COMMA] = new_just_type_Lexeme(COMMA);
    jtl[SEMICOLON] = new_just_type_Lexeme(SEMICOLON);
    jtl[COLON] = new_just_type_Lexeme(COLON);
    jtl[END_DEFINE] = new_just_type_Lexeme(END_DEFINE);
    jtl[QUESTION_MARK] = new_just_type_Lexeme(QUESTION_MARK);
    jtl[ELSE] = new_just_type_Lexeme(ELSE);
    jtl[UNKNOWN] = new_just_type_Lexeme(UNKNOWN);
    jtl[END_OF_INPUT] = new_just_type_Lexeme(END_OF_INPUT);

    jtl[DELAY] = new_just_type_Lexeme(DELAY);
    jtl[FORCE] = new_just_type_Lexeme(FORCE);
    jtl[SCONS] = new_just_type_Lexeme(SCONS);
    jtl[SCAR] = new_just_type_Lexeme(SCAR);
    jtl[SCDR] = new_just_type_Lexeme(SCDR);
    */
}

/*
void free_discarded_lexemes(void)
{
    int i;
    for (i = 2; i < 20; ++i)
        free(just_type_Lexemes[i]);
}
*/

Lexeme *new_just_type_Lexeme(Type type)
{
    Lexeme *l = malloc(sizeof(*l));
    l->line_number = line_number;
    l->type = type;
    return l;
}

Lexeme *true_Lexeme(void)
{
    return just_type_Lexemes[TRUE];
}

Lexeme *false_Lexeme(void)
{
    return just_type_Lexemes[FALSE];
}

Lexeme *nil_Lexeme(void)
{
    return just_type_Lexemes[NIL];
}

Lexeme *new_Lexeme(Type type)
{
    /*
    if (type < JTL_COUNT)
        return just_type_Lexemes[type];
    */

    Lexeme *l = malloc(sizeof(*l));
    l->line_number = line_number;
    l->type = type;
    l->value.integer = 0;
    l->left = NULL;
    l->right = NULL;
    return l;
}

static Lexeme *new_Lexeme_string(Type type, char *string)
{
    Lexeme *l = new_Lexeme(type);
    l->value.string = string;
    return l;
}

static Lexeme *new_Lexeme_integer(Type type, int integer)
{
    Lexeme *l = new_Lexeme(type);
    l->value.integer = integer;
    return l;
}

static Lexeme *new_Lexeme_real(Type type, double real)
{
    Lexeme *l = new_Lexeme(type);
    l->value.real = real;
    return l;
}


// LEX IMPLEMENTATION ////////////////////////////////////////////////

Lexeme *lex(FILE *fp)
{
    int ch;

    ch = skip_whitespace(fp);

    if (ch == EOF) return new_Lexeme(END_OF_INPUT);

    switch (ch)
    {
        case '(' : return new_Lexeme(OPEN_PAREN);
        case ')' : return new_Lexeme(CLOSE_PAREN);
        case '[' : return new_Lexeme(OPEN_BRACKET);
        case ']' : return new_Lexeme(CLOSE_BRACKET);
        case '{' : return new_Lexeme(OPEN_BRACE);
        case '}' : return new_Lexeme(CLOSE_BRACE);
        case '|' : return new_Lexeme(VERTICAL_BAR);
        case ',' : return new_Lexeme(COMMA);
        case ';' : return new_Lexeme(SEMICOLON);
        case '+' : return new_Lexeme(PLUS);
        case '-' : return new_Lexeme(MINUS);
        case '*' : return new_Lexeme(TIMES);
        case '/' : return new_Lexeme(DIVIDED_BY);
        case '^' : return new_Lexeme(EXPONENT);
        case '%' : return new_Lexeme(REMAINDER);
        case '?' : return new_Lexeme(QUESTION_MARK);
        case '~' : return lex_not_or_not_equals(fp);
        case ':' : return lex_colon_or_define_or_enddefine(fp);
        case '=' : return lex_equals_or_assign(fp);
        case '<' : return lex_less_than(fp);
        case '>' : return lex_greater_than(fp);

        default :
            if (isdigit(ch))
            {
                ungetc(ch,fp);
                return lex_number(fp);
            }
            else if (isalpha(ch))
            {
                ungetc(ch,fp);
                return lex_variable_or_keyword(fp);
            }
            else if (ch == '\"')
            {
                return lex_string(fp);
            }
            fprintf(stderr, "line %d: Unknown character \'%c\'.\n",line_number, ch);
            exit(EXIT_FAILURE);
    }
}

// LEX HELPERS ////////////////////////////////////////////////

static Lexeme *lex_not_or_not_equals(FILE *fp)
{
    int ch = fgetc(fp);
    if (ch == '=') return new_Lexeme(NOT_EQUALS);
    ungetc(ch,fp);
    return new_Lexeme(NOT);
}

static Lexeme *lex_colon_or_define_or_enddefine(FILE *fp)
{
    //int ch = fgetc(fp);
    int ch = skip_whitespace(fp);
    if (ch == '=') return new_Lexeme(DEFINE);
    if (ch == ';') return new_Lexeme(END_DEFINE);
    ungetc(ch,fp);
    return new_Lexeme(COLON);
}

static Lexeme *lex_equals_or_assign(FILE *fp)
{
    int ch = fgetc(fp);
    if (ch == '=') return new_Lexeme(ASSIGN);
    ungetc(ch,fp);
    return new_Lexeme(EQUALS);
}

static Lexeme *lex_less_than(FILE *fp)
{
    int ch = fgetc(fp);
    if (ch == '=') return new_Lexeme(LESS_THAN_OR_EQUALS);
    ungetc(ch,fp);
    return new_Lexeme(LESS_THAN);
}

static Lexeme *lex_greater_than(FILE *fp)
{
    int ch = fgetc(fp);
    if (ch == '=') return new_Lexeme(GREATER_THAN_OR_EQUALS);
    ungetc(ch,fp);
    return new_Lexeme(GREATER_THAN);
}

static Lexeme *lex_number(FILE *fp)
{
    int ch;
    int integer = 0;
    int digit;

    while (isdigit(ch = fgetc(fp)))
    {
        digit = ch - '0';
        integer = 10*integer + digit;
    }

    if (ch == '.') // is a real number, so read decimal digits
    {
        double real = (double) integer; // store results
        integer = 0; // to collect digits after decimal
        int decimal_places = 0; // to shift value

        while (isdigit(ch = fgetc(fp)))
        {
            digit = ch - '0';
            integer = 10*integer + digit;
            ++decimal_places;
        }
        real += integer * pow(10,-decimal_places); // add decimal part

        ungetc(ch,fp);
        return new_Lexeme_real(REAL,real);
    }

    ungetc(ch,fp);
    return new_Lexeme_integer(INTEGER,integer);
}

static Lexeme *lex_variable_or_keyword(FILE *fp)
{
    char ch = fgetc(fp); // guaranteed at least one character

    int length = 0;
    int capacity = 2; // there will always be at least 2 characters
    char *string =  malloc(capacity * sizeof(char));

    string[length++] = ch;

    while (isalnum(ch = fgetc(fp)) || ch == '_')
    {
        if (length == capacity) // expand
        {
            capacity *= 2;
            string = realloc(string,capacity);
        }
        string[length++] = ch;
    }

    ungetc(ch,fp);

    if (length == capacity) // expand
    {
        capacity += 1;
        string = realloc(string,capacity);
    }
    string[length] = '\0';

    // keyword
    if(strcmp(string,"true") == 0)        return new_Lexeme(TRUE);
    else if(strcmp(string,"false") == 0)  return new_Lexeme(FALSE);
    else if(strcmp(string,"div") == 0)    return new_Lexeme(DIV);
    else if(strcmp(string,"mod") == 0)    return new_Lexeme(MOD);
    else if(strcmp(string,"equals") == 0) return new_Lexeme(EQUALS);
    else if(strcmp(string,"not") == 0)    return new_Lexeme(NOT);
    else if(strcmp(string,"and") == 0)    return new_Lexeme(AND);
    else if(strcmp(string,"or") == 0)     return new_Lexeme(OR);
    else if(strcmp(string,"if") == 0)     return new_Lexeme(IF);
    else if(strcmp(string,"else") == 0)   return new_Lexeme(ELSE);
    else if(strcmp(string,"while") == 0)  return new_Lexeme(WHILE);
    //else if(strcmp(string,"array") == 0)  return new_Lexeme(ARRAY);
    else if(strcmp(string,"lambda") == 0) return new_Lexeme(LAMBDA);
    else if(strcmp(string,"nil") == 0)    return new_Lexeme(NIL);
    else if(strcmp(string,"this") == 0)   return new_Lexeme(THIS);
    else if(strcmp(string,"delay") == 0)  return new_Lexeme(DELAY);
    else if(strcmp(string,"force") == 0)  return new_Lexeme(FORCE);
    else if(strcmp(string,"scons") == 0)  return new_Lexeme(SCONS);
    else if(strcmp(string,"scar") == 0)  return new_Lexeme(SCAR);
    else if(strcmp(string,"scdr") == 0)  return new_Lexeme(SCDR);
    else if(strcmp(string,"return") == 0)  return new_Lexeme(RETURN);
    // variable
    else return new_Lexeme_string(VARIABLE,string);
}

char escape_char(char ch)
{
    switch (ch)
    {
        case 'n': return '\n';
        case 't': return '\t';
        case 'b': return '\b';
        case 'v': return '\v';
        case 'f': return '\f';
        case 'r': return '\r';
        case 'a': return '\a';
        default:  return ch;
    }
}

static Lexeme *lex_string(FILE *fp)
{
    char ch = fgetc(fp); // guaranteed at least one character

    int escape = 0;
    // check if starting with escape character
    if (ch == '\\')
    {
        escape = 1;
        ch = fgetc(fp);
        ch = escape_char(ch);
    }

    int length = 0;
    int capacity = 2; // there will always be at least 2 characters
    char *string = malloc(capacity * sizeof(char));

    string[length++] = ch;
    escape = 0;

    while (((ch = fgetc(fp)) != EOF) && ((ch != '"') || (escape == 1)))
    {
        if (ch == '\n') ++line_number;

        if (escape == 1) ch = escape_char(ch);

        if ((escape == 0) && (ch == '\\'))
            escape = 1;
        else
        {
            if (length == capacity)
            {
                capacity *= 2;
                string = realloc(string,capacity);
            }
            string[length++] = ch;
            escape = 0;
        }
    }

    if (length == capacity)
    {
        capacity += 1;
        string = realloc(string,capacity);
    }
    string[length] = '\0';

    return new_Lexeme_string(STRING,string);
}


// OTHER //////////////////////////////////////////////////////


void display_Lexeme_value(Lexeme *l)
{
    switch (l->type)
    {
        case DEFINE : printf("DEFINE\n"); break;
        case END_DEFINE : printf("END_DEFINE\n"); break;
        case ASSIGN : printf("ASSIGN\n"); break;
        case VARIABLE : printf("VARIABLE\t%s\n",l->value.string); break;
        case INTEGER : printf("INTEGER\t%d\n",l->value.integer); break;
        case REAL : printf("REAL\t%f\n",l->value.real); break;
        case TRUE : printf("true\t"); break;
        case FALSE : printf("false\n"); break;
        case STRING : printf("STRING\t\"%s\"\n",l->value.string); break;
        case OPEN_PAREN : printf("OPEN_PAREN\n"); break;
        case CLOSE_PAREN : printf("CLOSE_PAREN\n"); break;
        case OPEN_BRACKET : printf("OPEN_BRACKET\n"); break;
        case CLOSE_BRACKET : printf("CLOSE_BRACKET\n"); break;
        case OPEN_BRACE : printf("OPEN_BRACE\n"); break;
        case CLOSE_BRACE : printf("CLOSE_BRACE\n"); break;
        case VERTICAL_BAR : printf("VERTICAL_BAR\n"); break;
        case COMMA : printf("COMMA\n"); break;
        case SEMICOLON : printf("SEMICOLON\n"); break;
        case COLON : printf("COLON\n"); break;
        case PLUS : printf("PLUS\n"); break;
        case MINUS : printf("MINUS\n"); break;
        case TIMES : printf("TIMES\n"); break;
        case DIVIDED_BY : printf("DIVIDED_BY\n"); break;
        case EXPONENT : printf("EXPONENT\n"); break;
        case REMAINDER : printf("REMAINDER\n"); break;
        case DIV : printf("DIV\n"); break;
        case MOD : printf("MOD\n"); break;
        case EQUALS : printf("EQUALS\n"); break;
        case NOT_EQUALS : printf("NOT_EQUALS\n"); break;
        case LESS_THAN : printf("LESS_THAN\n"); break;
        case GREATER_THAN : printf("GREATER_THAN\n"); break;
        case LESS_THAN_OR_EQUALS : printf("LESS_THAN_OR_EQUALS\n"); break;
        case GREATER_THAN_OR_EQUALS : printf("GREATER_THAN_OR_EQUALS\n"); break;
        case NOT : printf("NOT\n"); break;
        case AND : printf("AND\n"); break;
        case OR : printf("OR\n"); break;
        case IF : printf("IF\n"); break;
        case QUESTION_MARK : printf("QUESTION_MARK\n"); break;
        case ELSE : printf("ELSE\n"); break;
        case WHILE : printf("WHILE\n"); break;
        case UNKNOWN : printf("UNKNOWN\n"); break;
        case END_OF_INPUT : printf("END_OF_INPUT\n"); break;
        case LINK : printf("LINK\n"); break;
        case NIL : printf("NIL\n"); break;
        case UNINITIALIZED : printf("UNINITIALIZED\n"); break;
        case FUNCTION_CALL : printf("FUNCTION_CALL\n"); break;
        case UNARY_PLUS : printf("UNARY_PLUS\n"); break;
        case UNARY_MINUS : printf("UNARY_MINUS\n"); break;
        case ARRAY_ACCESS : printf("ARRAY_ACCESS\n"); break;
        case PARENTHESIS : printf("PARENTHESIS\n"); break;
        case ABSOLUTE_VALUE : printf("ABSOLUTE_VALUE\n"); break;
        case CURLY_BRACES : printf("CURLY_BRACES\n"); break;
        case CASE : printf("CASE\n"); break;
        case WHILE_SPLIT : printf("WHILE_SPLIT\n"); break;
        case ARRAY : printf("ARRAY\n"); break;
        case LAMBDA : printf("LAMBDA\n"); break;
        case CLOSURE : printf("CLOSURE\n"); break;
        case BUILTIN : printf("BUILTIN\n"); break;
        case ENV : printf("ENV\n"); break;
        case TABLE : printf("TABLE\n"); break;
        case BODY : printf("BODY\n"); break;
        case THIS : printf("THIS\n"); break;
        case DELAY : printf("DELAY\n"); break;
        case FORCE : printf("FORCE\n"); break;
        case SCONS : printf("SCONS\n"); break;
        case SCAR : printf("SCAR\n"); break;
        case SCDR : printf("SCDR\n"); break;
        case RETURN : printf("RETURN\n"); break;
        default : printf("Unexpected type: %d\n",l->type);
    }
}

char *lexeme_info(Lexeme *l)
{
    char *s = lexeme_info_helper(l);
    int max = 64;
    char *t =  malloc(max * sizeof(char));
    sprintf(t," (line %d)",l->line_number);
    strcat(s,t);
    free(t);
    return s;
}

char *lexeme_info_helper(Lexeme *l)
{
    int max = 512;
    int max_copy = 256; 
    char *s = malloc(max * sizeof(char));
    switch (l->type)
    {
        case DEFINE :
            strcpy(s, "DEFINE ':='");
            return s;
        case END_DEFINE :
            strcpy(s, "END_DEFINE ':;'");
            return s;
        case ASSIGN :
            strcpy(s,"ASSIGN '=='");
            return s;
        case VARIABLE : 
            {
                strcpy(s,"VARIABLE ");
                strncat(s,l->value.string,max_copy);
                return s;
            }
        case INTEGER :
            {
                char *t = malloc(max_copy * sizeof(char));
                strcpy(s,"INTEGER ");
                sprintf(t,"%d",l->value.integer);
                strncat(s,t,max_copy);
                free(t);
                return s;
            }
        case REAL : 
            {
                char *t = malloc(max_copy * sizeof(char));
                strcpy(s,"REAL ");
                sprintf(t,"%f",l->value.real);
                strncat(s,t,max_copy);
                free(t);
                return s;
            }
        case TRUE :
            strcpy(s,"true");
            return s;
        case FALSE :
            strcpy(s,"false");
            return s;
        case STRING :
            {
                strcpy(s,"STRING \"");
                strncat(s,l->value.string,max_copy);
                strcat(s,"\"");
                return s;
            }
        case OPEN_PAREN :
            strcpy(s,"OPEN_PAREN '('");
            return s;
        case CLOSE_PAREN :
            strcpy(s,"CLOSE_PAREN ')'");
            return s;
        case OPEN_BRACKET :
            strcpy(s,"OPEN_BRACKET '['");
            return s;
        case CLOSE_BRACKET : 
            strcpy(s,"CLOSE_BRACKET ']'");
            return s;
        case OPEN_BRACE : 
            strcpy(s,"OPEN_BRACE '{'");
            return s;
        case CLOSE_BRACE : 
            strcpy(s,"CLOSE_BRACE '}'");
            return s;
        case VERTICAL_BAR : 
            strcpy(s,"VERTICAL_BAR '|'");
            return s;
        case COMMA : 
            strcpy(s,"COMMA ','");
            return s;
        case SEMICOLON : 
            strcpy(s,"SEMICOLON ';'");
            return s;
        case COLON : 
            strcpy(s,"COLON ':'");
            return s;
        case PLUS : 
            strcpy(s,"PLUS '+'");
            return s;
        case MINUS : 
            strcpy(s,"MINUS '-'");
            return s;
        case TIMES : 
            strcpy(s,"TIMES '*'");
            return s;
        case DIVIDED_BY : 
            strcpy(s,"DIVIDED_BY '/'");
            return s;
        case EXPONENT : 
            strcpy(s,"EXPONENT '^'");
            return s;
        case REMAINDER : 
            strcpy(s,"REMAINDER '%'");
            return s;
        case DIV : 
            strcpy(s,"DIV 'div'");
            return s;
        case MOD : 
            strcpy(s,"MOD 'mod'");
            return s;
        case EQUALS : 
            strcpy(s,"EQUALS '=' or 'equals'");
            return s;
        case NOT_EQUALS : 
            strcpy(s,"NOT_EQUALS '~=' or 'not equals'");
            return s;
        case LESS_THAN : 
            strcpy(s,"LESS_THAN '<'");
            return s;
        case GREATER_THAN : 
            strcpy(s,"GREATER_THAN '>'");
            return s;
        case LESS_THAN_OR_EQUALS : 
            strcpy(s,"LESS_THAN_OR_EQUALS '<='");
            return s;
        case GREATER_THAN_OR_EQUALS : 
            strcpy(s,"GREATER_THAN_OR_EQUALS '>='");
            return s;
        case NOT : 
            strcpy(s,"NOT '~'");
            return s;
        case AND : 
            strcpy(s,"AND 'and'");
            return s;
        case OR : 
            strcpy(s,"OR 'or'");
            return s;
        case IF : 
            strcpy(s,"IF 'if'");
            return s;
        case QUESTION_MARK : 
            strcpy(s,"QUESTION_MARK '?'");
            return s;
        case ELSE : 
            strcpy(s,"ELSE 'else'");
            return s;
        case WHILE : 
            strcpy(s,"WHILE 'while'");
            return s;
        case UNKNOWN : 
            strcpy(s,"UNKNOWN");
            return s;
        case END_OF_INPUT : 
            strcpy(s,"END_OF_INPUT");
            return s;
        case LINK : 
            strcpy(s,"LINK");
            return s;
        case NIL : 
            strcpy(s,"NIL");
            return s;
        case UNINITIALIZED : 
            strcpy(s,"UNINITIALIZED");
            return s;
        case FUNCTION_CALL : 
            strcpy(s,"FUNCTION_CALL");
            return s;
        case UNARY_PLUS : 
            strcpy(s,"UNARY_PLUS");
            return s;
        case UNARY_MINUS : 
            strcpy(s,"UNARY_MINUS");
            return s;
        case ARRAY_ACCESS : 
            strcpy(s,"ARRAY_ACCESS");
            return s;
        case PARENTHESIS : 
            strcpy(s,"PARENTHESIS");
            return s;
        case ABSOLUTE_VALUE : 
            strcpy(s,"ABSOLUTE_VALUE");
            return s;
        case CURLY_BRACES : 
            strcpy(s,"CURLY_BRACES");
            return s;
        case CASE : 
            strcpy(s,"CASE");
            return s;
        case WHILE_SPLIT : 
            strcpy(s,"WHILE_SPLIT");
            return s;
        case ARRAY : 
            strcpy(s,"ARRAY");
            return s;
        case LAMBDA : 
            strcpy(s,"LAMBDA");
            return s;
        case CLOSURE : 
            strcpy(s,"CLOSURE");
            return s;
        case BODY: 
            strcpy(s,"BODY");
            return s;
        case BUILTIN: 
            strcpy(s,"BUILTIN");
            return s;
        case ENV : 
            strcpy(s,"ENV");
            return s;
        case TABLE : 
            strcpy(s,"TABLE");
            return s;
        case THIS: 
            strcpy(s,"THIS");
            return s;
        case DELAY: 
            strcpy(s,"DELAY");
            return s;
        case FORCE: 
            strcpy(s,"FORCE");
            return s;
        case SCONS: 
            strcpy(s,"SCONS");
            return s;
        case SCAR: 
            strcpy(s,"SCAR");
            return s;
        case SCDR: 
            strcpy(s,"SCDR");
            return s;
        case RETURN: 
            strcpy(s,"RETURN");
            return s;
        default :
            printf("Unexpected type: %d\n",l->type);
            exit(EXIT_FAILURE);
    }
}

char *type_to_string(Type type)
{
    switch (type)
    {
        case DEFINE : return "DEFINE";
        case END_DEFINE : return "END_DEFINE";
        case ASSIGN : return "ASSIGN";
        case VARIABLE : return "VARIABLE";
        case INTEGER : return "INTEGER";
        case REAL : return "REAL"; 
        case TRUE : return "TRUE";
        case FALSE : return "FALSE";
        case STRING : return "STRING";
        case OPEN_PAREN : return "OPEN_PAREN";
        case CLOSE_PAREN : return "CLOSE_PAREN";
        case OPEN_BRACKET : return "OPEN_BRACKET";
        case CLOSE_BRACKET : return "CLOSE_BRACKET";
        case OPEN_BRACE : return "OPEN_BRACE";
        case CLOSE_BRACE : return "CLOSE_BRACE";
        case VERTICAL_BAR : return "VERTICAL_BAR";
        case COMMA : return "COMMA";
        case SEMICOLON : return "SEMICOLON";
        case COLON : return "COLON";
        case PLUS : return "PLUS";
        case MINUS : return "MINUS";
        case TIMES : return "TIMES";
        case DIVIDED_BY : return "DIVIDED_BY";
        case EXPONENT : return "EXPONENT";
        case REMAINDER : return "REMAINDER";
        case DIV : return "DIV";
        case MOD : return "MOD";
        case EQUALS : return "EQUALS";
        case NOT_EQUALS : return "NOT_EQUALS";
        case LESS_THAN : return "LESS_THAN";
        case GREATER_THAN : return "GREATER_THAN";
        case LESS_THAN_OR_EQUALS : return "LESS_THAN_OR_EQUALS";
        case GREATER_THAN_OR_EQUALS : return "GREATER_THAN_OR_EQUALS";
        case NOT : return "NOT";
        case AND : return "AND";
        case OR : return "OR";
        case IF : return "IF";
        case QUESTION_MARK : return "QUESTION_MARK";
        case ELSE : return "ELSE";
        case WHILE : return "WHILE";
        case UNKNOWN : return "UNKNOWN";
        case END_OF_INPUT : return "END_OF_INPUT";
        case LINK : return "LINK";
        case NIL : return "NIL";
        case UNINITIALIZED : return "UNINITIALIZED";
        case FUNCTION_CALL : return "FUNCTION_CALL";
        case UNARY_PLUS : return "UNARY_PLUS";
        case UNARY_MINUS : return "UNARY_MINUS";
        case ARRAY_ACCESS : return "ARRAY_ACCESS";
        case PARENTHESIS : return "PARENTHESIS";
        case ABSOLUTE_VALUE : return "ABSOLUTE_VALUE";
        case CURLY_BRACES : return "CURLY_BRACES";
        case CASE : return "CASE";
        case WHILE_SPLIT : return "WHILE_SPLIT";
        case ARRAY : return "ARRAY";
        case LAMBDA : return "LAMBDA";
        case CLOSURE : return "CLOSURE";
        case BODY: return "BODY";
        case BUILTIN : return "BUILTIN";
        case ENV : return "ENV";
        case TABLE : return "TABLE";
        case THIS: return "THIS";
        case DELAY: return "DELAY";
        case FORCE: return "FORCE";
        case SCONS: return "SCONS";
        case SCAR: return "SCAR";
        case SCDR: return "SCDR";
        case RETURN: return "RETURN";
        default :
            printf("Unexpected type: %d\n",type);
            exit(EXIT_FAILURE);
    }
}

static char skip_whitespace(FILE *fp)
{
    int ch;
    int comment = 0;
    // if !! then it is a supercomment that ignores until newline
    int superable = 0;

    while (((ch = fgetc(fp)) != EOF) && (isspace(ch) || (comment > 0) || (ch == '!')))
    {
        if (ch == '\n') ++line_number;

        switch(comment)
        {
            case 0:
                // enter comment if '!'
                if (ch == '\n')
                {
                    superable = 0;
                }
                else if (ch == '!')
                {
                    // just left a comment
                    if (superable == 1)
                    {
                        comment = 2;
                    }
                    // just entered a comment
                    else
                    {
                        superable = 1;
                        comment = 1;
                    }
                }
                break;
            case 1:
                // ignore characters until newline or !
                if (ch == '\n')
                {
                    comment = 0;
                    superable = 0;
                }
                else if (ch == '!')
                {
                    if (superable == 1)
                    {
                        comment = 2;
                    }
                    else
                    {
                        comment = 0;
                        superable = 1;
                    }
                }
                else
                {
                    superable = 0;
                }
                break;
            case 2:
                // ignore all characters until newline
                if (ch == '\n')
                {
                    comment = 0;
                    superable = 0;
                }
                break;
            default:
                break;
        }
    }
    return ch;
}

