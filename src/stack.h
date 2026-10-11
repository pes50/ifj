/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file stack.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of stack
 *
\*****************************************************************************/

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
typedef struct {
  t_stack_element *head;
  unsigned int size;
} t_stack;

t_stack *create_stack();
bool stack_push(t_stack *stack, void *data);
bool stack_pop(t_stack *stack, void **data);
t_stack_element *stack_head(t_stack *stack);
