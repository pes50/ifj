/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file analyse_callable_declarations.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of analyse_function_declaration and
analyse_lambda_declaration for semantic analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

/**
 * @brief Checks if AST function declaration node is semantically correct
 *
 * @param node A pointer to an AST function declaration node
 * @param symtable A pointer to the symtable
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_function_declaration(ast_node_t *node,
                                        hashtable_t *symtable);

/**
 * @brief Checks if AST lambda declaration node is semantically correct
 *
 * @param node A pointer to an AST lambda declaration node
 * @param symtable A pointer to the symtable
 * @param explicit_data_type A pointer to a lambda return data type node, NULL
 * if not defined
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_lambda_declaration(ast_node_t *node, hashtable_t *symtable,
                                      data_type_node_t *explicit_data_type);