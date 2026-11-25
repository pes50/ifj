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

static bool is_current_function_unit(context_t *context) {
  return context != NULL && context->current_return_type != NULL &&
         context->current_return_type->type == DATA_TYPE_UNIT;
}

err_code_t convert_return(context_t *context, ast_node_t *node) {
  inner_symbol_t empty_sym = create_empty_symbol();
  bool push_nil = (node->data.return_statement.expression == NULL);

  if (node->data.return_statement.expression != NULL) {
    err_code_t err = convert_expression_on_stack(
        context, node->data.return_statement.expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (is_current_function_unit(context)) {
      err = discard_stack_top(context);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      push_nil = true;
    }
  }

  if (push_nil) {
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

static err_code_t init_for_iterator(context_t *context, ast_node_t *iterator,
                                    inner_symbol_t *out_iter_var) {
  if (context == NULL || iterator == NULL || iterator->type != AST_PARAMETER ||
      iterator->data.parameter.name == NULL || out_iter_var == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *unique_name =
      create_unique_variable(context, iterator->data.parameter.name);
  if (unique_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  free(iterator->data.parameter.name);
  iterator->data.parameter.name = unique_name;
  *out_iter_var = create_var_symbol(unique_name);

  inner_symbol_t empty_sym = create_empty_symbol();
  RETURN_ERR(insert_at_function_start(context, INNER_DEFVAR, empty_sym,
                                      *out_iter_var, empty_sym));
}

static err_code_t convert_for_guards(context_t *context, ast_node_t *guards,
                                     inner_symbol_t continue_label) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_value_t val = {0};
  val.inner_bool = true;
  inner_symbol_t true_sym = create_const_symbol(INNER_BOOL, val);

  for (ast_node_t *guard = guards; guard != NULL; guard = guard->next) {
    err_code_t err = convert_expression_on_stack(context, guard);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err =
        append_statement(context, INNER_PUSHS, empty_sym, true_sym, empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err = append_statement(context, INNER_JUMPIFNEQS, continue_label, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  RETURN_ERR(NO_ERR);
}

static err_code_t convert_for_list(context_t *context, ast_node_t *node,
                                   inner_symbol_t iterator_var) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t list_var, size_var, idx_var;

  if (create_helper_variable(context, &list_var) != NO_ERR ||
      create_helper_variable(context, &size_var) != NO_ERR ||
      create_helper_variable(context, &idx_var) != NO_ERR) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_value_t val = {0};
  val.inner_int = 1;
  inner_symbol_t sym_1 = create_const_symbol(INNER_INT, val);
  val.inner_int = 2;
  inner_symbol_t sym_2 = create_const_symbol(INNER_INT, val);

  err_code_t err =
      convert_expression_on_stack(context, node->data.for_do.generator);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_POPS, list_var, empty_sym,
                              empty_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, size_var, list_var,
                              sym_1)) != NO_ERR ||
      (err = append_statement(context, INNER_MOVE, idx_var, sym_2,
                              empty_sym)) != NO_ERR) {
    RETURN_ERR(err);
  }

  char *start_label_name = create_unique_label(context, "for_list_start");
  char *continue_label_name = create_unique_label(context, "for_list_cont");
  char *end_label_name = create_unique_label(context, "for_list_end");
  if (start_label_name == NULL || continue_label_name == NULL ||
      end_label_name == NULL) {
    free(start_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t start_label = create_label_symbol(start_label_name);
  inner_symbol_t start_label_owned = start_label;
  start_label_owned.owns_identifier = true;

  inner_symbol_t cont_label = create_label_symbol(continue_label_name);
  inner_symbol_t cont_label_owned = cont_label;
  cont_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // LABEL for_list_start
  if ((err = append_statement(context, INNER_LABEL, empty_sym,
                              start_label_owned, empty_sym)) != NO_ERR) {
    free(start_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_JUMPIFEQ, end_label, idx_var,
                              size_var)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, iterator_var, list_var,
                              idx_var)) != NO_ERR ||
      (err = convert_for_guards(context, node->data.for_do.guards,
                                cont_label)) != NO_ERR ||
      (err = convert_statement_list(context, node->data.for_do.body, false)) !=
          NO_ERR) {
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_list_continue
  if ((err = append_statement(context, INNER_LABEL, empty_sym, cont_label_owned,
                              empty_sym)) != NO_ERR) {
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_ADD, idx_var, idx_var, sym_1)) !=
          NO_ERR ||
      (err = append_statement(context, INNER_JUMP, start_label, empty_sym,
                              empty_sym)) != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_list_end
  if ((err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                              empty_sym)) != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  RETURN_ERR(NO_ERR);
}

static err_code_t convert_for_range(context_t *context, ast_node_t *node,
                                    inner_symbol_t iterator_var) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t range_var, end_var, step_var, incl_var, cond_var;

  if (create_helper_variable(context, &range_var) != NO_ERR ||
      create_helper_variable(context, &end_var) != NO_ERR ||
      create_helper_variable(context, &step_var) != NO_ERR ||
      create_helper_variable(context, &incl_var) != NO_ERR ||
      create_helper_variable(context, &cond_var) != NO_ERR) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_value_t val = {0};
  val.inner_int = 0;
  inner_symbol_t sym_0 = create_const_symbol(INNER_INT, val);
  val.inner_int = 1;
  inner_symbol_t sym_1 = create_const_symbol(INNER_INT, val);
  val.inner_int = 2;
  inner_symbol_t sym_2 = create_const_symbol(INNER_INT, val);
  val.inner_int = 3;
  inner_symbol_t sym_3 = create_const_symbol(INNER_INT, val);

  val.inner_bool = true;
  inner_symbol_t true_sym = create_const_symbol(INNER_BOOL, val);
  val.inner_bool = false;
  inner_symbol_t false_sym = create_const_symbol(INNER_BOOL, val);

  err_code_t err =
      convert_expression_on_stack(context, node->data.for_do.generator);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_POPS, range_var, empty_sym,
                              empty_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, iterator_var, range_var,
                              sym_0)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, end_var, range_var,
                              sym_1)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, step_var, range_var,
                              sym_2)) != NO_ERR ||
      (err = append_statement(context, INNER_ARRAYGET, incl_var, range_var,
                              sym_3)) != NO_ERR) {
    RETURN_ERR(err);
  }

  char *start_label_name = create_unique_label(context, "for_range_start");
  char *neg_label_name = create_unique_label(context, "for_neg_step");
  char *body_label_name = create_unique_label(context, "for_range_body");
  char *continue_label_name =
      create_unique_label(context, "for_range_continue");
  char *end_label_name = create_unique_label(context, "for_range_end");
  if (start_label_name == NULL || neg_label_name == NULL ||
      body_label_name == NULL || continue_label_name == NULL ||
      end_label_name == NULL) {
    free(start_label_name);
    free(neg_label_name);
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t start_label = create_label_symbol(start_label_name);
  inner_symbol_t start_label_owned = start_label;
  start_label_owned.owns_identifier = true;

  inner_symbol_t neg_label = create_label_symbol(neg_label_name);
  inner_symbol_t neg_label_owned = neg_label;
  neg_label_owned.owns_identifier = true;

  inner_symbol_t body_label = create_label_symbol(body_label_name);
  inner_symbol_t body_label_owned = body_label;
  body_label_owned.owns_identifier = true;

  inner_symbol_t cont_label = create_label_symbol(continue_label_name);
  inner_symbol_t cont_label_owned = cont_label;
  cont_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // LABEL for_range_start
  if ((err = append_statement(context, INNER_LABEL, empty_sym,
                              start_label_owned, empty_sym)) != NO_ERR) {
    free(start_label_name);
    free(neg_label_name);
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // step > 0
  if ((err = append_statement(context, INNER_LT, cond_var, step_var, sym_0)) !=
          NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFEQ, neg_label, cond_var,
                              true_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_GT, cond_var, iterator_var,
                              end_var)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFEQ, end_label, cond_var,
                              true_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFNEQ, body_label,
                              iterator_var, end_var)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFEQ, end_label, incl_var,
                              false_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMP, body_label, empty_sym,
                              empty_sym)) != NO_ERR) {
    free(neg_label_name);
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_neg_step (step < 0)
  if ((err = append_statement(context, INNER_LABEL, empty_sym, neg_label_owned,
                              empty_sym)) != NO_ERR) {
    free(neg_label_name);
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_LT, cond_var, iterator_var,
                              end_var)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFEQ, end_label, cond_var,
                              true_sym)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFNEQ, body_label,
                              iterator_var, end_var)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMPIFEQ, end_label, incl_var,
                              false_sym)) != NO_ERR) {
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_range_body
  if ((err = append_statement(context, INNER_LABEL, empty_sym, body_label_owned,
                              empty_sym)) != NO_ERR) {
    free(body_label_name);
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if ((err = convert_for_guards(context, node->data.for_do.guards,
                                cont_label)) != NO_ERR ||
      (err = convert_statement_list(context, node->data.for_do.body, false)) !=
          NO_ERR) {
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_range_continue
  if ((err = append_statement(context, INNER_LABEL, empty_sym, cont_label_owned,
                              empty_sym)) != NO_ERR) {
    free(continue_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if ((err = append_statement(context, INNER_ADD, iterator_var, iterator_var,
                              step_var)) != NO_ERR ||
      (err = append_statement(context, INNER_JUMP, start_label, empty_sym,
                              empty_sym)) != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  // LABEL for_range_end
  if ((err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                              empty_sym)) != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_for_do(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_FOR_DO ||
      node->data.for_do.generator == NULL || node->data.for_do.body == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t iterator_var;
  err_code_t err =
      init_for_iterator(context, node->data.for_do.iterator, &iterator_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (node->data.for_do.generator->data_type != NULL &&
      node->data.for_do.generator->data_type->type == DATA_TYPE_LIST) {
    RETURN_ERR(convert_for_list(context, node, iterator_var));
  }

  RETURN_ERR(convert_for_range(context, node, iterator_var));
}