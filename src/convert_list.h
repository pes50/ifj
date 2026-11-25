/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_list.h
 *
 * @author Roman Skoupý     (xskoupr00)
 *
 * @brief Specification of AST to 3AC conversion of lists
 *
\*****************************************************************************/

#pragma once

#include "ast_convert.h"

/**
 * @brief Converts an AST function call node of list constructor to 3AC
 * expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the AST function call node
 *
 * @return NO_ERR on success
 */
err_code_t convert_list_constructor_inline(context_t *context,
                                           ast_node_t *node);

/**
 * @brief Converts a list access AST node of list to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the AST function call node
 *
 * @return NO_ERR on success
 */
err_code_t convert_list_access(context_t *context, ast_node_t *node);

/**
 * @brief Converts an AST binary operation node of list concatenation to 3AC
 * expression
 *
 * @param context Context structure of the function in which the node is
 * @param node A pointer to the AST binary operation node
 *
 * @return NO_ERR on success
 */
err_code_t convert_list_concat_expression(context_t *context, ast_node_t *node);