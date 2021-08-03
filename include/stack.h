#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>
#include <stdbool.h>

struct stack {
  void         *value;
  struct stack *next;
};

extern inline bool empty(struct stack *stack) { return stack == NULL; }

extern inline void *top(struct stack *stack){ return empty(stack) ? NULL : stack->value; }

extern inline void push(struct stack **stack, void *value){
  struct stack *top = malloc(sizeof(struct stack));
  top->next = *stack;
  top->value = value;
  *stack = top;
}

extern inline void *pop(struct stack **stack){
  if (empty(*stack))  return NULL;
  struct stack *top = *stack;
  void  *value      = top->value;
  *stack            = top->next;
  free(top);
  return value;
}

extern inline void destroy(struct stack **stack) {
  register struct stack *top;
  
  while (!empty(*stack)){
    top = *stack;
    *stack = top->next;
    free(top);
  }
}

#endif /* _STACK_H */
