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
#include "string.h"
#include "token.h"
#include <stdio.h>

/*********************************\
 * Helper functions declarations *
\*********************************/

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

err_code_t list_ctor(context_t *context, ast_node_t *node);

err_code_t insert_at_function_start(context_t *context,
                                    inner_statement_operation_t op,
                                    inner_symbol_t dest, inner_symbol_t src1,
                                    inner_symbol_t src2) {
  if (context == NULL || context->function_start == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_statement_t *statement = create_statement(op, dest, src1, src2);
  if (statement == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  statement->next_statement = context->function_start->next_statement;
  context->function_start->next_statement = statement;

  // If no body statements were appended yet, tail must move forward too
  if (context->tail == context->function_start) {
    context->tail = statement;
  }

  RETURN_ERR(NO_ERR);
}

/**
 * @brief Removes '_', 'd' and 'D' characters from numeric literals
 *
 * @param raw String form of the literal
 * @param strip_d_suffix Bool to inform that the type is double -> 'd' or 'D'
 *                       should be removed from the end of the literal
 *
 * @return A pointer to a sanitized literal or NULL if failed
 */
static char *clean_numeric_literal(const char *raw, bool strip_d_suffix) {
  if (raw == NULL) {
    return NULL;
  }

  size_t len = strlen(raw);
  char *clean = malloc(len + 1);
  if (clean == NULL) {
    return NULL;
  }

  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    // Skip underscores
    if (raw[i] == '_') {
      continue;
    }

    if (strip_d_suffix && i == len - 1 &&   // Is last
        (raw[i] == 'd' || raw[i] == 'D')) { // Suffix 'd' or 'D'
      break;
    }

    clean[j++] = raw[i];
  }

  // Add terminator
  clean[j] = '\0';

  return clean;
}

static err_code_t create_literal_symbol(ast_node_t *node,
                                        inner_symbol_t *out_symbol) {
  if (node == NULL || node->type != AST_LITERAL || out_symbol == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  data_type_node_t *dtype = node->data.literal.data_type;
  if (dtype == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_value_t val = {0};

  // Convert unit to nil
  if (dtype->type == DATA_TYPE_UNIT) {
    val.inner_nil = NULL;
    *out_symbol = create_const_symbol(INNER_NIL, val);
    RETURN_ERR(NO_ERR);
  }

  if (dtype->type != DATA_TYPE_PRIMITIVE) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *raw = node->data.literal.raw;
  if (raw == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  switch (dtype->data.primitive_type) {
  case PRIMITIVE_INT: {
    char *clean = clean_numeric_literal(raw, false);
    if (clean == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    char *endptr = NULL;
    val.inner_int = strtoll(clean, &endptr, 10);
    bool invalid = clean[0] == '\0' // is empty string
                   || *endptr != '\0';
    free(clean);
    if (invalid) {
      RETURN_ERR(INTERNAL_ERR);
    }

    *out_symbol = create_const_symbol(INNER_INT, val);
    RETURN_ERR(NO_ERR);
  }

  case PRIMITIVE_DOUBLE: {
    char *clean = clean_numeric_literal(raw, true);
    if (clean == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    char *endptr = NULL;
    val.inner_float = strtod(clean, &endptr);
    bool invalid = clean[0] == '\0' // is empty string
                   || *endptr != '\0';
    free(clean);
    if (invalid) {
      RETURN_ERR(INTERNAL_ERR);
    }

    *out_symbol = create_const_symbol(INNER_FLOAT, val);
    RETURN_ERR(NO_ERR);
  }

  case PRIMITIVE_BOOLEAN:
    val.inner_bool = strcmp(raw, "true") == 0;
    *out_symbol = create_const_symbol(INNER_BOOL, val);
    RETURN_ERR(NO_ERR);

  case PRIMITIVE_STRING:
    val.inner_string = raw;
    *out_symbol = create_const_symbol(INNER_STRING, val);
    RETURN_ERR(NO_ERR);
  }

  RETURN_ERR(INTERNAL_ERR);
}

/***********************************\
 * Helper functions implementation *
\***********************************/

/**
 * @brief Converts number to it's string equivalent
 *
 * @param number The number to convert
 *
 * @return Pointer to the allocated string
 */
char *number_to_string(size_t number) {
  int length = snprintf(NULL, 0, "%llu", (unsigned long long)number);
  if (length < 0) {
    return NULL;
  }

  char *result = malloc((size_t)length + 1);
  if (result == NULL) {
    return NULL;
  }

  snprintf(result, (size_t)length + 1, "%llu", (unsigned long long)number);

  return result;
}

// Adds $unique_number prefix to vraibles or helper labels
char *prefix_number_with_dollar(size_t unique_number, char *identifier) {
  if (identifier == NULL) {
    return NULL;
  }

  int len = snprintf(NULL, 0, "LF@$%llu%s", (unsigned long long)unique_number,
                     identifier);
  if (len < 0) {
    return NULL;
  }

  char *result = malloc((size_t)len + 1);
  if (result == NULL) {
    return NULL;
  }

  snprintf(result, (size_t)len + 1, "LF@$%llu%s",
           (unsigned long long)unique_number, identifier);
  return result;
}

char *create_unique_variable(context_t *context, char *identifier) {
  context->unique_var_counter++;
  return prefix_number_with_dollar(context->unique_var_counter - 1, identifier);
}

// To traverse all nodes inside block
err_code_t block_body_list_traversal(context_t *context, ast_node_t *node) {
  while (node != NULL) {
    err_code_t err = convert_node(context, node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    node = node->next;
  }
  RETURN_ERR(NO_ERR);
}

err_code_t create_operand(context_t *context, ast_node_t *node,
                          inner_symbol_t *operand) {
  if (node->type == AST_LITERAL) {
    err_code_t err = create_literal_symbol(node, operand);
    RETURN_ERR(err);
  }
  if (node->type == AST_IDENTIFIER) {
    ast_node_t *calling = node->data.identifier.calling;
    if (calling == NULL) {
      RETURN_ERR(INTERNAL_ERR);
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
                            inner_statement_operation_t *operation) {
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
  case TOK_DIV:
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

    // TODO others

  default:
    RETURN_ERR(INTERNAL_ERR);
  }
  RETURN_ERR(NO_ERR);
}

err_code_t retype_if_needed(context_t *context, ast_node_t *examined_operand,
                            ast_node_t *other_operand) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  if (are_data_types_identical(&node_int, examined_operand->data_type) &&
      are_data_types_identical(&node_double, other_operand->data_type)) {
    err = append_statement(context, INNER_INT2FLOATS, empty_sym, empty_sym,
                           empty_sym);
  }
  RETURN_ERR(err);
}

/**
 * @brief Converts expression using stack instruction and
 *        puts result on top of data stack (in ifjcode26)
 *
 * @param context Context structure
 * @param node Node with the root of the expression subtree
 */
err_code_t convert_expression_on_stack(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol(),
                 operand = create_empty_symbol();

  // if recursion ends in this call
  if (node->type != AST_BINARY_OPERATION && node->type != AST_UNARY_OPERATION) {
    err = create_operand(context, node, &operand);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    RETURN_ERR(
        append_statement(context, INNER_PUSHS, empty_sym, operand, empty_sym));
  }

  // if node is unary operation
  if (node->type == AST_UNARY_OPERATION) {
    if (node->data.unary_operation.op == TOK_UNARY_MINUS) {
      inner_symbol_t zero_const;
      if (are_data_types_identical(
              &node_int, node->data.unary_operation.expression->data_type)) {
        inner_value_t zero = {.inner_int = 0};
        zero_const = create_const_symbol(INNER_INT, zero);
      } else if (are_data_types_identical(
                     &node_double,
                     node->data.unary_operation.expression->data_type)) {
        inner_value_t zero = {.inner_float = 0};
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

      err = append_statement(context, INNER_SUBS, empty_sym, empty_sym,
                             empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    }
    err = create_operand(context, node->data.unary_operation.expression,
                         &operand);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    if (node->data.unary_operation.op == TOK_NOT) {
      RETURN_ERR(append_statement(context, INNER_NOTS, empty_sym, operand,
                                  empty_sym)); // tohle je spravne?
    }

    RETURN_ERR(NO_ERR);
  }

  // if node is binary operation
  ast_node_t *left_operand = node->data.binary_operation.left;
  ast_node_t *right_operand = node->data.binary_operation.right;

  // check invalid pointers
  if (right_operand == NULL || left_operand == NULL) {
    RETURN_ERR(INTERNAL_ERR);
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
  if (create_operation(node, &operation) == INTERNAL_ERR) {
    RETURN_ERR(INTERNAL_ERR);
  }

  RETURN_ERR(
      append_statement(context, operation, empty_sym, empty_sym, empty_sym));
}

// conversion functions
err_code_t convert_function_call(context_t *context, ast_node_t *node) {
  inner_symbol_t empty_sym = create_empty_symbol();
  inner_symbol_t label = create_label_symbol(node->data.function_call.name);

  // Push all arguments left-to-right onto the data stack
  ast_node_t *arg_list = node->data.function_call.argument_lists;
  while (arg_list != NULL) {
    if (arg_list->type != AST_ARGUMENT_LIST) {
      RETURN_ERR(INTERNAL_ERR);
    }

    ast_node_t *arg = arg_list->data.argument_list.arguments;
    while (arg != NULL) {
      err_code_t err = convert_expression_on_stack(context, arg);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      arg = arg->next;
    }

    arg_list = arg_list->next;
  }

  // call
  RETURN_ERR(
      append_statement(context, INNER_CALL, empty_sym, label, empty_sym));
}

err_code_t convert_block(context_t *context, ast_node_t *node) {
  RETURN_ERR(block_body_list_traversal(context, node->data.block.statement));
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

/****************************\
 * Interface implementation *
\****************************/

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

char *alloc_lf_var_name(const char *identifier) {
  if (identifier == NULL) {
    return NULL;
  }

  size_t len = strlen("LF@") + strlen(identifier);
  char *result = malloc(len + 1);
  if (result == NULL) {
    return NULL;
  }

  snprintf(result, len + 1, "LF@%s", identifier);
  return result;
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
    err = convert_function_call(context, node);
    break;
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
    break;
  case AST_WHILE_DO:
    break;
  case AST_FOR_DO:
    break;
  case AST_RETURN:
    err = convert_return(context, node);
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

  RETURN_ERR(err);
}