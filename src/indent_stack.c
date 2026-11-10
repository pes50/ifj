
#include "indent_stack.h"

#define MAX_INDENT 100

void indent_stack_init(indent_stack_t *stack) {
  stack->top_idx = 0;
  stack->data[0] = 0;
}

size_t indent_stack_top(indent_stack_t *stack) {
  return stack->data[stack->top_idx];
}

bool indent_stack_push(indent_stack_t *stack, int spaces) {
  if (stack->top_idx >= MAX_INDENT - 1) {
    return false;
  }
  stack->top_idx++;
  stack->data[stack->top_idx] = spaces;
  return true;
}

void indent_stack_pop(indent_stack_t *stack) {
  if (stack->top_idx > 0) {
    stack->top_idx--;
  }
}
