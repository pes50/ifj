/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file ast_convert.h
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Interface of whole AST to 3AC conversion
 *
 \*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "inner_code.h"
#include "stack.h"
#include "symtable.h"

#define TRASH "GF@$trash"

#define visited_table_size 100

/**
 * @brief Structure to contain all relevant data about function context
 */
typedef struct {
  size_t unique_label_counter;
  size_t unique_var_counter;
  t_stack *stack;
  hashtable_t *table;
  inner_statement_t *head;
  inner_statement_t *tail;
  inner_statement_t *function_start;
} context_t;

/**
 * @brief Insert function on stack to wait for its conversion to 3AC
 *        (only if it isn't already waiting for conversion or was converted)
 *
 * @param context Context of the function
 * @param function_declaration_node Node from AST which contains
 *        data of this function's declaration
 *
 * @return NO_ERR if everything ok
 */
err_code_t enqueue_function(context_t *context,
                            ast_node_t *function_declaration_node);

/**
 * @brief Converts analysed AST to 3AC
 *
 * @param main_node A pointer to an AST node representing
 *                  the entrypoint of the program
 * @param [out] code A pointer to store the generated code
 *
 * @return NO_ERR on success
 */
err_code_t ast_convert(ast_node_t *main_node, inner_statement_t **code);