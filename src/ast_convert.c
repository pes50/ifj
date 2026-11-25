/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file ast_convert.c
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of AST to 3AC conversion entrypoint
 *
 * @note Gemini Hydrogen was used to translate plain-text IFJcode26 into C inner
 * statement constructor calls in this file and files related to AST to 3AC
 * conversion (convert_*.c)
 *
 \*****************************************************************************/

#include "ast_convert.h"
#include "convert_node.h"
#include "convert_utils.h"
#include <stdio.h>
#include <string.h>

#pragma region Helper functions

#pragma region Pop parameter lists

/**
 * @brief Creates variable identifier with LF@ (local frame) prefix
 *
 * @param identifier Identifier of the variable
 *
 * @return Allocated string with the prefix
 */
static char *alloc_lf_var_name(const char *identifier) {
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

/**
 * @brief Defines LF parameter variables and pops incoming arguments in reverse
 *        order from the data stack (top of stack holds the rightmost argument)
 */
err_code_t pop_parameter_list_reverse(context_t *context, ast_node_t *param) {
  if (param == NULL) {
    RETURN_ERR(NO_ERR);
  }
  if (param->type != AST_PARAMETER || param->data.parameter.name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *lf_name = alloc_lf_var_name(param->data.parameter.name);
  if (lf_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  free(param->data.parameter.name);
  param->data.parameter.name = lf_name;

  inner_symbol_t empty_sym = create_empty_symbol();

  err_code_t err = append_statement(context, INNER_DEFVAR, empty_sym,
                                    create_var_symbol(lf_name), empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = pop_parameter_list_reverse(context, param->next);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(append_statement(context, INNER_POPS, create_var_symbol(lf_name),
                              empty_sym, empty_sym));
}

err_code_t pop_parameter_lists_reverse(context_t *context,
                                       ast_node_t *param_list) {
  if (param_list == NULL) {
    RETURN_ERR(NO_ERR);
  }
  if (param_list->type != AST_PARAMETER_LIST) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = pop_parameter_lists_reverse(context, param_list->next);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(pop_parameter_list_reverse(
      context, param_list->data.parameter_list.parameters));
}

#pragma endregion

/**
 * @brief Adds call main and exit 0 statements to the program
 *
 * @param context Context structure
 * @param node AST node with the main declaration
 *
 * @return NO_ERR if everything ok
 */
err_code_t append_main_call(context_t *context, ast_node_t *node) {
  err_code_t err = NO_ERR;
  inner_symbol_t empty_sym = create_empty_symbol();

  inner_symbol_t main_label =
      create_label_symbol(node->data.function_declaration.name);
  err = append_statement(context, INNER_CALL, empty_sym, main_label, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err =
      append_statement(context, INNER_CLEARS, empty_sym, empty_sym, empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_value_t zero = {.inner_int = 0};
  inner_symbol_t zero_const = create_const_symbol(INNER_INT, zero);
  err = append_statement(context, INNER_EXIT, empty_sym, zero_const, empty_sym);

  RETURN_ERR(err);
}

/**
 * @brief Converts function definition to 3AC
 *
 * @param context Context of the function
 * @param node Node from AST which contains data of this function's declaration
 *
 * @return NO_ERR if everything ok
 */
err_code_t convert_function_declaration(context_t *context, ast_node_t *node) {
  // check for invalid inputs
  if (context == NULL || node == NULL ||
      node->type != AST_FUNCTION_DECLARATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *fn_body = node->data.function_declaration.body;
  data_type_node_t *return_type = node->data.function_declaration.return_type;
  if (fn_body == NULL || return_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->data.function_declaration.is_main) {
    err_code_t err = append_main_call(context, node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // builtin functions have the declaration predefined
  if (fn_body != NULL && fn_body->type == AST_BUILTIN_BODY) {
    if (context->tail == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    inner_statement_t *first_instruction =
        fn_body->data.builtin_body.first_instruction;
    if (first_instruction != NULL) {
      inner_statement_t *duplicated_tail = NULL;
      inner_statement_t *body =
          duplicate_statement_list(first_instruction, &duplicated_tail);
      if (body == NULL || duplicated_tail == NULL) {
        RETURN_ERR(INTERNAL_ERR);
      }
      context->tail->next_statement = body;
      context->tail = duplicated_tail;
    }

    ast_node_t *dependency = fn_body->data.builtin_body.dependencies;
    while (dependency != NULL) {
      enqueue_function(context, dependency);
      dependency = dependency->next;
    }

    RETURN_ERR(NO_ERR);
  }

  // label
  inner_symbol_t empty_sym = create_empty_symbol();
  err_code_t err = append_statement(
      context, INNER_LABEL, empty_sym,
      create_label_symbol(node->data.function_declaration.name), empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_CREATEFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = append_statement(context, INNER_PUSHFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (!node->data.function_declaration.is_main) {
    // Define LF parameter variables and pop arguments from the data stack
    err = pop_parameter_lists_reverse(
        context, node->data.function_declaration.parameter_lists);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // Mark the end of the function prologue
  context->function_start = context->tail;
  context->current_return_type = return_type;

  bool is_unit = return_type->type == DATA_TYPE_UNIT;

  err = convert_statement_list(context, node->data.function_declaration.body,
                               !is_unit);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (context->tail->operation != INNER_RETURN) {
    if (is_unit) {
      inner_value_t nil_val = {.inner_nil = NULL};
      inner_symbol_t nil_sym = create_const_symbol(INNER_NIL, nil_val);
      err =
          append_statement(context, INNER_PUSHS, empty_sym, nil_sym, empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    }

    err = append_statement(context, INNER_POPFRAME, empty_sym, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = append_statement(context, INNER_RETURN, empty_sym, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  RETURN_ERR(NO_ERR);
}

err_code_t convert_class_constructor(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_CLASS_DECLARATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *class_name = node->data.class_declaration.name;
  if (class_name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  size_t class_id = 0;
  err_code_t err = get_or_assign_class_id(context, node, &class_id);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  size_t attr_count = node->data.class_declaration.attribute_count;
  inner_symbol_t empty_sym = create_empty_symbol();

  // LABEL {class_name}
  err = append_statement(context, INNER_LABEL, empty_sym,
                         create_label_symbol(class_name), empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // CREATEFRAME
  err = append_statement(context, INNER_CREATEFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // PUSHFRAME
  err = append_statement(context, INNER_PUSHFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  context->function_start = context->tail;

  inner_symbol_t instance_variable;
  err = create_helper_variable(context, &instance_variable);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  inner_symbol_t temp_variable = create_empty_symbol();
  if (attr_count > 0) {
    err = create_helper_variable(context, &temp_variable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // NEWARRAY {instance_variable} int@{attr_count + 1}
  inner_value_t size_val = {.inner_int = (long long)(attr_count + 1)};
  inner_symbol_t size_sym = create_const_symbol(INNER_INT, size_val);
  err = append_statement(context, INNER_NEWARRAY, instance_variable, size_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // ARRAYSET {instance_variable} int@0 int@{class_id}
  inner_value_t zero_val = {.inner_int = 0};
  inner_symbol_t zero_sym = create_const_symbol(INNER_INT, zero_val);
  inner_value_t id_val = {.inner_int = (long long)class_id};
  inner_symbol_t id_sym = create_const_symbol(INNER_INT, id_val);

  err = append_statement(context, INNER_ARRAYSET, instance_variable, zero_sym,
                         id_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Pop arguments to class
  for (size_t i = attr_count; i >= 1; i--) {
    // Get value from stack
    err = append_statement(context, INNER_POPS, temp_variable, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    inner_value_t idx_value = {.inner_int = (long long)i};
    inner_symbol_t idx_sym = create_const_symbol(INNER_INT, idx_value);

    err = append_statement(context, INNER_ARRAYSET, instance_variable, idx_sym,
                           temp_variable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // PUSHS {instance_variable}
  err = append_statement(context, INNER_PUSHS, empty_sym, instance_variable,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // POPFRAME
  err = append_statement(context, INNER_POPFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // RETURN
  err =
      append_statement(context, INNER_RETURN, empty_sym, empty_sym, empty_sym);
  RETURN_ERR(err);
}

err_code_t convert_lambda_declaration(context_t *context, ast_node_t *node) {
  if (context == NULL || node == NULL || node->type != AST_LAMBDA_DECLARATION ||
      node->data_type == NULL || node->data_type->type != DATA_TYPE_LAMBDA ||
      node->data_type->data.lambda_type.return_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *lambda_name = node->data.lambda_declaration.unique_id;
  ast_node_t *body = node->data.lambda_declaration.body;
  if (lambda_name == NULL || body == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t empty_sym = create_empty_symbol();

  // LABEL {lambda_name}
  err_code_t err =
      append_statement(context, INNER_LABEL, empty_sym,
                       create_label_symbol(lambda_name), empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // CREATEFRAME
  err = append_statement(context, INNER_CREATEFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // PUSHFRAME
  err = append_statement(context, INNER_PUSHFRAME, empty_sym, empty_sym,
                         empty_sym);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  err = pop_parameter_list_reverse(context,
                                   node->data.lambda_declaration.parameters);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  context->function_start = context->tail;
  context->current_return_type = node->data_type->data.lambda_type.return_type;

  bool is_unit =
      node->data_type->data.lambda_type.return_type->type == DATA_TYPE_UNIT;

  err = convert_statement_list(context, body, !is_unit);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (context->tail->operation != INNER_RETURN) {
    if (is_unit) {
      inner_value_t nil_val = {.inner_nil = NULL};
      inner_symbol_t nil_sym = create_const_symbol(INNER_NIL, nil_val);
      err =
          append_statement(context, INNER_PUSHS, empty_sym, nil_sym, empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    }

    err = append_statement(context, INNER_POPFRAME, empty_sym, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = append_statement(context, INNER_RETURN, empty_sym, empty_sym,
                           empty_sym);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  RETURN_ERR(NO_ERR);
}

#pragma endregion

#pragma region Interface implementation

err_code_t enqueue_function(context_t *context, ast_node_t *declaration_node) {
  if (context == NULL || context->stack == NULL || context->table == NULL ||
      declaration_node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *name = NULL;
  switch (declaration_node->type) {
  case AST_FUNCTION_DECLARATION:
    name = declaration_node->data.function_declaration.name;
    break;
  case AST_CLASS_DECLARATION:
    name = declaration_node->data.class_declaration.name;
    break;
  case AST_LAMBDA_DECLARATION:
    name = declaration_node->data.lambda_declaration.unique_id;
    break;
  case AST_LIST_CONSTRUCTOR:
  case AST_VARIABLE_DECLARATION:
  case AST_PARAMETER:
    RETURN_ERR(NO_ERR);
  default:
    RETURN_ERR(INTERNAL_ERR);
  }

  if (name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // Check if node is already visited
  ast_node_t *table_node = NULL;
  err_code_t err = ht_get_item(context->table, name, (void **)&table_node);
  if (err != UNDEF_SYMBOL_ERR) {
    RETURN_ERR(err);
  }

  // Mark as visited
  err = ht_insert_item(context->table, name, declaration_node);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Add to stack to wait for processing
  if (!stack_push(context->stack, declaration_node)) {
    RETURN_ERR(INTERNAL_ERR);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t ast_convert(ast_node_t *main_node, inner_statement_t **code) {
  // Check input
  if (main_node == NULL || main_node->type != AST_FUNCTION_DECLARATION ||
      code == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  *code = NULL;

  context_t context = {
      .unique_label_counter = 0,
      .unique_var_counter = 0,
      .unique_class_counter = 0,
      .head = NULL,
      .stack = NULL,
      .table = NULL,
      .tail = NULL,
      .function_start = NULL,
      .current_return_type = NULL,
  };

  // Initialize variables
  context.stack = create_stack();
  if (context.stack == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = ht_init(&context.table, VISITED_TABLE_SIZE);
  if (err != NO_ERR) {
    goto cleanup;
  }

  // Start converting from main
  err = enqueue_function(&context, main_node);
  if (err != NO_ERR) {
    goto cleanup;
  }

  while (stack_get_size(context.stack) > 0) {
    t_stack_element *current_element = stack_get_top(context.stack);
    if (current_element == NULL) {
      err = INTERNAL_ERR;
      goto cleanup;
    }

    ast_node_t *current = current_element->data;
    stack_pop(context.stack);

    switch (current->type) {
    case AST_FUNCTION_DECLARATION:
      err = convert_function_declaration(&context, current);
      break;
    case AST_CLASS_DECLARATION:
      err = convert_class_constructor(&context, current);
      break;
    case AST_LAMBDA_DECLARATION:
      err = convert_lambda_declaration(&context, current);
      break;
    default:
      err = INTERNAL_ERR;
    }

    if (err != NO_ERR) {
      goto cleanup;
    }
  }

  *code = context.head;

cleanup:
  if (context.stack != NULL) {
    while (stack_get_size(context.stack) > 0) {
      stack_pop(context.stack);
    }
    free(context.stack);
  }

  if (err != NO_ERR) {
    destroy_statement_list(context.head);
  }

  if (context.table != NULL) {
    ht_destruct(&context.table);
  }

  RETURN_ERR(err);
}

#pragma endregion