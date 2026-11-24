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
#include "ast_node.h"
#include "builtin.h"
#include "convert_expression.h"
#include "convert_match.h"
#include "convert_statements.h"
#include "convert_utils.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

err_code_t append_statement(context_t *context, inner_statement_operation_t op,
                            inner_symbol_t dest, inner_symbol_t src1,
                            inner_symbol_t src2) {
  inner_statement_t *statement = create_statement(op, dest, src1, src2);
  if (statement == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (context->head == NULL) {
    context->head = context->tail = statement;
  } else {
    context->tail->next_statement = statement;
    context->tail = statement;
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_node(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;

  switch (node->type) {
  case AST_FUNCTION_CALL:
    err = enqueue_function(context, node->data.function_call.calling);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    RETURN_ERR(convert_function_call(context, node));
  case AST_DOT_FUNCTION_CALL:
    break;
  case AST_LAMBDA_DECLARATION:
    break;
  case AST_EXTENSION_DECLARATION:
    break;
  case AST_BLOCK:
    err = convert_block(context, node);
    break;
  case AST_VARIABLE_DECLARATION:
    err = convert_var_declaration(context, node);
    break;
  case AST_ASSIGNMENT:
    err = convert_assignment(context, node);
    break;
  case AST_IF:
    err = convert_if(context, node, false);
    break;
  case AST_WHILE_DO:
    err = convert_while_do(context, node);
    break;
  case AST_FOR_DO:
    break;
  case AST_RETURN:
    err = convert_return(context, node);
    break;
  case AST_MATCH:
    err = convert_match(context, node);
    if (err == NO_ERR) {
      err = discard_stack_top(context);
    }
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

  RETURN_ERR(err);
}