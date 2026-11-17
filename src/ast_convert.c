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

/********************\
 * Helper functions *
\********************/

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
    return INTERNAL_ERR;
  }

  // TODO
  // Use node->data.function_call.calling to get correct pointer

  return NO_ERR;
}

/****************************\
 * Interface implementation *
\****************************/

err_code_t enqueue_function(context_t *context,
                            ast_node_t *function_declaration_node) {
  // Check if arguments are valid
  if (context == NULL || context->stack == NULL || context->table == NULL ||
      function_declaration_node == NULL) {
    return INTERNAL_ERR;
  }

  if (function_declaration_node->type != AST_FUNCTION_DECLARATION) {
    return INTERNAL_ERR;
  }

  char *name = function_declaration_node->data.function_declaration.name;
  if (name == NULL) {
    return INTERNAL_ERR;
  }

  // Check if funtion is visited (already converted)
  ast_node_t *table_node = NULL;
  err_code_t err = ht_get_item(context->table, name, (void **)&table_node);
  if (err != UNDEF_SYMBOL_ERR) {
    return err;
  }

  // Mark function as visited (Needs to be done before actually processing the
  //                           function - it is saved on stack)
  err = ht_insert_item(context->table, name, function_declaration_node);
  if (err != NO_ERR) {
    return err;
  }

  // Add function to stack to wait for processing
  if (!stack_push(context->stack, function_declaration_node)) {
    return INTERNAL_ERR;
  }

  return NO_ERR;
}

err_code_t ast_convert(ast_node_t *main_node, inner_statement_t **code) {
  // Check input
  if (main_node == NULL || main_node->type != AST_FUNCTION_DECLARATION) {
    return INTERNAL_ERR;
  }

  context_t context = {
      .head = NULL,
      .stack = NULL,
      .table = NULL,
      .tail = NULL,
  };

  // Initialize variables
  context.stack = create_stack();
  if (context.stack == NULL) {
    return INTERNAL_ERR;
  }

  context.table = NULL;
  err_code_t err = ht_init(&context.table, visited_table_size);
  if (err != NO_ERR) {
    free(context.stack);
    return err;
  }

  // Start converting from main
  err = enqueue_function(&context, main_node);
  if (err != NO_ERR) {
    return err;
  }

  while (stack_get_size(context.stack) > 0) {
    t_stack_element *current_element = stack_get_top(context.stack);
    stack_pop(context.stack);
    ast_node_t *current = current_element->data;
    free(current_element);
    err = convert_function_declaration(&context, current);
    if (err != NO_ERR) {
      break;
    }
  }
  if (err != NO_ERR) {
    // Destroy stack
    while (stack_get_size(context.stack) > 0) {
      t_stack_element *current_element = stack_get_top(context.stack);
      stack_pop(context.stack);
      free(current_element);
    }
    free(context.stack);

    // destroy hashtable
    ht_destruct(&context.table);

    // TODO should code be also deallocated or returned in case of failure?
  } else {
    *code = context.head;
  }

  return err;
}
