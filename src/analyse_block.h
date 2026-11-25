/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file analyse_block.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of analyse_block function for semantic
analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

#define local_scope_table_size 100

/**
 * @brief Checks if AST block node is semantically correct
 *
 * @param node A pointer to an AST block node
 * @param symtable A pointer to the symtable
 * @param explicit_data_type A pointer to a function return data type node, NULL
 * if not defined
 * @param allow_return True if AST return node can be used inside the block
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_block(ast_node_t *node, hashtable_t *symtable,
                         data_type_node_t *explicit_data_type,
                         bool allow_return);

/**
 * @brief Checks if AST return node is semantically correct
 *
 * @param node A pointer to an AST return node
 * @param symtable A pointer to the symtable
 * @param explicit_data_type A pointer to a function return data type node, NULL
 * if not defined
 * @param allow_return True if AST return node can be used inside the block
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_return(ast_node_t *node, hashtable_t *symtable,
                          data_type_node_t *explicit_data_type,
                          bool allow_return);