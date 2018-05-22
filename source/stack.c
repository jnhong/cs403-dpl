#include "stack.h" 
#include <stdlib.h>
#include <stdio.h>

Stack *new_Stack(void)
{
    Stack *s = (Stack *) malloc(sizeof(Stack));
    s->size = 0;
    s->head = NULL;
    return s;
}

void push_Stack(Stack *s, void *p)
{
    Node *n = malloc(sizeof(Node));
    n->handle = p;
    n->next = s->head;
    s->head = n;
    (s->size)++;
}

void *pop_Stack(Stack *s)
{
    if (is_empty_Stack(s))
    {
        fprintf(stderr,"attempt to pop empty stack.\n");
        exit(EXIT_SUCCESS);
    }
    Node *n = s->head;
    s->head = s->head->next;
    void *p = n->handle;
    free(n);
    (s->size)--;
    return p;
}

void *peek_Stack(Stack *s)
{
    if (is_empty_Stack(s))
    {
        fprintf(stderr,"attempt to peek empty stack.\n");
        exit(EXIT_SUCCESS);
    }
    return s->head->handle;
}

int is_empty_Stack(Stack *s)
{
    return s->size == 0;
}

void free_Stack(Stack *s)
{
    if (!is_empty_Stack(s))
    {
        fprintf(stderr,"attempt to free non-empty stack.\n");
        exit(EXIT_SUCCESS);
    }
    free(s);
}

int size_Stack(Stack *s)
{
    return s->size;
}
