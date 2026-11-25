/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_list.c
 *
 * @author Roman Skoupý     (xskoupr00)
 *
 * @brief Implementation of AST to 3AC conversion of lists
 *
\*****************************************************************************/

#include "convert_list.h"
#include "convert_expression.h"
#include "convert_node.h"
#include "convert_utils.h"
#include <stdio.h>
#include <string.h>

err_code_t convert_list_constructor_inline(context_t *context,
                                           ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_FUNCTION_CALL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *argument_list = node->data.function_call.argument_lists;
  if (argument_list == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *argument = NULL;
  if (argument_list->type != AST_ARGUMENT_LIST || argument_list->next != NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  argument = argument_list->data.argument_list.arguments;

  // Count arguments
  size_t count = 0;
  ast_node_t *current = argument;
  while (current != NULL) {
    count++;
    current = current->next;
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t zero_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 0});
  inner_symbol_t one_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 1});
  inner_symbol_t total_size_sym = create_const_symbol(
      INNER_INT, (inner_value_t){.inner_int = (long long)(count + 2)});

  inner_symbol_t list_var;
  err_code_t err = create_helper_variable(context, &list_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t temp_val = create_empty_symbol();
  if (count > 0) {
    err = create_helper_variable(context, &temp_val);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // NEWARRAY list_var int@{count + 2}
  err = append_statement(context, INNER_NEWARRAY, list_var, total_size_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // ARRAYSET list_var int@0 int@0
  err = append_statement(context, INNER_ARRAYSET, list_var, zero_sym, zero_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // ARRAYSET list_var int@1 int@{count + 2}
  err = append_statement(context, INNER_ARRAYSET, list_var, one_sym,
                         total_size_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  size_t idx = 2;
  current = argument;
  // Insert arguments into the list
  while (current != NULL) {
    // Evaluate argument
    err = convert_expression_on_stack(context, current);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // Move argument to temporary variable
    err = append_statement(context, INNER_POPS, temp_val, empty_sym, empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // Move argument to list
    inner_symbol_t idx_sym = create_const_symbol(
        INNER_INT, (inner_value_t){.inner_int = (long long)idx});
    err =
        append_statement(context, INNER_ARRAYSET, list_var, idx_sym, temp_val);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    current = current->next;
    idx++;
  }

  // Push list
  RETURN_ERR(
      append_statement(context, INNER_PUSHS, empty_sym, list_var, empty_sym));
}

err_code_t convert_list_access(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_FUNCTION_CALL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *calling = node->data.function_call.calling;
  ast_node_t *argument_list = node->data.function_call.argument_lists;
  if (calling == NULL || argument_list == NULL ||
      argument_list->type != AST_ARGUMENT_LIST || argument_list->next != NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *argument = argument_list->data.argument_list.arguments;
  if (argument == NULL || argument->next != NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t zero_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 0});
  inner_symbol_t one_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 1});
  inner_symbol_t two_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 2});
  inner_symbol_t true_sym =
      create_const_symbol(INNER_BOOL, (inner_value_t){.inner_bool = true});

  inner_symbol_t target_helper;
  inner_symbol_t idx_var;
  inner_symbol_t total_size_var;
  inner_symbol_t condition_var;
  err_code_t err;
  if ((err = create_helper_variable(context, &target_helper)) != NO_ERR ||
      (err = create_helper_variable(context, &idx_var)) != NO_ERR ||
      (err = create_helper_variable(context, &total_size_var)) != NO_ERR ||
      (err = create_helper_variable(context, &condition_var)) != NO_ERR) {
    RETURN_ERR(err);
  }

  // Evaluate target expression
  if (calling->type == AST_VARIABLE_DECLARATION) {
    err = append_statement(
        context, INNER_MOVE, target_helper,
        create_var_symbol(calling->data.variable_declaration.unique_id),
        empty_sym);
  } else if (calling->type == AST_PARAMETER) {
    err = append_statement(context, INNER_MOVE, target_helper,
                           create_var_symbol(calling->data.parameter.name),
                           empty_sym);
  } else {
    err = convert_expression_on_stack(context, calling);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err = append_statement(context, INNER_POPS, target_helper, empty_sym,
                           empty_sym);
  }
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Evaluate index expression
  err = convert_expression_on_stack(
      context, argument_list->data.argument_list.arguments);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Pop to variable
  err = append_statement(context, INNER_POPS, idx_var, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Get list size
  err = append_statement(context, INNER_ARRAYGET, total_size_var, target_helper,
                         one_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  char *err_label_str = create_unique_label(context, "list_out_of_bounds");
  char *ok_label_str = create_unique_label(context, "list_ok");
  if (err_label_str == NULL || ok_label_str == NULL) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t err_label = create_label_symbol(err_label_str);
  inner_symbol_t err_label_owned = err_label;
  err_label_owned.owns_identifier = true;

  inner_symbol_t ok_label = create_label_symbol(ok_label_str);
  inner_symbol_t ok_label_owned = ok_label;
  ok_label_owned.owns_identifier = true;

  // Check idx < 0
  err = append_statement(context, INNER_LT, condition_var, idx_var, zero_sym);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_JUMPIFEQ, err_label, condition_var,
                         true_sym);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }

  // Check {idx + 2} < total_size
  err = append_statement(context, INNER_ADD, idx_var, idx_var, two_sym);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_LT, condition_var, idx_var,
                         total_size_var);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_JUMPIFEQ, ok_label, condition_var,
                         true_sym);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_LABEL, empty_sym, err_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(err_label_str);
    free(ok_label_str);
    RETURN_ERR(err);
  }

  inner_symbol_t exit_code_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = RUNTIME_ERR});

  err = append_statement(context, INNER_EXIT, empty_sym, exit_code_sym,
                         empty_sym);
  if (err != NO_ERR) {
    free(ok_label_str);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_LABEL, empty_sym, ok_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(ok_label_str);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_ARRAYGET, target_helper, target_helper,
                         idx_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(append_statement(context, INNER_PUSHS, empty_sym, target_helper,
                              empty_sym));
}

