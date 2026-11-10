
#ifndef INDENT_STACK_H
#define INDENT_STACK_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_INDENT 100

typedef struct {
  size_t data[MAX_INDENT];
  size_t top_idx;
} indent_stack_t;

void indent_stack_init(indent_stack_t *stack);

size_t indent_stack_top(indent_stack_t *stack);

bool indent_stack_push(indent_stack_t *stack, int spaces);

void indent_stack_pop(indent_stack_t *stack);

#endif
