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

/**
 * @brief Allocates and initializes a new empty stack
 *
 * @return A pointer to the allocated stack or NULL if allocation fails
 */
t_stack *create_stack() { return calloc(1, sizeof(t_stack)); }

/**
 * @brief Pushes a new element onto the top of the stack
 *
 * @param stack A pointer to the target stack
 * @param data A pointer to the data being stored
 *
 * @return true if the element was successfully pushed, false if allocation
 * fails or stack is NULL.
 */
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

/**
 * @brief Removes the top element from the stack and optionally retrieves its
 * data
 *
 * @param stack A pointer to the target stack
 * @param data A pointer used to output the data, can be set to NULL to discard
 * the value
 *
 * @return true on success, false if the stack is empty or NULL
 */
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

/**
 * @brief Returns element on top of the stack
 *
 * @param stack A pointer to the target stack
 *
 * @return A pointer to the top element, or NULL if stack is empty or NULL
 */
t_stack_element *stack_head(t_stack *stack) {
  if (stack == NULL || stack->size == 0) {
    return NULL;
  }

  return stack->head;
}