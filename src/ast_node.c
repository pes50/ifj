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

void destroy_node(t_ast_node *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case AST_FUNCTION_DECLARATION:
    free(node->data.function_declaration.name);
    destroy_node(node->data.function_declaration.parameters);
    destroy_node(node->data.function_declaration.body);
    break;
  case AST_FUNCTION_CALL:
    free(node->data.function_call.name);
    destroy_node(node->data.function_call.arguments);
    break;
  case AST_DOT_FUNCTION_CALL:
    free(node->data.dot_function_call.name);
    destroy_node(node->data.dot_function_call.target);
    destroy_node(node->data.dot_function_call.arguments);
    break;
  case AST_PARAMETER:
    free(node->data.parameter.name);
    break;
  case AST_BLOCK:
    destroy_node(node->data.block.statement);
    break;
  case AST_VARIABLE_DECLARATION:
    free(node->data.variable_declaration.name);
    destroy_node(node->data.variable_declaration.expression);
    break;
  case AST_ASSIGNMENT:
    free(node->data.assignment.name);
    destroy_node(node->data.assignment.expression);
    break;
  case AST_FOR_DO:
    destroy_node(node->data.for_do.generator);
    destroy_node(node->data.for_do.guards);
    destroy_node(node->data.for_do.body);
    break;
  case AST_WHILE_DO:
    destroy_node(node->data.while_do.condition);
    destroy_node(node->data.while_do.body);
    break;
  case AST_IF:
    destroy_node(node->data.if_statement.condition);
    destroy_node(node->data.if_statement.true_branch);
    destroy_node(node->data.if_statement.false_branch);
    break;
  case AST_RETURN:
    destroy_node(node->data.return_statement.expression);
    break;
  case AST_BINARY_OPERATION:
    destroy_node(node->data.binary_operation.left);
    destroy_node(node->data.binary_operation.right);
    break;
  case AST_UNARY_OPERATION:
    destroy_node(node->data.unary_operation.expression);
    break;
  case AST_IDENTIFIER:
    free(node->data.identifier.name);
    break;
  case AST_LITERAL:
    free(node->data.literal.raw);
    break;
  }

  destroy_node(node->next);

  free(node);
}