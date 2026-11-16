/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file expresion_stack.h
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Stack of opened interpolation expressions.
 *
\*****************************************************************************/

#ifndef EXPRESION_STACK_H
#define EXPRESION_STACK_H

#include "err_codes.h"

#include <stdbool.h>
#include <stddef.h>

// Initial capacity of the stack.
#define EXPRESION_STACK_DEPTH 16

/**
 * @brief Stack of expresions.
 */
typedef struct {
  bool *data;
  size_t size;
  size_t capacity;
} expresion_stack_t;

/**
 * @brief Initializes an empty stack.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on allocation failure.
 */
err_code_t expresion_stack_init(expresion_stack_t *stack);

/**
 * @brief Pushes a value on the stack.
 *
 * @param stack     Pointer to the stack.
 * @param ml_string True if string is multiline.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on allocation failure.
 */
err_code_t expresion_stack_push(expresion_stack_t *stack, bool ml_string);

/**
 * @brief Removes the top value and returns it.
 *        Stack must no be empty.
 */
bool expresion_stack_pop(expresion_stack_t *stack);

/**
 * @brief Returns true if the stack is empty.
 */
bool expresion_stack_empty(const expresion_stack_t *stack);

/**
 * @brief Removes all values from the stack.
 */
void expresion_stack_clear(expresion_stack_t *stack);

/**
 * @brief Frees the stack.
 */
void expresion_stack_free(expresion_stack_t *stack);

#endif
