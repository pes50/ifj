/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_node.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Interface of AST node to 3AC statement conversion
 *        (1 node -> 1 statement)
 *
 \*****************************************************************************/

#ifndef CONVERT_NODE_H
#define CONVERT_NODE_H

#include "ast_convert.h"

/**
 * @brief Converts one AST node to 3AC statement
 *
 * @param context Context structure of the funciton in which the node is
 * @param node Node to convert
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_node(context_t *context, ast_node_t *node);

#endif