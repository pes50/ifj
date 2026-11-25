/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file semantic_analyser.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of analyse function used in semantic analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

/**
 * @brief Performs semantic analysis
 *
 * @param root A pointer to the root of AST
 * @param symtable A pointer to the symtable supplied with built in functions
 * and types
 * @param main_node [out] A pointer to store the AST node of the entrypoint of
 * the program
 *
 * @return NO_ERR on success
 */
err_code_t analyse(ast_node_t *root, hashtable_t *symtable,
                   ast_node_t **main_node);