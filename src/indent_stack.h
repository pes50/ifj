/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file indent_stack.h
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Definition of operations on indentation stack.
 *
\*****************************************************************************/

#ifndef INDENT_STACK_H
#define INDENT_STACK_H

#include "err_codes.h"

#include <stddef.h>

// Initial number of indentation levels.
#define INITIAL_INDENT 100

/**
 * @brief Structure representing an indentation stack.
 */
typedef struct {
  size_t *data;
  size_t top_idx;
  size_t capacity;
} indent_stack_t;

/**
 * @brief Initializes the indentation stack.
 *
 * @param stack Pointer to the stack to initialize.
 */
err_code_t indent_stack_init(indent_stack_t *stack);

/**
 * @brief Returns the indentation level at the top of the stack.
 *
 * @param stack Pointer to the stack.
 *
 * @return Indentation level at the top of the stack.
 */
size_t indent_stack_top(indent_stack_t *stack);

/**
 * @brief Pushes a new value onto the stack.
 *
 * @param stack  Pointer to the stack.
 * @param spaces Value to be pushed on the top of the stack.
 *
 * @return NO_ERR on succeeds.
 *         INTERNAL_ERR if the stack is full.
 */
err_code_t indent_stack_push(indent_stack_t *stack, size_t spaces);

/**
 * @brief Removes the top value from the stack.
 *
 * @param stack Pointer to the stack.
 */
void indent_stack_pop(indent_stack_t *stack);

/**
 * @brief Free memory owned by stack.
 */
void indent_stack_free(indent_stack_t *stack);

#endif
