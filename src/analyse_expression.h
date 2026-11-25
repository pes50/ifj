/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file analyse_expression.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of analyse_expression function for semantic analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

/**
 * @brief Checks if AST node representing expression is semantically correct
 *
 * @param node A pointer to an AST node
 * @param symtable A pointer to the symtable
 * @param explicit_data_type A pointer to a function return data type node, NULL
 * if not defined
 * @param is_last True if the expression is the last AST node in current context
 * @param allow_return True if AST return node can be used inside the block
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_expression(ast_node_t *node, hashtable_t *symtable,
                              data_type_node_t *explicit_data_type,
                              bool is_last, bool allow_return);