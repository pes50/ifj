/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file analyse_callable_declarations.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of analyse_function_declaration and
analyse_lambda_declaration for semantic analysis
 *
\*****************************************************************************/

#include "analyse_callable_declarations.h"
#include "analyse_block.h"
#include "analyse_expression.h"
#include "analyse_types.h"

err_code_t add_parameters_to_scope(ast_node_t *node, hashtable_t *symtable) {
  ast_node_t *current_list = node->data.function_declaration.parameter_lists;
  while (current_list != NULL) {
    ast_node_t *current_parameter =
        current_list->data.parameter_list.parameters;
    while (current_parameter != NULL) {
      if (current_parameter->data.parameter.name == NULL) {
        return INTERNAL_ERR;
      }

      err_code_t err =
          analyse_type(current_parameter->data.parameter.data_type, symtable);
      if (err != NO_ERR) {
        return err;
      }

      err = ht_insert_item(symtable, current_parameter->data.parameter.name,
                           (void **)&current_parameter);
      if (err != NO_ERR) {
        return err;
      }
      current_parameter = current_parameter->next;
    }
    current_list = current_list->next;
  }

  return NO_ERR;
}

err_code_t analyse_function_declaration(ast_node_t *node,
                                        hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_FUNCTION_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  // Already analysed
  if (node->data_type != NULL) {
    return NO_ERR;
  }

  // Recursive function without defined type
  if (node->_visited) {
    return TYPE_COMPAT_ERR;
  }

  ast_node_t *body = node->data.function_declaration.body;
  if (body == NULL || body->type != AST_BLOCK) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  data_type_node_t *return_type = node->data.function_declaration.return_type;
  if (return_type != NULL) {
    err = analyse_type(return_type, symtable);
    if (err != NO_ERR) {
      return err;
    }
  }

  // Create new scope
  hashtable_t *current_scope;
  if (ht_init(&current_scope, local_scope_table_size) != NO_ERR) {
    return INTERNAL_ERR;
  }
  current_scope->parent = symtable;

  // Create symbols for parameters
  err = add_parameters_to_scope(node, current_scope);
  if (err != NO_ERR) {
    goto fun_err;
  }

  // Return type is known => function body must return this type
  if (return_type != NULL) {
    // Duplicate type
    err = duplicate_type_node(return_type, &body->data_type);
    if (err != NO_ERR) {
      goto fun_err;
    }
  }

  node->_visited = true;
  err = analyse_block(body, current_scope, return_type, true);
  if (err != NO_ERR) {
    goto fun_err;
  }

  // Update if return type of function was unknown during the collecting
  // symbols phase
  if (node->data.function_declaration.return_type == NULL) {
    // Duplicate type
    err = duplicate_type_node(body->data_type, &node->data_type);
    if (err != NO_ERR) {
      goto fun_err;
    }
  } else if (!are_data_types_identical(node->data_type, &node_unit)) {
    if (!are_data_types_compatible(body->data_type, node->data_type)) {
      err = TYPE_COMPAT_ERR;
      goto fun_err;
    }
  }

  node->_visited = false;
  ht_destruct(&current_scope);
  return NO_ERR;
fun_err:
  node->_visited = false;
  ht_destruct(&current_scope);
  return err;
}

err_code_t analyse_lambda_declaration(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_LAMBDA_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  data_type_node_t *return_type = node->data.lambda_declaration.return_type;
  if (return_type == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *body = node->data.lambda_declaration.body;
  if (body == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  // Create new scope
  hashtable_t *current_scope;
  if (ht_init(&current_scope, local_scope_table_size) != NO_ERR) {
    return INTERNAL_ERR;
  }
  current_scope->parent = symtable;

  // Create symbols for parameters
  err_code_t err = add_parameters_to_scope(node, current_scope);
  if (err != NO_ERR) {
    goto fun_err;
  }

  // Duplicate type
  err = duplicate_type_node(return_type, &node->data_type);
  if (err != NO_ERR) {
    goto fun_err;
  }

  if (body->type == AST_BLOCK) {
    err = analyse_block(body, current_scope, return_type, false);
  } else {
    err = analyse_expression(body, current_scope, node->data_type, true, true);
  }
  if (err != NO_ERR) {
    goto fun_err;
  }

  ht_destruct(&current_scope);
  return NO_ERR;
fun_err:
  ht_destruct(&current_scope);
  return err;
}