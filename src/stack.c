/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file stack.c
 *
 * @author Matous Jahoda     (xjahodm00)
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of stack using a linked list
 *
\*****************************************************************************/

#include "stack.h"
#include <stdlib.h>

struct t_stack {
  t_stack_element *head;
  unsigned int size;
};

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

void *stack_pop(t_stack *stack) {
  if (stack == NULL || stack->size == 0) {
    return NULL;
  }

  t_stack_element *element = stack->head;
  void *data = element->data;

  // remove element
  stack->head = element->next;
  stack->size--;
  free(element);

  return data;
}

t_stack_element *stack_get_top(t_stack *stack) {
  if (stack == NULL || stack->size == 0) {
    return NULL;
  }

  return stack->head;
}

unsigned int stack_get_size(t_stack *stack) {
  if (stack == NULL) {
    return 0;
  }
  return stack->size;
}