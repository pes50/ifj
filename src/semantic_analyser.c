/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file semantic_analyser.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of functions used in semantic analysis
 *
\*****************************************************************************/

#include "semantic_analyser.h"
#include "string_duplicate.h"
#include <stdlib.h>

t_ast_node *create_leaf_node(token_t token) {
  // Create node
  t_ast_node *node = create_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  switch (token.type) {
  case TOK_IDENTIFIER:
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = string_duplicate(token.value);
    break;
  case TOK_LIT_INT:
    node->type = AST_LITERAL;
    node->data.literal.data_type = DT_INT;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_DOUBLE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = DT_DOUBLE;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_STRING:
    node->type = AST_LITERAL;
    node->data.literal.data_type = DT_STRING;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_TRUE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = DT_BOOLEAN;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  case TOK_LIT_FALSE:
    node->type = AST_LITERAL;
    node->data.literal.data_type = DT_BOOLEAN;
    node->data.literal.raw = string_duplicate(token.value);
    break;
  default:
    // Invalid token for leaf node
    free(node);
    return NULL;
  }

  // Check if string_duplicate failed
  if (token.type == TOK_IDENTIFIER) {
    if (node->data.identifier.name == NULL) {
      free(node);
      return NULL;
    }
  } else {
    if (node->data.literal.raw == NULL) {
      free(node);
      return NULL;
    }
  }

  return node;
}