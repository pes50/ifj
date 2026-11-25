/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file analyse_types.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of functions used for analysing types in semantic
analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "symtable.h"

/**
 * @brief Check if type declaration is semantically correct
 *
 * @param node A pointer to an AST type declaration node
 * @param symtable A pointer to the symtable
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_type_declaration(ast_node_t *node, hashtable_t *symtable);

/**
 * @brief Check if class declaration is semantically correct
 *
 * @param node A pointer to an AST class declaration node
 * @param symtable A pointer to the symtable
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_class_declaration(ast_node_t *node, hashtable_t *symtable);

/**
 * @brief Check if data type is semantically correct and resolve aliases
 *
 * @param node A pointer to a data_type node
 * @param symtable A pointer to the symtable
 *
 * @return NO_ERR if correct
 */
err_code_t analyse_type(data_type_node_t *node, hashtable_t *symtable);
