/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_node.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Interface of AST node to 3AC statement conversion
 *        (1 node -> 1 statement)
 *
 \*****************************************************************************/

#ifndef CONVERT_NODE_H
#define CONVERT_NODE_H

#include "ast_convert.h"

/**
 * @brief Appends statement into the context list of statements
 *
 * @param context Context structure of the operation
 * @param op Operation to put into statement
 * @param dst Destination of the instruction
 * @param src1 First operand
 * @param src2 Second operand
 *
 * @return NO_ERR if everything ok
 */
err_code_t append_statement(context_t *context, inner_statement_operation_t op,
                            inner_symbol_t dest, inner_symbol_t src1,
                            inner_symbol_t src2);

/**
 * @brief Creates variable identifier with LF@ (local frame) prefix
 *
 * @param identifier Identifier of the variable
 *
 * @return Allocated string with the prefix
 */
char *alloc_lf_var_name(const char *identifier);

/**
 * @brief Converts one AST node to 3AC statement
 *
 * @param context Context structure of the funciton in which the node is
 * @param node Node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_node(context_t *context, ast_node_t *node);

#endif