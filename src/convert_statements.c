/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_statements.c
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of AST to 3AC conversion of statements
 *
 \*****************************************************************************/

#include "convert_statements.h"
#include "convert_expression.h"
#include "convert_node.h"
#include "convert_utils.h"
#include <stdlib.h>

#pragma region Helpers

static bool is_expression_node(ast_node_t *node) {
  if (node == NULL) {
    return false;
  }

  switch (node->type) {
  case AST_FUNCTION_CALL:
  case AST_DOT_FUNCTION_CALL:
  case AST_BINARY_OPERATION:
  case AST_UNARY_OPERATION:
  case AST_LITERAL:
  case AST_IDENTIFIER:
  case AST_INTERPOLATED_STRING:
  case AST_LIST_CONSTRUCTOR:
  case AST_LAMBDA_DECLARATION:
  case AST_MATCH:
  case AST_IF:
    return true;
  default:
    return false;
  }
}

err_code_t discard_stack_top(context_t *context) {
  inner_symbol_t trash_var = create_var_symbol(TRASH);
  inner_symbol_t empty_sym = create_empty_symbol();

  RETURN_ERR(
      append_statement(context, INNER_POPS, trash_var, empty_sym, empty_sym));
}

#pragma endregion

err_code_t convert_statement_list(context_t *context, ast_node_t *node,
                                  bool keep_last) {
  if (context == NULL || node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  while (node != NULL) {
    bool keep_current = (node->next == NULL) && keep_last;
    err_code_t err = NO_ERR;

    if (node->type == AST_BLOCK) {
      err = convert_statement_list(context, node->data.block.statement,
                                   keep_current);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    } else if (node->type == AST_IF) {
      err = convert_if(context, node, keep_current);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    } else if (is_expression_node(node)) {
      err = convert_expression_on_stack(context, node);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      if (!keep_current) {
        err = discard_stack_top(context);
        if (err != NO_ERR) {
          RETURN_ERR(err);
        }
      }
    } else {
      err = convert_node(context, node);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      if (keep_current && node->type != AST_RETURN) {
        inner_value_t nil_val = {.inner_nil = NULL};
        inner_symbol_t nil_sym = create_const_symbol(INNER_NIL, nil_val);
        err = append_statement(context, INNER_PUSHS, empty_sym, nil_sym,
                               empty_sym);
        if (err != NO_ERR) {
          RETURN_ERR(err);
        }
      }
    }

    if (context->tail != NULL && context->tail->operation == INNER_RETURN) {
      break;
    }

    node = node->next;
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_while_do(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_WHILE_DO) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *condition = node->data.while_do.condition;
  ast_node_t *body = node->data.while_do.body;
  if (condition == NULL || body == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  char *start_label_name = create_unique_label(context, "while_start");
  char *end_label_name = create_unique_label(context, "while_end");
  if (start_label_name == NULL || end_label_name == NULL) {
    free(start_label_name);
    free(end_label_name);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t start_label = create_label_symbol(start_label_name);
  inner_symbol_t start_label_owned = start_label;
  start_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  inner_value_t true_val = {.inner_bool = true};
  inner_symbol_t true_sym = create_const_symbol(INNER_BOOL, true_val);

  // label while_start
  err_code_t err = append_statement(context, INNER_LABEL, empty_sym,
                                    start_label_owned, empty_sym);
  if (err != NO_ERR) {
    free(start_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // condition
  err = convert_expression_on_stack(context, condition);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // push true
  err = append_statement(context, INNER_PUSHS, empty_sym, true_sym, empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // jump to while_end
  err = append_statement(context, INNER_JUMPIFNEQS, end_label, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // loop body
  err = convert_node(context, body);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // back to while_start
  err =
      append_statement(context, INNER_JUMP, start_label, empty_sym, empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // while_end
  err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_block(context_t *context, ast_node_t *node) {
  RETURN_ERR(
      convert_statement_list(context, node->data.block.statement, false));
}

err_code_t convert_var_declaration(context_t *context, ast_node_t *node) {
  inner_symbol_t empty_sym = create_empty_symbol();

  char *unique_name =
      create_unique_variable(context, node->data.variable_declaration.name);
  if (unique_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  node->data.variable_declaration.unique_id = unique_name;

  // Insert DEFVAR at the start of the function body
  err_code_t err =
      insert_at_function_start(context, INNER_DEFVAR, empty_sym,
                               create_var_symbol(unique_name), empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (node->data.variable_declaration.expression != NULL) {
    err = convert_expression_on_stack(
        context, node->data.variable_declaration.expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    RETURN_ERR(append_statement(context, INNER_POPS,
                                create_var_symbol(unique_name), empty_sym,
                                empty_sym));
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_assignment(context_t *context, ast_node_t *node) {
  inner_symbol_t empty_sym = create_empty_symbol();

  char *var_name = NULL;
  if (node->data.assignment.calling->type == AST_PARAMETER) {
    var_name = node->data.assignment.calling->data.parameter.name;
  } else {
    var_name =
        node->data.assignment.calling->data.variable_declaration.unique_id;
  }
  if (var_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err =
      convert_expression_on_stack(context, node->data.assignment.expression);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(append_statement(context, INNER_POPS, create_var_symbol(var_name),
                              empty_sym, empty_sym));
}

err_code_t convert_return(context_t *context, ast_node_t *node) {
  inner_symbol_t empty_sym = create_empty_symbol();

  // Push return value onto the data stack (nil@nil if bare return / Unit)
  if (node->data.return_statement.expression != NULL) {
    err_code_t err = convert_expression_on_stack(
        context, node->data.return_statement.expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  } else {
    inner_value_t nil_val = {.inner_nil = NULL};
    inner_symbol_t nil_sym = create_const_symbol(INNER_NIL, nil_val);
    err_code_t err =
        append_statement(context, INNER_PUSHS, empty_sym, nil_sym, empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // Callee pops its own local frame before returning
  err_code_t err = append_statement(context, INNER_POPFRAME, empty_sym,
                                    empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(
      append_statement(context, INNER_RETURN, empty_sym, empty_sym, empty_sym));
}