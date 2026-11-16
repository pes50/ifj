/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file expresion_stack.c
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Implementation of the stack of open interpolation expressions.
 *
\*****************************************************************************/

#include "expresion_stack.h"

#include <stdlib.h>

err_code_t expresion_stack_init(expresion_stack_t *stack) {
  stack->data = (bool *)malloc(EXPRESION_STACK_DEPTH * sizeof(bool));

  if (stack->data == NULL) {
    stack->size = 0;
    stack->capacity = 0;
    return INTERNAL_ERR;
  }

  stack->size = 0;
  stack->capacity = EXPRESION_STACK_DEPTH;
  return NO_ERR;
}

err_code_t expresion_stack_push(expresion_stack_t *stack, bool ml_string) {
  // Double the capacity when the stack is full.
  if (stack->size >= stack->capacity) {
    size_t new_capacity = stack->capacity * 2;
    bool *new_data = (bool *)realloc(stack->data, new_capacity * sizeof(bool));

    if (new_data == NULL) {
      return INTERNAL_ERR;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
  }

  stack->data[stack->size] = ml_string;
  stack->size++;
  return NO_ERR;
}

bool expresion_stack_pop(expresion_stack_t *stack) {
  stack->size--;
  return stack->data[stack->size];
}

bool expresion_stack_empty(const expresion_stack_t *stack) {
  return stack->size == 0;
}

void expresion_stack_clear(expresion_stack_t *stack) { stack->size = 0; }

void expresion_stack_free(expresion_stack_t *stack) {
  if (stack->data != NULL) {
    free(stack->data);
    stack->data = NULL;
  }

  stack->size = 0;
  stack->capacity = 0;
}
