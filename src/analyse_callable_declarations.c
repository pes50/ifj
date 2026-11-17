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

err_code_t duplicate_function_type(ast_node_t *parameter_node,
                                   data_type_node_t *return_type,
                                   data_type_node_t **out_type) {
  data_type_node_t *duplicated_return_type = NULL;
  err_code_t err = duplicate_type_node(return_type, &duplicated_return_type);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  data_type_node_t *first_parameter_type = NULL;
  data_type_node_t *last_parameter_type = NULL;
  while (parameter_node != NULL) {
    data_type_node_t *parameter_type = NULL;
    err = duplicate_type_node(parameter_node->data.parameter.data_type,
                              &parameter_type);
    if (err != NO_ERR) {
      destroy_type_node(duplicated_return_type);
      destroy_type_node(first_parameter_type);
      RETURN_ERR(err);
    }

    if (first_parameter_type == NULL) {
      first_parameter_type = parameter_type;
      last_parameter_type = parameter_type;
    } else {
      last_parameter_type->next = parameter_type;
      last_parameter_type = parameter_type;
    }
    parameter_node = parameter_node->next;
  }
  *out_type =
      create_lambda_type_node(first_parameter_type, duplicated_return_type);
  if (*out_type == NULL) {
    destroy_type_node(duplicated_return_type);
    destroy_type_node(first_parameter_type);
    RETURN_ERR(INTERNAL_ERR);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t add_parameters_to_scope(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;
  ast_node_t temp_list = {
      .type = AST_PARAMETER_LIST,
      .data.parameter_list.parameters = NULL,
      .next = NULL,
  };

  ast_node_t *current_list = NULL;
  if (node->type == AST_FUNCTION_DECLARATION) {
    current_list = node->data.function_declaration.parameter_lists;
  } else if (node->type == AST_LAMBDA_DECLARATION) {
    temp_list.data.parameter_list.parameters =
        node->data.lambda_declaration.parameters;
    current_list = &temp_list;
  } else {
    RETURN_ERR(INTERNAL_ERR);
  }

  while (current_list != NULL) {
    ast_node_t *current_parameter =
        current_list->data.parameter_list.parameters;
    while (current_parameter != NULL) {
      if (current_parameter->data.parameter.name == NULL) {
        return INTERNAL_ERR;
      }

      err = analyse_type(current_parameter->data.parameter.data_type, symtable);
      if (err != NO_ERR) {
        return err;
      }

      err = ht_insert_item(symtable, current_parameter->data.parameter.name,
                           current_parameter);
      if (err != NO_ERR) {
        return err;
      }

      if (current_parameter->data_type == NULL) {
        err = duplicate_type_node(current_parameter->data.parameter.data_type,
                                  &current_parameter->data_type);
      }

      if (err != NO_ERR) {
        return err;
      }

      current_parameter = current_parameter->next;
    }
    if (current_list->data_type == NULL && current_list != &temp_list) {
      err = duplicate_type_node(&node_unit, &current_list->data_type);
    }

    if (err != NO_ERR) {
      return err;
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
    if (node->data.function_declaration.return_type == NULL) {
      return TYPE_COMPAT_ERR;
    }
    return NO_ERR;
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

  node->_visited = true;
  err = analyse_block(body, current_scope, return_type, true);
  if (err != NO_ERR) {
    goto fun_err;
  }

  if (return_type != NULL &&
      !are_data_types_identical(node->data_type, &node_unit)) {
    if (!are_data_types_compatible(body->data_type, return_type)) {
      err = TYPE_COMPAT_ERR;
      goto fun_err;
    }
  }

  // Duplicate type
  ast_node_t *parameter_list = node->data.function_declaration.parameter_lists;
  if (parameter_list == NULL || parameter_list->next != NULL) {
    err = duplicate_type_node(&node_curried_function, &node->data_type);
    if (err != NO_ERR) {
      goto fun_err;
    }
  } else {
    err =
        duplicate_function_type(parameter_list->data.parameter_list.parameters,
                                return_type, &node->data_type);
    if (err != NO_ERR) {
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

  if (body->type == AST_BLOCK) {
    err = analyse_block(body, current_scope, NULL, false);
  } else {
    err = analyse_expression(body, current_scope, NULL, true, true);
  }
  if (err != NO_ERR) {
    goto fun_err;
  }

  err = duplicate_function_type(node->data.lambda_declaration.parameters,
                                body->data_type, &node->data_type);
  if (err != NO_ERR) {
    goto fun_err;
  }

  ht_destruct(&current_scope);
  RETURN_ERR(NO_ERR);
fun_err:
  ht_destruct(&current_scope);
  RETURN_ERR(err);
}
