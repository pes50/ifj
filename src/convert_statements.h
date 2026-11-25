/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_statements.h
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Specification of AST to 3AC conversion of statements
 *
 \*****************************************************************************/

#pragma once

#include "ast_convert.h"

/**
 * @brief Converts list of AST nodes to 3AC expressions
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the first node to convert
 * @param keep_last Keep last value on stack if true
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_statement_list(context_t *context, ast_node_t *node,
                                  bool keep_last);

/**
 * @brief Converts an AST while do node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the first node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_while_do(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST block node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the first node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_block(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST variable declaration node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_var_declaration(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST assignment node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_assignment(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST return node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_return(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST for do node to 3AC
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_for_do(context_t *context, ast_node_t *node);