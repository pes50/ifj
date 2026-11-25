/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_match.c
 *
 * @author Roman Skoupý     (xskoupr00)
 *
 * @brief Implementation of AST to 3AC conversion of match expressions
 *
\*****************************************************************************/

#include "convert_match.h"
#include "ast_node.h"
#include "convert_expression.h"
#include "convert_node.h"
#include "convert_statements.h"
#include "convert_utils.h"
#include "data_type.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>

#pragma region Helpers

static bool is_wildcard_node(ast_node_t *node) {
  if (node == NULL || node->data_type == NULL || node->type != AST_LITERAL) {
    return false;
  }

  return node->data_type->type == DATA_TYPE_ANY;
}

static err_code_t create_runtime_type_check(context_t *context,
                                            inner_symbol_t value_var,
                                            char *expected_type,
                                            inner_symbol_t fail_label) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t type_var;
  err_code_t err = create_helper_variable(context, &type_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Save type to variable
  err = append_statement(context, INNER_TYPE, type_var, value_var, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Compare with expected type
  inner_value_t exp_val = {.inner_string = expected_type};
  inner_symbol_t exp_sym = create_const_symbol(INNER_STRING, exp_val);
  RETURN_ERR(append_statement(context, INNER_JUMPIFNEQ, fail_label, type_var,
                              exp_sym));
}

static err_code_t create_array_id_check(context_t *context,
                                        inner_symbol_t value_var,
                                        size_t expected_id,
                                        inner_symbol_t fail_label) {
  err_code_t err =
      create_runtime_type_check(context, value_var, "array", fail_label);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t id_var;
  err = create_helper_variable(context, &id_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Save id to a variable
  inner_value_t zero_val = {.inner_int = 0};
  inner_symbol_t zero_sym = create_const_symbol(INNER_INT, zero_val);
  err = append_statement(context, INNER_ARRAYGET, id_var, value_var, zero_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Compare with expected id
  inner_value_t id_val = {.inner_int = (long long)expected_id};
  inner_symbol_t id_sym = create_const_symbol(INNER_INT, id_val);
  RETURN_ERR(
      append_statement(context, INNER_JUMPIFNEQ, fail_label, id_var, id_sym));
}

static err_code_t create_class_id_check(context_t *context,
                                        inner_symbol_t value_var,
                                        ast_node_t *class_declaration,
                                        inner_symbol_t fail_label) {
  size_t class_id = 0;
  err_code_t err =
      get_or_assign_class_id(context, class_declaration, &class_id);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(create_array_id_check(context, value_var, class_id, fail_label));
}

static err_code_t create_type_check(context_t *context,
                                    inner_symbol_t value_var,
                                    data_type_node_t *target_type,
                                    inner_symbol_t fail_label) {
  if (target_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  switch (target_type->type) {
  case DATA_TYPE_ANY:
    RETURN_ERR(NO_ERR);
  case DATA_TYPE_PRIMITIVE: {
    char *expected = NULL;
    switch (target_type->data.primitive_type) {
    case PRIMITIVE_INT:
      expected = "int";
      break;
    case PRIMITIVE_DOUBLE:
      expected = "float";
      break;
    case PRIMITIVE_STRING:
      expected = "string";
      break;
    case PRIMITIVE_BOOLEAN:
      expected = "bool";
      break;
    }
    RETURN_ERR(
        create_runtime_type_check(context, value_var, expected, fail_label));
  }
  case DATA_TYPE_UNIT:
    RETURN_ERR(
        create_runtime_type_check(context, value_var, "nil", fail_label));
  case DATA_TYPE_LIST:
    RETURN_ERR(create_array_id_check(context, value_var, 0, fail_label));
  case DATA_TYPE_CLASS:
    RETURN_ERR(create_class_id_check(context, value_var,
                                     target_type->data.class_type.declaration,
                                     fail_label));
  case DATA_TYPE_UNION: {
    char *matched_name = create_unique_label(context, "union_ok");
    if (matched_name == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    inner_symbol_t matched_label = create_label_symbol(matched_name);
    inner_symbol_t matched_owned = matched_label;
    matched_owned.owns_identifier = true;

    // Create check for every type in union
    data_type_node_t *type = target_type->data.union_type.types;
    while (type != NULL) {
      // Last union type
      if (type->next == NULL) {
        err_code_t err =
            create_type_check(context, value_var, type, fail_label);
        if (err != NO_ERR) {
          free(matched_name);
          RETURN_ERR(err);
        }
        break;
      }

      char *next_name = create_unique_label(context, "union_next");
      if (next_name == NULL) {
        free(matched_name);
        RETURN_ERR(INTERNAL_ERR);
      }
      inner_symbol_t next_label = create_label_symbol(next_name);
      inner_symbol_t next_label_owned = next_label;
      next_label_owned.owns_identifier = true;

      err_code_t err = create_type_check(context, value_var, type, next_label);
      if (err != NO_ERR) {
        free(next_name);
        free(matched_name);
        RETURN_ERR(err);
      }

      // Type matched, jump to the end of union check
      err = append_statement(context, INNER_JUMP, matched_label, empty_sym,
                             empty_sym);
      if (err != NO_ERR) {
        free(next_name);
        free(matched_name);
        RETURN_ERR(err);
      }

      // Label for next union type
      err = append_statement(context, INNER_LABEL, empty_sym, next_label_owned,
                             empty_sym);
      if (err != NO_ERR) {
        free(next_name);
        free(matched_name);
        RETURN_ERR(err);
      }

      type = type->next;
    }

    err_code_t err = append_statement(context, INNER_LABEL, empty_sym,
                                      matched_owned, empty_sym);
    if (err != NO_ERR) {
      free(matched_name);
      RETURN_ERR(err);
    }

    return NO_ERR;
  }

  default:
    RETURN_ERR(INTERNAL_ERR);
  }
}

static err_code_t create_literal_check(context_t *context,
                                       inner_symbol_t value_var,
                                       ast_node_t *pattern,
                                       inner_symbol_t fail_label) {
  inner_symbol_t lit_sym = create_empty_symbol();
  err_code_t err = create_literal_symbol(pattern, &lit_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = create_type_check(context, value_var, pattern->data.literal.data_type,
                          fail_label);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(append_statement(context, INNER_JUMPIFNEQ, fail_label, value_var,
                              lit_sym));
}

static err_code_t bind_pattern_variable(context_t *context, ast_node_t *binding,
                                        inner_symbol_t *out_variable) {
  if (context == NULL || binding == NULL || out_variable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *original_name = NULL;
  if (binding->type == AST_PARAMETER) {
    original_name = binding->data.parameter.name;
  } else {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (original_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *unique_name = create_unique_variable(context, original_name);
  if (unique_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  free(binding->data.parameter.name);
  binding->data.parameter.name = unique_name;

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t var_sym = create_var_symbol(unique_name);
  err_code_t err = insert_at_function_start(context, INNER_DEFVAR, empty_sym,
                                            var_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  *out_variable = var_sym;
  RETURN_ERR(NO_ERR);
}

static err_code_t create_pattern_match(context_t *context,
                                       inner_symbol_t matched_variable,
                                       ast_node_t *pattern,
                                       inner_symbol_t fail_label) {
  if (is_wildcard_node(pattern)) {
    RETURN_ERR(NO_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  // Literal
  if (pattern->type == AST_LITERAL) {
    RETURN_ERR(
        create_literal_check(context, matched_variable, pattern, fail_label));
  }

  // Identifier: Type
  if (pattern->type == AST_PARAMETER) {
    // Check type
    err_code_t err =
        create_type_check(context, matched_variable,
                          pattern->data.parameter.data_type, fail_label);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    inner_symbol_t bound_var;
    err = bind_pattern_variable(context, pattern, &bound_var);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err = append_statement(context, INNER_MOVE, bound_var, matched_variable,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    RETURN_ERR(NO_ERR);
  }

  // Class(a, b, ...)
  if (pattern->type == AST_FUNCTION_CALL) {
    // Check type
    err_code_t err =
        create_class_id_check(context, matched_variable,
                              pattern->data.function_call.calling, fail_label);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    long long field_idx = 1;
    ast_node_t *argument_list = pattern->data.function_call.argument_lists;
    if (argument_list == NULL || argument_list->type != AST_ARGUMENT_LIST) {
      RETURN_ERR(INTERNAL_ERR);
    }

    ast_node_t *argument = argument_list->data.argument_list.arguments;
    while (argument != NULL) {
      if (argument->type != AST_PARAMETER) {
        RETURN_ERR(INTERNAL_ERR);
      }

      inner_value_t idx_val = {.inner_int = field_idx};
      inner_symbol_t idx_sym = create_const_symbol(INNER_INT, idx_val);

      inner_symbol_t field_var;
      err = bind_pattern_variable(context, argument, &field_var);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      err = append_statement(context, INNER_ARRAYGET, field_var,
                             matched_variable, idx_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      field_idx++;
      argument = argument->next;
    }

    RETURN_ERR(NO_ERR);
  }

  RETURN_ERR(INTERNAL_ERR);
}

static err_code_t convert_match_case(context_t *context, ast_node_t *node,
                                     ast_node_t *match_node,
                                     inner_symbol_t matched_variable,
                                     inner_symbol_t end_label) {
  if (context == NULL || node == NULL || node->type != AST_MATCH_CASE) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *block = node->data.match_case.block;
  if (block == NULL || block->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  //  _ => ...
  if (is_wildcard_node(node->data.match_case.pattern)) {
    err_code_t err = convert_statement_list(context, block, true);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    RETURN_ERR(retype_if_needed(context, block, match_node));
  }

  char *next_label_name = create_unique_label(context, "match_next");
  if (next_label_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  inner_symbol_t next_label = create_label_symbol(next_label_name);
  inner_symbol_t next_label_owned = next_label;
  next_label_owned.owns_identifier = true;

  // Match pattern and bind any pattern variables
  err_code_t err = create_pattern_match(
      context, matched_variable, node->data.match_case.pattern, next_label);
  if (err != NO_ERR) {
    free(next_label_name);
    RETURN_ERR(err);
  }

  // Evaluate guard
  if (node->data.match_case.guard != NULL) {
    err = convert_expression_on_stack(context, node->data.match_case.guard);
    if (err != NO_ERR) {
      free(next_label_name);
      RETURN_ERR(err);
    }

    inner_value_t true_val = {.inner_bool = true};
    inner_symbol_t true_sym = create_const_symbol(INNER_BOOL, true_val);
    err =
        append_statement(context, INNER_PUSHS, empty_sym, true_sym, empty_sym);
    if (err != NO_ERR) {
      free(next_label_name);
      RETURN_ERR(err);
    }

    err = append_statement(context, INNER_JUMPIFNEQS, next_label, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      free(next_label_name);
      RETURN_ERR(err);
    }
  }

  // Evaluate branch body
  err = convert_statement_list(context, block, true);
  if (err != NO_ERR) {
    free(next_label_name);
    RETURN_ERR(err);
  }

  // Convert int to double
  err = retype_if_needed(context, block, match_node);
  if (err != NO_ERR) {
    free(next_label_name);
    RETURN_ERR(err);
  }

  // Jump to end
  err = append_statement(context, INNER_JUMP, end_label, empty_sym, empty_sym);
  if (err != NO_ERR) {
    free(next_label_name);
    RETURN_ERR(err);
  }

  // Label of next match case
  err = append_statement(context, INNER_LABEL, empty_sym, next_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(next_label_name);
    RETURN_ERR(err);
  }

  return NO_ERR;
}

#pragma endregion

#pragma region Interface implementation

err_code_t convert_match(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_MATCH ||
      node->data.match.expression == NULL || node->data.match.cases == NULL ||
      node->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  // Evaluate matched expression
  err_code_t err =
      convert_expression_on_stack(context, node->data.match.expression);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t matched_variable;
  err = create_helper_variable(context, &matched_variable);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_POPS, matched_variable, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  char *end_label_name = create_unique_label(context, "match_end");
  if (end_label_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // Convert cases
  ast_node_t *current_case = node->data.match.cases;
  while (current_case != NULL) {
    err = convert_match_case(context, current_case, node, matched_variable,
                             end_label);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }

    // Reached _ => ...
    if (is_wildcard_node(current_case->data.match_case.pattern)) {
      break;
    }

    current_case = current_case->next;
  }

  err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  return NO_ERR;
}

#pragma endregion