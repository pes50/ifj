/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_expression.c
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of AST to 3AC conversion of expressions
 *
 \*****************************************************************************/

#include "convert_expression.h"
#include "builtin.h"
#include "convert_list.h"
#include "convert_match.h"
#include "convert_node.h"
#include "convert_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static err_code_t convert_interpolated_string_on_stack(context_t *context,
                                                       ast_node_t *node) {
  if (context == NULL || node == NULL ||
      node->type != AST_INTERPOLATED_STRING) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t acc_var;
  inner_symbol_t part_var;

  // create helper variables for the accumulated string and current segment
  err_code_t err = create_helper_variable(context, &acc_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = create_helper_variable(context, &part_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // initialize accumulator to empty string
  inner_value_t empty_str_val = {.inner_string = ""};
  inner_symbol_t empty_str_sym =
      create_const_symbol(INNER_STRING, empty_str_val);
  err =
      append_statement(context, INNER_MOVE, acc_var, empty_str_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // "include" to str
  enqueue_function(context, &to_str_function_declaration);

  // iterate through all segments in the interpolated string
  ast_node_t *expression = node->data.interpolated_string.expressions;
  while (expression != NULL) {
    // Evaluate segment expression -> pushes result onto the data stack
    err = convert_expression_on_stack(context, expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // use toStr if the expression type is not string
    if (!are_data_types_identical(&node_string, expression->data_type)) {
      err = append_statement(
          context, INNER_CALL, empty_sym,
          create_label_symbol(
              to_str_function_declaration.data.function_declaration.name),
          empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    }

    // pop the resulting string into part_var and concatenate with acc_var
    err = append_statement(context, INNER_POPS, part_var, empty_sym, empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = append_statement(context, INNER_CONCAT, acc_var, acc_var, part_var);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    expression = expression->next;
  }

  // push final concatenated string
  RETURN_ERR(
      append_statement(context, INNER_PUSHS, empty_sym, acc_var, empty_sym));
}

err_code_t convert_function_call(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_FUNCTION_CALL ||
      node->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *calling = node->data.function_call.calling;
  if (calling == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // Function is used as identifier
  if (calling->type == AST_FUNCTION_DECLARATION &&
      node->data.function_call.argument_lists == NULL &&
      node->data_type->type == DATA_TYPE_LAMBDA) {
    inner_symbol_t empty_sym = create_empty_symbol();
    inner_value_t val = {
        .inner_string = calling->data.function_declaration.name,
    };
    inner_symbol_t label_sym = create_const_symbol(INNER_STRING, val);
    RETURN_ERR(append_statement(context, INNER_PUSHS, empty_sym, label_sym,
                                empty_sym));
  }

  // Function call is a list constructor
  if (calling->type == AST_LIST_CONSTRUCTOR) {
    RETURN_ERR(convert_list_constructor_inline(context, node));
  }

  // Function call is a list access
  if ((calling->type == AST_VARIABLE_DECLARATION ||
       calling->type == AST_PARAMETER) &&
      calling->data_type != NULL &&
      calling->data_type->type == DATA_TYPE_LIST) {
    RETURN_ERR(convert_list_access(context, node));
  }

  // Handle function call
  inner_symbol_t empty_sym = create_empty_symbol();

  // Push all arguments left-to-right onto the data stack
  ast_node_t *argument_list = node->data.function_call.argument_lists;
  while (argument_list != NULL) {
    if (argument_list->type != AST_ARGUMENT_LIST) {
      RETURN_ERR(INTERNAL_ERR);
    }

    ast_node_t *argument = argument_list->data.argument_list.arguments;
    while (argument != NULL) {
      err_code_t err = convert_expression_on_stack(context, argument);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      argument = argument->next;
    }

    argument_list = argument_list->next;
  }

  // Get calling label
  inner_symbol_t target_sym;
  if (calling->type == AST_VARIABLE_DECLARATION) {
    target_sym =
        create_var_symbol(calling->data.variable_declaration.unique_id);
  } else if (calling->type == AST_PARAMETER) {
    target_sym = create_var_symbol(calling->data.parameter.name);
  } else {
    target_sym = create_label_symbol(node->data.function_call.name);
  }

  RETURN_ERR(
      append_statement(context, INNER_CALL, empty_sym, target_sym, empty_sym));
}

static err_code_t convert_dot_function_call(context_t *context,
                                            ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_DOT_FUNCTION_CALL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *target = node->data.dot_function_call.target;
  if (target == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  // Evaluate target
  err_code_t err = convert_expression_on_stack(context, target);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Attribute access
  if (node->data.dot_function_call.attribute_index > 0) {
    inner_symbol_t instance_variable;
    inner_symbol_t attribute_val_variable;
    err = create_helper_variable(context, &instance_variable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err = create_helper_variable(context, &attribute_val_variable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // Pop the class array reference
    err = append_statement(context, INNER_POPS, instance_variable, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    inner_value_t idx_val = {
        .inner_int = (long long)node->data.dot_function_call.attribute_index,
    };
    inner_symbol_t idx_sym = create_const_symbol(INNER_INT, idx_val);
    // Save the attribute in a variable
    err = append_statement(context, INNER_ARRAYGET, attribute_val_variable,
                           instance_variable, idx_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // Push the attribute to the stack
    RETURN_ERR(append_statement(context, INNER_PUSHS, empty_sym,
                                attribute_val_variable, empty_sym));
  }

  // Function call
  ast_node_t *calling = node->data.dot_function_call.calling;
  if (calling == NULL || calling->type != AST_FUNCTION_DECLARATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err = enqueue_function(context, calling);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  ast_node_t *argument_list = node->data.dot_function_call.argument_lists;
  while (argument_list != NULL) {
    if (argument_list->type != AST_ARGUMENT_LIST) {
      RETURN_ERR(INTERNAL_ERR);
    }

    ast_node_t *argument = argument_list->data.argument_list.arguments;
    while (argument != NULL) {
      err = convert_expression_on_stack(context, argument);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      argument = argument->next;
    }

    argument_list = argument_list->next;
  }

  inner_symbol_t label =
      create_label_symbol(calling->data.function_declaration.name);
  RETURN_ERR(
      append_statement(context, INNER_CALL, empty_sym, label, empty_sym));
}

err_code_t create_operand(context_t *context, ast_node_t *node,
                          inner_symbol_t *operand) {
  (void)context;
  if (node->type == AST_LITERAL) {
    err_code_t err = create_literal_symbol(node, operand);
    RETURN_ERR(err);
  }
  if (node->type == AST_IDENTIFIER) {
    ast_node_t *calling = node->data.identifier.calling;
    if (calling == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    // Function is used as an identifier
    if (calling->type == AST_FUNCTION_DECLARATION) {
      err_code_t err = enqueue_function(context, calling);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      inner_value_t val = {
          .inner_string = calling->data.function_declaration.name,
      };
      *operand = create_const_symbol(INNER_STRING, val);
      RETURN_ERR(NO_ERR);
    }

    char *var_name = NULL;
    if (calling->type == AST_VARIABLE_DECLARATION) {
      var_name = calling->data.variable_declaration.unique_id;
    } else if (calling->type == AST_PARAMETER) {
      var_name = calling->data.parameter.name;
    }
    if (var_name == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    *operand = create_var_symbol(var_name);
    RETURN_ERR(NO_ERR);
  }

  RETURN_ERR(INTERNAL_ERR);
}

err_code_t create_operation(ast_node_t *node,
                            inner_statement_operation_t *operation,
                            bool *out_append_not) {
  *out_append_not = false;

  switch (node->data.binary_operation.op) {
  case TOK_BINARY_PLUS:
    *operation = INNER_ADDS;
    break;
  case TOK_BINARY_MINUS:
    *operation = INNER_SUBS;
    break;
  case TOK_MUL:
    *operation = INNER_MULS;
    break;
  case TOK_DIV: {
    data_type_node_t *left_type = node->data.binary_operation.left->data_type;
    data_type_node_t *right_type = node->data.binary_operation.right->data_type;
    if (left_type == NULL || right_type == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    if (are_data_types_identical(left_type, &node_double) ||
        are_data_types_identical(right_type, &node_double)) {
      *operation = INNER_DIVS;
    } else {
      *operation = INNER_IDIVS;
    }
    break;
  }
  case TOK_LOWER_T:
    *operation = INNER_LTS;
    break;
  case TOK_LOWER_EQ:
    *operation = INNER_GTS;
    *out_append_not = true;
    break;
  case TOK_GREATER_T:
    *operation = INNER_GTS;
    break;
  case TOK_GREATER_EQ:
    *operation = INNER_LTS;
    *out_append_not = true;
    break;
  case TOK_EQUAL:
    *operation = INNER_EQS;
    break;
  case TOK_NOT_EQ:
    *operation = INNER_EQS;
    *out_append_not = true;
    break;
  case TOK_AND:
    *operation = INNER_ANDS;
    break;
  case TOK_OR:
    *operation = INNER_ORS;
    break;
  default:
    RETURN_ERR(INTERNAL_ERR);
  }
  RETURN_ERR(NO_ERR);
}

err_code_t convert_unary_op_expression(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_UNARY_OPERATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  if (node->data.unary_operation.op == TOK_UNARY_MINUS) {
    inner_symbol_t zero_const;
    if (are_data_types_identical(
            &node_int, node->data.unary_operation.expression->data_type)) {
      inner_value_t zero = {.inner_int = 0};
      zero_const = create_const_symbol(INNER_INT, zero);
    } else if (are_data_types_identical(
                   &node_double,
                   node->data.unary_operation.expression->data_type)) {
      inner_value_t zero = {.inner_float = 0.0};
      zero_const = create_const_symbol(INNER_FLOAT, zero);
    } else {
      RETURN_ERR(INTERNAL_ERR);
    }

    err = append_statement(context, INNER_PUSHS, empty_sym, zero_const,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = convert_expression_on_stack(context,
                                      node->data.unary_operation.expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    RETURN_ERR(
        append_statement(context, INNER_SUBS, empty_sym, empty_sym, empty_sym));
  }

  if (node->data.unary_operation.op == TOK_NOT) {
    err = convert_expression_on_stack(context,
                                      node->data.unary_operation.expression);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    RETURN_ERR(
        append_statement(context, INNER_NOTS, empty_sym, empty_sym, empty_sym));
  }

  RETURN_ERR(INTERNAL_ERR);
}

err_code_t convert_string_expression(context_t *context, ast_node_t *node) {
  if (node->data.binary_operation.op != TOK_BINARY_PLUS) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *left_operand = node->data.binary_operation.left;
  ast_node_t *right_operand = node->data.binary_operation.right;
  if (left_operand == NULL || right_operand == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  // evaluate left and right string expressions left-to-right onto the stack
  err_code_t err = convert_expression_on_stack(context, left_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = convert_expression_on_stack(context, right_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // pop operands into helper variables
  inner_symbol_t left_var;
  inner_symbol_t right_var;
  inner_symbol_t dst_var;

  err = create_helper_variable(context, &left_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = create_helper_variable(context, &right_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = create_helper_variable(context, &dst_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_POPS, right_var, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_POPS, left_var, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_CONCAT, dst_var, left_var, right_var);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(
      append_statement(context, INNER_PUSHS, empty_sym, dst_var, empty_sym));
}

static err_code_t convert_logical_op_expression(context_t *context,
                                                ast_node_t *node) {
  token_type_t logical_op = node->data.binary_operation.op;
  if (logical_op != TOK_AND && logical_op != TOK_OR) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *left_operand = node->data.binary_operation.left;
  ast_node_t *right_operand = node->data.binary_operation.right;
  if (left_operand == NULL || right_operand == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *short_label_name = create_unique_label(context, "short_circuit");
  char *end_label_name = create_unique_label(context, "short_circuit_end");
  if (short_label_name == NULL || end_label_name == NULL) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_value_t bool_val = {.inner_bool = logical_op == TOK_OR};
  inner_symbol_t bool_sym = create_const_symbol(INNER_BOOL, bool_val);

  inner_symbol_t short_label = create_label_symbol(short_label_name);
  inner_symbol_t short_label_owned = short_label;
  short_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // Evaluate left operand
  err_code_t err = convert_expression_on_stack(context, left_operand);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // Short-circuit if ({left == false} for &&) or ({left == true} for ||)
  err = append_statement(context, INNER_PUSHS, empty_sym, bool_sym, empty_sym);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_JUMPIFEQS, short_label, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // Evaluate right operand
  err = convert_expression_on_stack(context, right_operand);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_JUMP, end_label, empty_sym, empty_sym);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // Restore the short-circuit value on stack
  err = append_statement(context, INNER_LABEL, empty_sym, short_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(short_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_PUSHS, empty_sym, bool_sym, empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_binary_op_expression(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_BINARY_OPERATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  ast_node_t *left_operand = node->data.binary_operation.left;
  ast_node_t *right_operand = node->data.binary_operation.right;
  if (right_operand == NULL || left_operand == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  token_type_t binary_op = node->data.binary_operation.op;

  // concat strings
  if (binary_op == TOK_BINARY_PLUS &&
      are_data_types_identical(&node_string, left_operand->data_type)) {
    RETURN_ERR(convert_string_expression(context, node));
  }

  // concat lists
  if (binary_op == TOK_CONCAT_LIST) {
    RETURN_ERR(convert_list_concat_expression(context, node));
  }

  // short-circuit logical operators
  if (binary_op == TOK_AND || binary_op == TOK_OR) {
    RETURN_ERR(convert_logical_op_expression(context, node));
  }

  // left child recursion
  err = convert_expression_on_stack(context, left_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = retype_if_needed(context, left_operand, right_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // right child recursion
  err = convert_expression_on_stack(context, right_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = retype_if_needed(context, right_operand, left_operand);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // choose what operation to convert
  inner_statement_operation_t operation = INNER_NOP_CUSTOM;
  bool should_append_not = false;
  if (create_operation(node, &operation, &should_append_not) == INTERNAL_ERR) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err = append_statement(context, operation, empty_sym, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (should_append_not) {
    err =
        append_statement(context, INNER_NOTS, empty_sym, empty_sym, empty_sym);
  }

  RETURN_ERR(err);
}

err_code_t convert_if(context_t *context, ast_node_t *node, bool push_result) {
  if (context == NULL || node == NULL || node->type != AST_IF ||
      node->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *condition = node->data.if_statement.condition;
  ast_node_t *true_branch = node->data.if_statement.true_branch;
  ast_node_t *false_branch = node->data.if_statement.false_branch;

  if (condition == NULL || true_branch == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // condition
  err_code_t err = convert_expression_on_stack(context, condition);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t empty_sym = create_empty_symbol();
  inner_value_t true_val = {.inner_bool = true};
  inner_symbol_t true_sym = create_const_symbol(INNER_BOOL, true_val);

  err = append_statement(context, INNER_PUSHS, empty_sym, true_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // without else
  if (false_branch == NULL && !push_result) {
    // INNER_JUMPIFNEQS if_end
    // {true_branch}
    // if_end
    char *end_label_name = create_unique_label(context, "if_end");
    if (end_label_name == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    inner_symbol_t end_label = create_label_symbol(end_label_name);
    inner_symbol_t end_label_owned = end_label;
    end_label_owned.owns_identifier = true;

    err = append_statement(context, INNER_JUMPIFNEQS, end_label, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }

    err = convert_statement_list(context, true_branch, false);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }

    err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                           empty_sym);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }

    return NO_ERR;
  }

  // if-else statement or if expression
  char *else_label_name = create_unique_label(context, "if_else");
  char *end_label_name = create_unique_label(context, "if_end");
  if (else_label_name == NULL || end_label_name == NULL) {
    free(else_label_name);
    free(end_label_name);
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t else_label = create_label_symbol(else_label_name);
  inner_symbol_t else_label_owned = else_label;
  else_label_owned.owns_identifier = true;

  inner_symbol_t end_label = create_label_symbol(end_label_name);
  inner_symbol_t end_label_owned = end_label;
  end_label_owned.owns_identifier = true;

  // INNER_JUMPIFNEQS if_else
  // {true_branch}
  // [INNER_INT2FLOATS]
  // INNER_JUMP if_end
  // if_else
  // {false_branch} [INNER_INT2FLOATS] | INNER_PUSHS nil
  // if_end
  err = append_statement(context, INNER_JUMPIFNEQS, else_label, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    free(else_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // true branch
  err = convert_statement_list(context, true_branch, push_result);
  if (err != NO_ERR) {
    free(else_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  bool target_is_double =
      are_data_types_identical(&node_double, node->data_type);

  if (push_result && target_is_double &&
      are_data_types_identical(&node_int, true_branch->data_type) &&
      context->tail != NULL && context->tail->operation != INNER_RETURN) {
    err = append_statement(context, INNER_INT2FLOATS, empty_sym, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      free(else_label_name);
      free(end_label_name);
      RETURN_ERR(err);
    }
  }

  err = append_statement(context, INNER_JUMP, end_label, empty_sym, empty_sym);
  if (err != NO_ERR) {
    free(else_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  // false branch
  err = append_statement(context, INNER_LABEL, empty_sym, else_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(else_label_name);
    free(end_label_name);
    RETURN_ERR(err);
  }

  if (false_branch != NULL) {
    err = convert_statement_list(context, false_branch, push_result);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }

    if (push_result && target_is_double &&
        are_data_types_identical(&node_int, false_branch->data_type) &&
        context->tail != NULL && context->tail->operation != INNER_RETURN) {
      err = append_statement(context, INNER_INT2FLOATS, empty_sym, empty_sym,
                             empty_sym);
      if (err != NO_ERR) {
        free(end_label_name);
        RETURN_ERR(err);
      }
    }
  } else if (push_result) {
    inner_value_t nil_val = {.inner_nil = NULL};
    inner_symbol_t nil_sym = create_const_symbol(INNER_NIL, nil_val);
    err = append_statement(context, INNER_PUSHS, empty_sym, nil_sym, empty_sym);
    if (err != NO_ERR) {
      free(end_label_name);
      RETURN_ERR(err);
    }
  }

  // end label
  err = append_statement(context, INNER_LABEL, empty_sym, end_label_owned,
                         empty_sym);
  if (err != NO_ERR) {
    free(end_label_name);
    RETURN_ERR(err);
  }

  return NO_ERR;
}

err_code_t convert_lambda_expression(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_LAMBDA_DECLARATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->data.lambda_declaration.unique_id == NULL) {
    node->data.lambda_declaration.unique_id =
        create_unique_label(context, "lambda");
    if (node->data.lambda_declaration.unique_id == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }
  }

  err_code_t err = enqueue_function(context, node);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_value_t val = {.inner_string = node->data.lambda_declaration.unique_id};
  inner_symbol_t label_sym = create_const_symbol(INNER_STRING, val);
  inner_symbol_t empty_sym = create_empty_symbol();

  RETURN_ERR(
      append_statement(context, INNER_PUSHS, empty_sym, label_sym, empty_sym));
}

err_code_t convert_expression_on_stack(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  switch (node->type) {
  case AST_IF:
    RETURN_ERR(convert_if(context, node, true));
  case AST_INTERPOLATED_STRING:
    RETURN_ERR(convert_interpolated_string_on_stack(context, node));
  case AST_MATCH:
    RETURN_ERR(convert_match(context, node));
  case AST_FUNCTION_CALL: {
    err_code_t err =
        enqueue_function(context, node->data.function_call.calling);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    RETURN_ERR(convert_function_call(context, node));
  }
  case AST_DOT_FUNCTION_CALL:
    RETURN_ERR(convert_dot_function_call(context, node));
  case AST_LAMBDA_DECLARATION:
    RETURN_ERR(convert_lambda_expression(context, node));
  case AST_UNARY_OPERATION:
    RETURN_ERR(convert_unary_op_expression(context, node));
  case AST_BINARY_OPERATION:
    RETURN_ERR(convert_binary_op_expression(context, node));
  default: {
    inner_symbol_t empty_sym = create_empty_symbol();
    inner_symbol_t operand = create_empty_symbol();
    err_code_t err = create_operand(context, node, &operand);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    RETURN_ERR(
        append_statement(context, INNER_PUSHS, empty_sym, operand, empty_sym));
  }
  }
}