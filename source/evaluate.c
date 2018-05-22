#include "evaluate.h"
#include "eval_math.h"
#include "eval_relational.h"
#include "eval_logic.h"
#include "eval_compound.h"
#include "eval_control.h"
#include "eval_builtin.h"
#include "lexeme.h"
#include "parse.h"
#include "pretty.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// list
Lexeme *cons(Type type, Lexeme *car, Lexeme *cdr);
Lexeme *car(Lexeme *l);
Lexeme *cdr(Lexeme *l);
void set_car(Lexeme *l, Lexeme *car);
void set_cdr(Lexeme *l, Lexeme *cdr);

Type get_type(Lexeme *l);

// environment
Lexeme *make_table(Lexeme *vars, Lexeme *vals);
Lexeme *create_env(void);
Lexeme *insert_env(Lexeme *env, Lexeme *var, Lexeme *val);
Lexeme *lookup_env(Lexeme *env, Lexeme *var);
Lexeme *lookup_local_env(Lexeme *env, Lexeme *target);
int check_env(Lexeme *env, Lexeme *target);
int check_local_env(Lexeme *env, Lexeme *target);
Lexeme *update_env(Lexeme *env, Lexeme *target, Lexeme *new_val);

// evaluate
Lexeme *eval_variable(Lexeme *tree, Lexeme *env);
Lexeme *eval_array_access(Lexeme *tree, Lexeme *env);
Lexeme *eval_define(Lexeme *tree, Lexeme *env);
Lexeme *eval_assign(Lexeme *tree, Lexeme *env);
Lexeme *eval_assign_array_element(Lexeme *tree, Lexeme *env);
Lexeme *eval_func_call(Lexeme *tree, Lexeme *env);
Lexeme *eval_return(Lexeme *tree,Lexeme *env);
Lexeme *eval_lambda(Lexeme *tree, Lexeme *env);

// builtin initialization
Lexeme *insert_builtin(char *var_name, Lexeme *(*fp)(Lexeme *), Lexeme *env);

// error handling
void match_param_args(Lexeme *list1, Lexeme *list2);

void nullcheck(void *p, char *caller)
{
    if (p == NULL)
    {
        fprintf(stderr,"%s: NULL POINTER\n",caller);
        exit(EXIT_FAILURE);
    }
}

Lexeme *cons(Type type, Lexeme *car, Lexeme *cdr)
{
    Lexeme *l = new_Lexeme(type);
    l->left   = car;
    l->right  = cdr;
    return l;
}

Lexeme *car(Lexeme *l)
{
    nullcheck(l,"car");
    return l->left;
}

Lexeme *cdr(Lexeme *l)
{
    nullcheck(l,"cdr");
    return l->right;
}

void set_car(Lexeme *l, Lexeme *car)
{
    nullcheck(l,"set_car");
    l->left = car;
}

void set_cdr(Lexeme *l, Lexeme *cdr)
{
    nullcheck(l,"set_cdr");
    l->right = cdr;
}

Type get_type(Lexeme *l)
{
    nullcheck(l,"get_type");
    return l->type;
}







Lexeme *extend_env(Lexeme *env, Lexeme *vars, Lexeme *vals)
{
    return cons(ENV, env, make_table(vars,vals));
}

Lexeme *make_table(Lexeme *vars, Lexeme *vals)
{
    return cons(TABLE,vars,vals);
}

Lexeme *create_env(void)
{
    return extend_env(nil_Lexeme(), nil_Lexeme(), nil_Lexeme());
}

Lexeme *insert_env(Lexeme *env, Lexeme *var, Lexeme *val)
{
    Lexeme *table = cdr(env);
    set_car(table, cons(LINK, var, car(table)));
    set_cdr(table, cons(LINK, val, cdr(table)));
    return val;
}

Lexeme *lookup_env(Lexeme *env, Lexeme *target)
{
    Lexeme *table, *vars, *vals, *var, *val;

    while (env->type != NIL)
    {
        table = cdr(env);
        vars  = car(table);
        vals  = cdr(table);

        while (vars->type != NIL) 
        {
            var = car(vars);
            val = car(vals);
            if (strcmp(target->value.string,var->value.string) == 0)
                return val;
            vars = cdr(vars);
            vals = cdr(vals);
        }

        env = car(env);
    }

    fprintf(stderr, "lookup_env(): Variable %s is undefined.\n", target->value.string);
    exit(EXIT_FAILURE);
}

