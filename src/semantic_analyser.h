/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file semantic_analyser.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of functions used in semantic analysis
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"

/**
 * @brief Creates an AST node from given token
 *
 * @param token A token of an identifier or a literal
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_leaf_node(token_t token);