static err_code_t
create_list_copy_loop(context_t *context, inner_symbol_t src_list,
                      inner_symbol_t src_size, inner_symbol_t dst_list,
                      inner_symbol_t dst_idx, bool int_to_float) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t one_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 1});
  inner_symbol_t two_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 2});
  inner_symbol_t true_sym =
      create_const_symbol(INNER_BOOL, (inner_value_t){.inner_bool = true});

  inner_symbol_t src_idx;
  inner_symbol_t condition_var;
  inner_symbol_t element_var;
  err_code_t err = create_helper_variable(context, &src_idx);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = create_helper_variable(context, &condition_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = create_helper_variable(context, &element_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  char *loop_label_str = create_unique_label(context, "list_concat_loop");
  char *end_label_str = create_unique_label(context, "list_concat_end");
  if (loop_label_str == NULL || end_label_str == NULL) {
    free(loop_label_str);
    free(end_label_str);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t loop_label = create_label_symbol(loop_label_str);
  inner_symbol_t loop_label_owned = loop_label;
  loop_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_str);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // src_idx = 2
  err = append_statement(context, INNER_MOVE, src_idx, two_sym, empty_sym);
  if (err != NO_ERR) {
    free(loop_label_str);
    free(end_label_str);
    RETURN_ERR(err);
  }

  // LABEL loop
  err = append_statement(context, INNER_LABEL, empty_sym, loop_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(loop_label_str);
    free(end_label_str);
    RETURN_ERR(err);
  }

  // if (!(src_idx < src_size)) jump end
  err = append_statement(context, INNER_LT, condition_var, src_idx, src_size);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_JUMPIFNEQ, end_label, condition_var,
                         true_sym);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  // element = src_list[src_idx]
  err =
      append_statement(context, INNER_ARRAYGET, element_var, src_list, src_idx);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  if (int_to_float) {
    err = append_statement(context, INNER_INT2FLOAT, element_var, element_var,
                           empty_sym);
    if (err != NO_ERR) {
      free(end_label_str);
      RETURN_ERR(err);
    }
  }

  // dst_list[dst_idx] = element
  err =
      append_statement(context, INNER_ARRAYSET, dst_list, dst_idx, element_var);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  // src_idx++
  err = append_statement(context, INNER_ADD, src_idx, src_idx, one_sym);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  // dst_idx++
  err = append_statement(context, INNER_ADD, dst_idx, dst_idx, one_sym);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_JUMP, loop_label, empty_sym, empty_sym);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_str);
    RETURN_ERR(err);
  }

  return NO_ERR;
}

