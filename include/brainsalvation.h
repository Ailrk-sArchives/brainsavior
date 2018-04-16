#ifndef BRAINSALVATION_H
#define BRAINSALVATION_H

#define POINTER_UP      '>'
#define POINTER_DOWN    '<'
#define INCRE           '+'
#define DECRE           '-'
#define INPUT           ','
#define OUTPUT          '.'
#define LOOP_LEFT        '['
#define LOOP_RIGHT       ']'

/* stack element */
typedef struct StackNode{
    struct StackNode *next;
    int val;
}StackNode;
typedef StackNode Stack;

typedef struct bs_execution_context{
    
}Bs_execution_context;

void tape_init();
char* read_file(char*);
void execute_bs(char*);
void execute_bs_by_instr(char, char* , int* , Stack**);
void usage();
void runtime_error();

/* headers for stack operation*/
Stack *stack_init(int);
void stack_push(Stack**, int);
int stack_pop(Stack**);


/* the tape for data io */
int *tape;
// tape_ptr point to the current position on tape.
int tape_ptr;

#endif
