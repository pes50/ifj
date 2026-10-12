/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file stack.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of stack using a linked list
 *
\*****************************************************************************/

#include "stack.h"
#include <stdlib.h>

t_stack *create_stack() { return calloc(1, sizeof(t_stack)); }

bool stack_push(t_stack *stack, void *data) {
  if (stack == NULL) {
    return false;
  }

  // create a new element
  t_stack_element *element = malloc(sizeof(t_stack_element));
  if (element == NULL) {
    return false;
  }
  element->data = data;
  element->next = stack->head;

  // add the element to the stack
  stack->head = element;
  stack->size++;

  return true;
}

bool stack_pop(t_stack *stack, void **data) {
  if (stack == NULL || stack->size == 0) {
    return false;
  }

  t_stack_element *element = stack->head;

  if (data != NULL) {
    *data = element->data;
  }

  // remove element
  stack->head = element->next;
  stack->size--;
  free(element);

  return true;
}

t_stack_element *stack_head(t_stack *stack) {
  if (stack == NULL || stack->size == 0) {
    return NULL;
  }

  return stack->head;
}