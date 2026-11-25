/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_match.h
 *
 * @author Roman Skoupý     (xskoupr00)
 *
 * @brief Interface for AST to 3AC conversion of match expressions
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "convert_node.h"
#include "err_codes.h"

/**
 * @brief Converts an AST match node to 3AC expression
 *
 * @param context Context structure of the function in which the node is
 * @param node Node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_match(context_t *context, ast_node_t *node);