Lexeme *lookup_local_env(Lexeme *env, Lexeme *target)
{
    Lexeme *table, *vars, *vals, *var, *val;

    table = cdr(env);
    vars  = car(table);
    vals  = cdr(table);

    while (vars->type != NIL) 
    {
        var = car(vars);
        val = car(vals);
        if (strcmp(target->value.string,var->value.string) == 0)
            return val;

        vars = cdr(vars);
        vals = cdr(vals);
    }

    fprintf(stderr, "lookup_local_env(): Variable %s is undefined.\n", target->value.string);
    exit(EXIT_FAILURE);
}

int check_env(Lexeme *env, Lexeme *target)
{
    Lexeme *table, *vars, *var;

    while (env->type != NIL)
    {
        table = cdr(env);
        vars  = car(table);
        while (vars->type != NIL) 
        {
            var = car(vars);
            if (strcmp(target->value.string,var->value.string) == 0)
                return 1;
            vars = cdr(vars);
        }
        env = car(env);
    }
    return 0;
}

int check_local_env(Lexeme *env, Lexeme *target)
{
    Lexeme *table, *vars, *var;

    if (env->type != NIL)
    {
        table = cdr(env);
        vars  = car(table);
        while (vars->type != NIL) 
        {
            var = car(vars);
            if (strcmp(target->value.string,var->value.string) == 0)
                return 1;
            vars = cdr(vars);
        }
    }

    return 0;
}

