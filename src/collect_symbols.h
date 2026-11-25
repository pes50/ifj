/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file collect_symbols.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of collect_symbols function used in semantic analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

/**
 * @brief Finds all function, type, class and extension declarations and inserts
 * them into symtable
 *
 * @param root A pointer to the root of AST
 * @param symtable A pointer to the symtable supplied with built in functions
 * and types
 * @param main_node [out] A pointer to store the AST node of the entrypoint of
 * the program
 *
 * @return NO_ERR on success
 */
err_code_t collect_symbols(ast_node_t *root, hashtable_t *symtable,
                           ast_node_t **main_node);