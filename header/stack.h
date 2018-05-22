#ifndef STACK_H
#define STACK_H

typedef struct node
{
    void *handle;
    struct node *next;
} Node;

typedef struct stack
{
    int size;
    Node *head;
} Stack;

Stack *new_Stack(void);
void push_Stack(Stack *s, void *p);
void *pop_Stack(Stack *s);
void *peek_Stack(Stack *s);
int is_empty_Stack(Stack *s);
void free_Stack(Stack *s);
int size_Stack(Stack *s);

#endif