Lexeme *update_env(Lexeme *env, Lexeme *target, Lexeme *new_val)
{
    Lexeme *table, *vars, *vals, *var;

    while (env->type != NIL)
    {
        table = cdr(env);
        vars  = car(table);
        vals  = cdr(table);
        while (vars->type != NIL) 
        {
            var = car(vars);
            if (strcmp(target->value.string,var->value.string) == 0)
            {
                set_car(vals,new_val);
                return new_val;
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = car(env);
    }
    fprintf(stderr, "update_env(): Variable %s is undefined.\n", target->value.string);
    exit(EXIT_FAILURE);
}

/* MOVED TO PARSE.C
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
*/

Lexeme *display_env(Lexeme *arglist)
{
    check_argcount(1,"display_env()",arglist);

    Lexeme *env = arglist->left;

    if (env->type != ENV)
    {
        fprintf(stderr, "display_env(): argument not of type ENV: %s\n",
                env->value.string);
        exit(EXIT_FAILURE);
    }

    Lexeme *table, *vars, *vals, *var, *val;

    int i = 0;

    while (env->type != NIL)
    {
        printf("-------[ENVIRONMENT %d]--------\n\n",i++);
        table = cdr(env);
        vars  = car(table);
        vals  = cdr(table);

        while (vars->type != NIL) 
        {
            var = car(vars);
            val = car(vals);

            display_Lexeme_value(var);
            display_Lexeme_value(val);
            printf("\n");

            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = car(env);
    }

    return new_Lexeme(UNINITIALIZED);
}

Lexeme *display_local_env(Lexeme *arglist)
{
    check_argcount(1,"display_local_env()",arglist);

    Lexeme *env = arglist->left;

    if (env->type != ENV)
    {
        fprintf(stderr, "display_env(): argument not of type ENV: %s\n",
                env->value.string);
        exit(EXIT_FAILURE);
    }

    Lexeme *table, *vars, *vals, *var, *val;

    printf("-------[LOCAL ENVIRONMENT]--------\n\n");

    if (env->type != NIL)
    {
        table = cdr(env);
        vars  = car(table);
        vals  = cdr(table);

        while (vars->type != NIL) 
        {
            var = car(vars);
            val = car(vals);

            display_Lexeme_value(var);
            display_Lexeme_value(val);
            printf("\n");

            vars = cdr(vars);
            vals = cdr(vals);
        }
    }

    return new_Lexeme(UNINITIALIZED);
}

Lexeme *eval(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval()");

    switch (tree->type)
    {
        // literals/self-evaluated 
        case INTEGER:       return tree;
        case REAL:          return tree;
        case TRUE:          return tree;
        case FALSE:         return tree;
        case STRING:        return tree;
        case CLOSURE:       return tree;
        case UNINITIALIZED: return tree;
        case NIL:           return tree;
        case ARRAY:         return tree;

        // pseudo-literals
        case THIS:          return env;
        case ELSE:          return true_Lexeme();

        // math
        case PLUS:       return eval_plus(tree,env);
        case MINUS:      return eval_minus(tree,env);
        case TIMES:      return eval_times(tree,env);
        case DIVIDED_BY: return eval_divide(tree,env);
        case EXPONENT:   return eval_pow(tree,env);
        case REMAINDER:  return eval_remainder(tree,env);
        case DIV:        return eval_div(tree,env);
        case MOD:        return eval_mod(tree,env);
        case UNARY_PLUS:     return eval_unary_plus(tree,env);
        case UNARY_MINUS:    return eval_unary_minus(tree,env);
        case ABSOLUTE_VALUE: return eval_abs(tree,env);

        // variables
        case VARIABLE:     return eval_variable(tree,env);
        case ARRAY_ACCESS: return eval_array_access(tree,env);

        // definitions and assignment
        case DEFINE: return eval_define(tree,env);
        case ASSIGN: return eval_assign(tree,env);

        // function call
        case FUNCTION_CALL: return eval_func_call(tree,env);

        // lambda
        case LAMBDA: return eval_lambda(tree,env);

        // relational
        case EQUALS:                 return eval_equals(tree,env);
        case NOT_EQUALS:             return eval_neq(tree,env);
        case LESS_THAN:              return eval_lt(tree,env);
        case GREATER_THAN:           return eval_gt(tree,env);
        case LESS_THAN_OR_EQUALS:    return eval_lt_or_eq(tree,env);
        case GREATER_THAN_OR_EQUALS: return eval_gt_or_eq(tree,env);

        // logical
        case NOT: return eval_not(tree,env);
        case AND: return eval_and(tree,env);
        case OR:  return eval_or(tree,env);

        // conditional
        case IF: return eval_if(tree,env);

        // loop
        case WHILE: return eval_while(tree,env);

        // compound
        case LINK: return eval_list(tree,env); 
        case BODY: return eval_body(tree,env); 
        case PARENTHESIS:  return eval_paren(tree,env);
        case CURLY_BRACES: return eval_block(tree,env);

        case RETURN: return eval_return(tree,env);

        default: break;
    }

    fprintf(stderr,"eval: INVALID LEXEME : %s.\n",type_to_string(tree->type));
    exit(EXIT_FAILURE);
}

Lexeme *eval_variable(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_variable()");

    return lookup_env(env,tree);
}

Lexeme *eval_array_access(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_array_access()");

    // left child should evaluate to an array
    Lexeme *array = eval(tree->left,env);
    if (array->type != ARRAY)
    {
        fprintf(stderr,"eval_array_access(): attempted to array-access non-array object: %s\n", type_to_string(array->type));
        exit(EXIT_FAILURE);
    }
    // right child should evaluate to an integer index < array->array_size
    Lexeme *index = eval(tree->right,env);
    if (index->type != INTEGER)
    {
        fprintf(stderr,"eval_array_access(): index is not an integer: %s\n",
                type_to_string(index->type));
        exit(EXIT_FAILURE);
    }
    else if (index->value.integer < 0 || index->value.integer >= array->array_size)
    {
        fprintf(stderr,"eval_array_access(): index (%d) is out of bounds [0,%d).\n",
                index->value.integer,array->array_size);
        exit(EXIT_FAILURE);
    }
    return array->value.array[index->value.integer];
}

Lexeme *eval_define(Lexeme *tree, Lexeme *env)
{
    Lexeme *var = tree->left; //do not evaluate
    if (var->type != VARIABLE)
    {
        fprintf(stderr,"eval_define(): attempted to define non-variable.\n");
        exit(EXIT_FAILURE);
    }
    if (check_local_env(env,var))
    {
        fprintf(stderr,"line %d: eval_define(): variable %s already defined.\n",
                var->line_number,var->value.string);
        exit(EXIT_FAILURE);
    }
    Lexeme *val = eval(tree->right,env);
    insert_env(env, var, val);
    // DEFINES ARE NOT EXPRESSIONS
    // THEY ARE NEVER FOUND INSIDE ANOTHER EXPRESSION
    // THEY ARE NEVER FOUND AT THE END OF A BODY
    return nil_Lexeme();
}

Lexeme *eval_assign(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_assign()");

    if (tree->left->type == ARRAY_ACCESS)
        return eval_assign_array_element(tree,env);

    Lexeme *var = tree->left; //do not evaluate
    if (var->type != VARIABLE)
    {
        fprintf(stderr,"eval_define(): attempted to assign to non-variable.\n");
        exit(EXIT_FAILURE);
    }
    if (!check_env(env,var))
    {
        fprintf(stderr,"line %d: eval_define(): variable %s not defined.\n",
                var->line_number,var->value.string);
        exit(EXIT_FAILURE);
    }
    Lexeme *val = eval(tree->right,env);
    return update_env(env, var, val);
}

Lexeme *eval_assign_array_element(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_assign_array_element()");

    Lexeme *val = eval(tree->right,env);

    tree = tree->left;
    // left child should evaluate to an array
    Lexeme *array = eval(tree->left,env);
    if (array->type != ARRAY)
    {
        fprintf(stderr,"eval_assign_array_element(): attempted to array-access non-array object.\n");
        exit(EXIT_FAILURE);
    }
    // right child should evaluate to an integer index < array->array_size
    Lexeme *index = eval(tree->right,env);
    if (index->type != INTEGER)
    {
        fprintf(stderr,"eval_assign_array_element(): index is not an integer: %s\n",
                type_to_string(index->type));
        exit(EXIT_FAILURE);
    }
    else if (index->value.integer < 0 || index->value.integer >= array->array_size)
    {
        fprintf(stderr,"eval_assign_array_element(): index (%d) is out of bounds [0,%d).\n",
                index->value.integer,array->array_size);
        exit(EXIT_FAILURE);
    }
    return array->value.array[index->value.integer] = val;
}

void match_param_args(Lexeme *list1, Lexeme *list2)
{
    nullcheck(list1,"match_param_args(): arg \"list1\"");
    nullcheck(list2,"match_param_args(): arg \"list2\"");

    while (list1->type != NIL)
    {
        if (list2->type == NIL)
        {
            fprintf(stderr,"invalid function call: not enough arguments!\n");
            exit(EXIT_FAILURE);
        }
        list1 = list1->right;
        list2 = list2->right;
    }
    if (list2->type != NIL)
    {
        fprintf(stderr,"invalid function call: too many arguments!\n");
        exit(EXIT_FAILURE);
    }
}

Lexeme *eval_func_call(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_func_call()");

    Lexeme *closure = eval(tree->left,env);

    if (closure->type == BUILTIN)
        return closure->value.fp(eval(tree->right,env));

    if (closure->type != CLOSURE)
    {
        fprintf(stderr,"eval_func_call(): object doesn't evaluate to a closure: %s\n",
                type_to_string(closure->type));
        exit(EXIT_FAILURE);
    }

    // get components
    Lexeme *def_env    = closure->left;
    Lexeme *lambda     = closure->right;
    Lexeme *parameters = lambda->left;
    Lexeme *body       = lambda->right;
    Lexeme *arguments  = eval(tree->right,env);

    // check that there are an equal number of elements
    match_param_args(parameters,arguments);

    // extend environment from defining environment and populate table
    Lexeme *x_env      = extend_env(def_env, parameters, arguments);
    // evaluate body in this new environment
    Lexeme *r = eval(body,x_env);
    if(r->type == RETURN)
    {
        return r->right;
    }
    return r;
}

Lexeme *eval_return(Lexeme *tree,Lexeme *env)
{
    if(is_false(tree->left)) // check if already evaluated
    {
        Lexeme *r = new_Lexeme(RETURN);
        r->right = eval(tree->right,env);
        r->left = true_Lexeme();
        return r;
    }
    return tree;
}

void return_error(char *calling_location)
{
    fprintf(stderr,"Cannot return from %s!\n",calling_location);
    exit(EXIT_FAILURE);
}

Lexeme *eval_lambda(Lexeme *tree, Lexeme *env)
{
    nullcheck(tree,"eval_lambda()");

    Lexeme *closure = new_Lexeme(CLOSURE);
    closure->left  = env;  // defining environment
    closure->right = tree; // lambda contains params and body
    return closure;
}

void init_builtins(Lexeme *env)
{
    insert_builtin("print",&eval_print,env);
    insert_builtin("println",&eval_println,env);
    insert_builtin("allocate",&eval_allocate,env);
    insert_builtin("real",&eval_real,env);
    insert_builtin("integer",&eval_integer,env);
    insert_builtin("list",&eval_make_list,env);
    insert_builtin("cons",&eval_cons,env);
    insert_builtin("car",&eval_car,env);
    insert_builtin("cdr",&eval_cdr,env);
    insert_builtin("display_env",&display_env,env);
    insert_builtin("sizeof",&eval_sizeof,env);

    insert_builtin("is_uninitialized",&eval_is_uninitialized,env);
    insert_builtin("is_array",&eval_is_array,env);
    insert_builtin("is_closure",&eval_is_closure,env);
    insert_builtin("is_boolean",&eval_is_boolean,env);
    insert_builtin("is_pair",&eval_is_pair,env);
    insert_builtin("is_list",&eval_is_nil,env);
    insert_builtin("is_nil",&eval_is_nil,env);
    insert_builtin("is_string",&eval_is_string,env);
    insert_builtin("is_integer",&eval_is_integer,env);
    insert_builtin("is_real",&eval_is_real,env);
    insert_builtin("is_return",&eval_is_return,env);
//    insert_builtin("eval",&builtin_eval,env);
}

Lexeme *insert_builtin(char *var_name, Lexeme *(*fp)(Lexeme *), Lexeme *env)
{
    Lexeme *var, *val;

    var = new_Lexeme(VARIABLE);
    var->value.string = var_name;
    val = new_Lexeme(BUILTIN);
    val->value.fp = fp;

    return insert_env(env, var, val);
}

