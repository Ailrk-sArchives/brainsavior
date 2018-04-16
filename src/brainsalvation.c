/* 
 * brainsavior is another brother of brainfuck. Compares with bf, bs supports
 * procedure calls and variable binding, which make it brain friendly.
 * 
 *
 * Doesn't like a regular brainfuck implementation which maybe has linked list   
 * array as tape, the primary datastructure in bs is a stack. Initially it has
 * only one node, whenever there is a function defined, the subprocedure will
 * be pushed into the stack. In the meantime, a new language context will
 * established for creating local variables. 
 *
 * src:     https://github.com/brainsavior
 * Author:  Jimmy Yao
 * Email:   jimmy123good@hotmail.com
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "brainsalvation.h"

/* return a initialized node as head of stack */
Stack *stack_init(int val){
    Stack *head = malloc( sizeof(Stack) );
    *head = (Stack) {
        .next = NULL,
        .val = val
    };
    return head;
}

/* push into the top */
void stack_push(Stack **head, int val){
    Stack *new_node = malloc( sizeof(Stack) );
    *new_node = (Stack) {
        .next = *head,
        .val = val
    };
    *head = new_node;
}

/* pop from the top */
int stack_pop(Stack **head){
    int val = (*head)->val;
    Stack *tmp = *head; *head = (*head)->next;
    free(tmp);
    return val;
}

/* execute whole program */
void execute_bs(char* source) {
    int i;
    int pc = 0;
    /*
     * stack for loop to store num of pairs need to ignore to match 
     * the corresponding pair.
     */
    Stack *loop_register;

    /* initialize tape */
    for (i = 0; i < (sizeof(tape)/sizeof(int)); i++){
        tape[i] = 0;
    }i = 0;

    while (source[pc] != '\0'){
//        printf("\npc %d, val: %c\n", pc, source[pc]);
//       printf("tape_ptr %d, val: %d\n", tape_ptr, tape[tape_ptr]);

        execute_bs_by_instr( source[pc], source, &pc, &loop_register);
        pc++;
    }
    printf("\n");
}

void execute_bs_by_instr( char c, char* source, int* pc, Stack**loop_register){
/* execute one instruction */
    int input_integer;
    char input_char;

    /* indicate num of brace need to be ignored to find target pair*/
    int pair_of_brace_to_ignore_register = 0; 
    /* scanpc is only used for scan */
    int scanpc = *pc;

    switch(c){
        case POINTER_UP:
        /* >, INCREase current pointer */
            if ( tape_ptr < sizeof(tape)/sizeof(int) ){
                tape_ptr ++; 
            } else {
                /* ask for more memory */
                tape = (int *)realloc( tape, sizeof(tape)+sizeof(int) );
                tape_ptr ++; 
            } 
            break;
        case POINTER_DOWN:
        /* <, DECREase current pointer */
            if (tape_ptr >= 0) tape_ptr --;
            else runtime_error("POINTER_DOWN: tape_ptr < 0");
            break;

        case INCRE:
        /* +, INCREase val in current cell by 1 */
            tape[tape_ptr] ++;
            break;
        case DECRE:
        /* -, DECREase val in current cell by 1 */
            tape[tape_ptr] --;
            break;

        case INPUT:
        /* ,, ask for INPUT. should be integer. */
            printf("(input) ");
            /* check INPUT validation */
            if (scanf("%d", &input_integer) != 1){
                if(scanf("%c", &input_char) != 1)
                    runtime_error("INPUT: input char or int you dummy");
                else
                    tape[tape_ptr] = (int)input_char;
            } else
                tape[tape_ptr] = input_integer;
            break;
        case OUTPUT:
        /* ., output char. */
            if (tape[tape_ptr] >= 0){
                printf("%c", tape[tape_ptr]);
            }else{
                printf("how should I OUTPUT a negative integer as char?\n");
                printf("Emm???\n");
                exit(-1);
            }
            break;

        case LOOP_LEFT:

            /* each time a new loop found, push a new node into stack */
            (*loop_register == NULL)?
                stack_init(0):stack_push(loop_register, 0);
            
            /* scan code array to find matched pair */
            while(1){
                if ((scanpc + 1) < strlen(source)) scanpc+=1;
                else runtime_error("LOOP_LEFT,scanpc larger than code length");
                /*
                 * pair_of_brace_to_ignore_register increse by 1 when matched
                 * a [
                 *
                 * decrease by 1 when matched a ]
                 *
                 * register  == 0 means the next ] is the target one.
                 * */
                if ( source[scanpc] == '[' ) {
                    
                    (*loop_register)->val += 1;
                    pair_of_brace_to_ignore_register += 1;
                  
                } else if( source[scanpc] == ']' ) {
                    
                    if (pair_of_brace_to_ignore_register > 0) {
                        pair_of_brace_to_ignore_register -= 1;
                    }
                    /* scanpc now stored the index of target ] */
                    else if(pair_of_brace_to_ignore_register < 0){
                        runtime_error("LOOP_LEFT, unmatched loop");
                    } else{
                        break;
                    }
                }
            }

            /* jump past ] if tape[tape_ptr] == 0 */
            if (tape[tape_ptr] == 0) *pc = scanpc + 1;
            break;

        case LOOP_RIGHT:

            /* jump back to target [ */
            if (tape[tape_ptr] != 0) {
                /* the num of [ need to be ignored */
                pair_of_brace_to_ignore_register = (*loop_register)->val;
                    
                while(1) {
                    scanpc -= 1;
                    
                    if (source[scanpc] == '[') {
                         
                         
                        if (pair_of_brace_to_ignore_register > 0) {
                            pair_of_brace_to_ignore_register -= 1;
                            
                        } else if(pair_of_brace_to_ignore_register == 0) {
                            *pc = scanpc;
                            break;    
                        } else {
                            runtime_error("LOOP_RIGHT, unmatched loop");
                        }
                    }
                }
            }
            /* finished the loop, pop */
            if (tape[tape_ptr] == 0) stack_pop(loop_register);
            break;
            
        default:
            printf("unexpected tokens. your brain fucked.\n");
            exit(-1);
    }
}

/* read source code. return a string of bs code*/
char* read_file(char* filename){
    FILE* fp;
    int file_size;
    int i = 0;
    char c;

    fp = fopen(filename, "r");
    if (fp == NULL){
        fprintf(stderr, "Error when opening file:%s\n", strerror(errno));
        runtime_error("No such a file");
    }  

    // get file size
    fseek(fp, 0L, SEEK_END); file_size = ftell(fp)+1; rewind(fp);
    char* raw_source =  malloc(file_size);

    // read char by char from file
    while ((c = fgetc(fp)) != EOF){
        if (c=='\n'||c=='\t'||c==' '||c=='\r' ) continue;
        raw_source[i] = c;
        i++;
    }
    raw_source[i] = '\0';
      
    return raw_source;
    
}

/* init tape in heap */
void tape_init(){
    tape = malloc(sizeof(int));
}

/* print usage for little dummies */
void usage(){
    printf("Usage:  brainsalvation <filename>.bs -<d|i>\n");
    printf("        -d debug\n");
    printf("        -i interative\n");
}

/* error message. */
void runtime_error(char * e){
    printf("\nOpps! error in %s\n", e);
    exit(-1);
}
