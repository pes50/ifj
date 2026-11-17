/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_node.c
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of AST node to 3AC statement conversion
 *        (1 node -> 1 statement)
 *
 \*****************************************************************************/

#include "convert_node.h"

/********************\
 * Helper functions *
\********************/

err_code_t convert_function_call(context_t *context, ast_node_t *node);

err_code_t convert_dot_function_call(context_t *context, ast_node_t *node);

err_code_t convert_lambda_declaration(context_t *context, ast_node_t *node);

err_code_t convert_extension_declaration(context_t *context, ast_node_t *node);

err_code_t convert_block(context_t *context, ast_node_t *node);

err_code_t convert_var_declaration(context_t *context, ast_node_t *node);

err_code_t convert_assignment(context_t *context, ast_node_t *node);

err_code_t convert_if(context_t *context, ast_node_t *node);

err_code_t convert_while_do(context_t *context, ast_node_t *node);

err_code_t convert_for_do(context_t *context, ast_node_t *node);

err_code_t convert_return(context_t *context, ast_node_t *node);

err_code_t convert_match(context_t *context, ast_node_t *node);

err_code_t convert_match_case(context_t *context, ast_node_t *node);

err_code_t convert_binary_op(context_t *context, ast_node_t *node);

err_code_t convert_unary_op(context_t *context, ast_node_t *node);

/****************************\
 * Interface implementation *
\****************************/

err_code_t convert_node(context_t *context, ast_node_t *node) {

  err_code_t err = NO_ERR;

  switch (node->type) {
  case AST_FUNCTION_CALL:
    break;
  case AST_DOT_FUNCTION_CALL:
    break;
  case AST_LAMBDA_DECLARATION:
    break;
  case AST_EXTENSION_DECLARATION:
    break;
  case AST_BLOCK:
    break;
  case AST_VARIABLE_DECLARATION:
    break;
  case AST_ASSIGNMENT:
    break;
  case AST_IF:
    break;
  case AST_WHILE_DO:
    break;
  case AST_FOR_DO:
    break;
  case AST_RETURN:
    break;
  case AST_MATCH:
    break;
  case AST_MATCH_CASE:
    break;
  case AST_BINARY_OPERATION:
    break;
  case AST_UNARY_OPERATION:
    break;
  case AST_LIST_CONSTRUCTOR:
    break;
  default:
    err = INTERNAL_ERR;
    break;
  }

  return err;
}