err_code_t convert_list_concat_expression(context_t *context,
                                          ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_BINARY_OPERATION ||
      node->data.binary_operation.op != TOK_CONCAT_LIST ||
      node->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *left_operand = node->data.binary_operation.left;
  ast_node_t *right_operand = node->data.binary_operation.right;
  if (left_operand == NULL || right_operand == NULL ||
      left_operand->data_type == NULL || right_operand->data_type == NULL ||
      left_operand->data_type->type != DATA_TYPE_LIST ||
      right_operand->data_type->type != DATA_TYPE_LIST) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // Evaluate operands
  err_code_t err = convert_expression_on_stack(context, left_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = convert_expression_on_stack(context, right_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t zero_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 0});
  inner_symbol_t one_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 1});
  inner_symbol_t two_sym =
      create_const_symbol(INNER_INT, (inner_value_t){.inner_int = 2});

  inner_symbol_t left_list;
  inner_symbol_t right_list;
  inner_symbol_t dst_list;
  inner_symbol_t left_size;
  inner_symbol_t right_size;
  inner_symbol_t total_size;
  inner_symbol_t dst_idx;

  if ((err = create_helper_variable(context, &left_list)) != NO_ERR ||
      (err = create_helper_variable(context, &right_list)) != NO_ERR ||
      (err = create_helper_variable(context, &dst_list)) != NO_ERR ||
      (err = create_helper_variable(context, &left_size)) != NO_ERR ||
      (err = create_helper_variable(context, &right_size)) != NO_ERR ||
      (err = create_helper_variable(context, &total_size)) != NO_ERR ||
      (err = create_helper_variable(context, &dst_idx)) != NO_ERR) {
    RETURN_ERR(err);
  }

  // Pop operands
  err = append_statement(context, INNER_POPS, right_list, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_POPS, left_list, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Read total array sizes
  err =
      append_statement(context, INNER_ARRAYGET, left_size, left_list, one_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_ARRAYGET, right_size, right_list,
                         one_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // total_size = left_size + right_size - 2
  err = append_statement(context, INNER_ADD, total_size, left_size, right_size);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_SUB, total_size, total_size, two_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Initialize dst_list
  err = append_statement(context, INNER_NEWARRAY, dst_list, total_size,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = append_statement(context, INNER_ARRAYSET, dst_list, zero_sym, zero_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err =
      append_statement(context, INNER_ARRAYSET, dst_list, one_sym, total_size);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Check for cast
  bool cast_left = false;
  bool cast_right = false;
  if (are_data_types_identical(&node_double,
                               node->data_type->data.list_type.elements_type)) {
    cast_left = are_data_types_identical(
        &node_int, left_operand->data_type->data.list_type.elements_type);
    cast_right = are_data_types_identical(
        &node_int, right_operand->data_type->data.list_type.elements_type);
  }

  err = append_statement(context, INNER_MOVE, dst_idx, two_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Copy left_list
  err = create_list_copy_loop(context, left_list, left_size, dst_list, dst_idx,
                              cast_left);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Copy right_list
  err = create_list_copy_loop(context, right_list, right_size, dst_list,
                              dst_idx, cast_right);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(
      append_statement(context, INNER_PUSHS, empty_sym, dst_list, empty_sym));
}