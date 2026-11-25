/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file indent_stack.c
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Implementation of operations on indentation stack.
 *
\*****************************************************************************/

#include "indent_stack.h"

#include <stdlib.h>

err_code_t indent_stack_init(indent_stack_t *stack) {
  stack->data = (size_t *)malloc(INITIAL_INDENT * sizeof(size_t));
  if (stack->data == NULL) {
    return INTERNAL_ERR;
  }

  stack->capacity = INITIAL_INDENT;
  stack->top_idx = 0;
  stack->data[0] = 0;

  return NO_ERR;
}

size_t indent_stack_top(indent_stack_t *stack) {
  return stack->data[stack->top_idx];
}

err_code_t indent_stack_push(indent_stack_t *stack, size_t spaces) {
  // Double the capacity when the current number of values is top_idx + 1
  if (stack->top_idx + 1 >= stack->capacity) {
    size_t new_capacity = stack->capacity * 2;
    size_t *new_data =
        (size_t *)realloc(stack->data, new_capacity * sizeof(size_t));

    if (new_data == NULL) {
      return INTERNAL_ERR;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
  }

  stack->top_idx++;
  stack->data[stack->top_idx] = spaces;
  return NO_ERR;
}

void indent_stack_pop(indent_stack_t *stack) {
  if (stack->top_idx > 0) {
    stack->top_idx--;

    // Halve the stack capacity when the number of values is bellow a quarter
    // and the current capacity is higher than INITIAL_INDENT.
    size_t current_size = stack->top_idx + 1;
    if (stack->capacity > INITIAL_INDENT &&
        current_size <= stack->capacity / 4) {
      size_t new_capacity = stack->capacity / 2;

      if (new_capacity < INITIAL_INDENT) {
        new_capacity = INITIAL_INDENT;
      }

      size_t *new_data =
          (size_t *)realloc(stack->data, new_capacity * sizeof(size_t));
      if (new_data != NULL) {
        stack->data = new_data;
        stack->capacity = new_capacity;
      }
    }
  }
}

void indent_stack_free(indent_stack_t *stack) {
  if (stack->data != NULL) {
    free(stack->data);
    stack->data = NULL;
  }
  stack->capacity = 0;
  stack->top_idx = 0;
}
