/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_expression.h
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Specification of AST to 3AC conversion of expressions
 *
\*****************************************************************************/

#pragma once

#include "ast_convert.h"
#include <stdbool.h>

/**
 * @brief Converts expression using stack instruction and
 *        puts result on top of data stack (in ifjcode26)
 *
 * @param context Context structure
 * @param node Node with the root of the expression subtree
 */
err_code_t convert_expression_on_stack(context_t *context, ast_node_t *node);

/**
 * @brief Converts AST function call node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node Node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_function_call(context_t *context, ast_node_t *node);

/**
 * @brief Converts AST if node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node Node to convert
 * @param push_result Push node value to stack if true
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_if(context_t *context, ast_node_t *node, bool push_result);

/**
 * @brief Converts AST lambda declaration node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node Node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_lambda_expression(context_t *context, ast_node_t *node);