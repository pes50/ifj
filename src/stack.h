/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file stack.h
 *
 * @author Matous Jahoda     (xjahodm00)
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of stack
 *
\*****************************************************************************/

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

/**
 * @brief Represents stack element
 */
typedef struct element {
  void *data;
  struct element *next;
} t_stack_element;

/**
 * @brief Represents stack
 */
typedef struct t_stack t_stack;

/**
 * @brief Allocates and initializes a new empty stack
 *
 * @return A pointer to the allocated stack or NULL if allocation fails
 */
t_stack *create_stack();

/**
 * @brief Pushes a new element onto the top of the stack
 *
 * @param stack A pointer to the target stack
 * @param data A pointer to the data being stored
 *
 * @return true if the element was successfully pushed, false if allocation
 * fails or stack is NULL.
 */
bool stack_push(t_stack *stack, void *data);

/**
 * @brief Removes the top element from the stack
 *
 * @param stack A pointer to the target stack
 *
 * @return pointer to data, or NULL if empty
 */
void *stack_pop(t_stack *stack);

/**
 * @brief Returns element on top of the stack
 *
 * @param stack A pointer to the target stack
 *
 * @return A pointer to the top element, or NULL if stack is empty or NULL
 */
t_stack_element *stack_get_top(t_stack *stack);

/**
 * @brief Returns element count in the stack
 *
 * @param stack A pointer to the target stack
 *
 * @return Element count, 0 if stack is NULL
 */
unsigned int stack_get_size(t_stack *stack);

#endif
