/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file ast_convert.c
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation whole of AST to 3AC conversion
 *
 \*****************************************************************************/

#include "ast_convert.h"
#include "convert_node.h"
#include <stdio.h>
#include <string.h>

/********************\
 * Helper functions *
\********************/

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

  if (node->data.function_declaration.is_main) {
    err_code_t err = append_main_call(context, node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  // builtin functions have the declaration predefined
  ast_node_t *fn_body = node->data.function_declaration.body;
  if (fn_body != NULL && fn_body->type == AST_BUILTIN_BODY) {
    if (context->tail == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }
    if (fn_body->data.builtin_body.first_instruction != NULL) {
      inner_statement_t *duplicated_tail = NULL;
      inner_statement_t *body = duplicate_statement_list(
          fn_body->data.builtin_body.first_instruction, &duplicated_tail);
      if (body == NULL || duplicated_tail == NULL) {
        RETURN_ERR(INTERNAL_ERR);
      }
      context->tail->next_statement = body;
      context->tail = duplicated_tail;
    }
    RETURN_ERR(NO_ERR);
  }

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

  // saves pointer to first node from function body
  ast_node_t *curr_node = node->data.function_declaration.body;

  // goes through all body nodes and converts them to statements
  while (context->tail->operation != INNER_RETURN) {
    // adds return if the function doesn't have one
    if (curr_node == NULL) {
      err_code_t err = append_statement(context, INNER_POPFRAME, empty_sym,
                                        empty_sym, empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      err = append_statement(context, INNER_RETURN, empty_sym, empty_sym,
                             empty_sym);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      break;
    }

    err = convert_node(context, curr_node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    curr_node = curr_node->next;
  }

  RETURN_ERR(NO_ERR);
}

/****************************\
 * Interface implementation *
\****************************/

err_code_t enqueue_function(context_t *context,
                            ast_node_t *function_declaration_node) {
  // Check if arguments are valid
  if (context == NULL || context->stack == NULL || context->table == NULL ||
      function_declaration_node == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (function_declaration_node->type != AST_FUNCTION_DECLARATION) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *name = function_declaration_node->data.function_declaration.name;
  if (name == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // Check if function is visited (already converted)
  ast_node_t *table_node = NULL;
  err_code_t err = ht_get_item(context->table, name, (void **)&table_node);
  if (err != UNDEF_SYMBOL_ERR) {
    RETURN_ERR(err);
  }

  // Mark function as visited (Needs to be done before actually processing the
  //                           function - it is saved on stack)
  err = ht_insert_item(context->table, name, function_declaration_node);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  // Add function to stack to wait for processing
  if (!stack_push(context->stack, function_declaration_node)) {
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
      .head = NULL,
      .stack = NULL,
      .table = NULL,
      .tail = NULL,
  };

  // Initialize variables
  context.stack = create_stack();
  if (context.stack == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = ht_init(&context.table, visited_table_size);
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

    err = convert_function_declaration(&context, current);
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