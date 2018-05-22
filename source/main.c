#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "lexeme.h"
#include "parse.h"
#include "evaluate.h"
#include "pretty.h"

void scanner(char *filename);

int main(int argc, char *argv[])
{
    int pretty_print_flag = 0;
    int ch;
    
    while ((ch = getopt(argc,argv,"p")) != -1)
    {
        switch (ch)
        {
            case 'p':
                pretty_print_flag = 1;
                break;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr,"Unknown option '-%c'.\n",optopt);
                else
                    fprintf(stderr,"Unknown option character '\\x%x'.\n",optopt);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr,"main():getopt(): failure.\n");
                exit(EXIT_FAILURE);
        }
    }
    
    char *filename = argv[optind];
    
    /*
    printf("\n------- SCANNER -------\n\n");

    scanner(filename);
    */

    Lexeme *parse_tree = parse_program(filename);

    if (pretty_print_flag)
    {
        //printf("\n------- PARSER -------\n\n");
        printf("------- BEGIN PRETTY PRINTING... -------\n\n");

        pretty(parse_tree);
        //print_tree(parse_tree,0); // prints inorder traversal with depth

        printf("\n------- END PRETTY PRINTING... -------\n");

        //printf("\n------- EVALUATOR -------\n\n");
    }
    else
    {
        Lexeme *hidden_env = create_env();

        init_builtins(hidden_env);

        Lexeme *global_env = extend_env(hidden_env,nil_Lexeme(),nil_Lexeme()); 

        eval(parse_tree,global_env);
    }

    return 0;
}

void scanner(char *filename)
{
    FILE *fp = lex_fopen(filename);
    Lexeme *token;
    while ((token = lex(fp))->type != END_OF_INPUT)
        display_Lexeme_value(token);
}
