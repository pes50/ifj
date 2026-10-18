/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file ast_node.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of abstract syntax tree node
 *
\*****************************************************************************/

#include "ast_node.h"
#include <stdlib.h>

t_ast_node *create_node() { return calloc(1, sizeof(t_ast_node)); }