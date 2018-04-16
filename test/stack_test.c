#include <stack.h>
#include <stdio.h>

int main(){
    Node *head = stack_init(5);
    stack_push(&head, 1);
    stack_push(&head, 3);

    printf("%d\n", head->val);
    printf("%d\n", head->next->val);
    printf("%d\n", head->next->next->val);

    printf("\n");
    printf("%d\n", stack_pop(&head));
    printf("%d\n", stack_pop(&head));
    printf("%d\n", stack_pop(&head));

    printf("\n");
    printf("%d\n", head->val);
    return 0;
}
