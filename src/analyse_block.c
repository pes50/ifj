/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file analyse_block.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of analyse_block function and helper functions for
semantic analysis
 *
\*****************************************************************************/

#include "analyse_block.h"
#include "analyse_callable_declarations.h"
#include "analyse_expression.h"
#include "analyse_types.h"

err_code_t analyse_while_do(ast_node_t *node, hashtable_t *symtable,
                            data_type_node_t *explicit_type) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_WHILE_DO) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *condition = node->data.while_do.condition;

  if (condition == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = analyse_expression(condition, symtable, NULL, true, false);
  if (err == NO_ERR) {
    if (!are_data_types_identical(condition->data_type, &node_boolean)) {
      err = TYPE_COMPAT_ERR;
    }

    if (err == NO_ERR) {
      // Create new scope
      hashtable_t *current_scope;
      if (ht_init(&current_scope, local_scope_table_size) != NO_ERR) {
        return INTERNAL_ERR;
      }
      current_scope->parent = symtable;
      err = analyse_block(node->data.while_do.body, current_scope,
                          explicit_type, true);
      ht_destruct(&current_scope);
    }
  }

  if (err == NO_ERR) {
    err = duplicate_type_node(&node_unit, &node->data_type);
  }

  return err;
}

err_code_t analyse_variable_declaration(ast_node_t *node,
                                        hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  ast_node_t *expression = node->data.variable_declaration.expression;

  if (node->type != AST_VARIABLE_DECLARATION || expression == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = analyse_expression(expression, symtable,
                                      node->data.variable_declaration.data_type,
                                      true, false);
  if (err != NO_ERR) {
    return err;
  }

  data_type_node_t *data_type = node->data.variable_declaration.data_type;
  if (data_type != NULL) {
    err = analyse_type(data_type, symtable);
    if (err != NO_ERR) {
      return err;
    }

    if (!are_data_types_compatible(expression->data_type, data_type)) {
      return OTHER_SEMANTIC_ERR;
    }
  }

  err = ht_insert_item(symtable, node->data.variable_declaration.name, node);
  if (err != NO_ERR) {
    return err;
  }

  err = duplicate_type_node(expression->data_type, &node->data_type);

  return err;
}

err_code_t analyse_assignment(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_ASSIGNMENT ||
      node->data.assignment.expression == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *table_node;
  err_code_t err =
      ht_get_item(symtable, node->data.assignment.name, (void **)&table_node);
  if (err != NO_ERR) {
    return err;
  }

  if (table_node->type == AST_PARAMETER) {
    return ASSIGN_CONST_ERR;
  }

  if (table_node->type != AST_VARIABLE_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  if (table_node->data.variable_declaration.is_const) {
    return ASSIGN_CONST_ERR;
  }

  err = analyse_expression(node->data.assignment.expression, symtable, NULL,
                           true, false);
  if (err != NO_ERR) {
    return err;
  }

  if (!are_data_types_compatible(node->data.assignment.expression->data_type,
                                 table_node->data_type)) {
    return TYPE_COMPAT_ERR;
  }

  err = duplicate_type_node(&node_unit, &node->data_type);

  return err;
}

err_code_t analyse_return(ast_node_t *node, hashtable_t *symtable,
                          data_type_node_t *explicit_data_type,
                          bool allow_return) {

  if (!allow_return) {
    return OTHER_SEMANTIC_ERR;
  }
  // Type must be explicitly defined for return
  if (explicit_data_type == NULL) {
    return TYPE_COMPAT_ERR;
  }

  ast_node_t *expression = node->data.return_statement.expression;
  if (expression == NULL) {
    // No expression is allowed only when return type is explicitly Unit
    if (!are_data_types_identical(&node_unit, explicit_data_type)) {
      return TYPE_COMPAT_ERR;
    }
    node->data_type = &node_unit;
  } else {
    err_code_t err =
        analyse_expression(expression, symtable, NULL, true, false);
    if (err != NO_ERR) {
      return err;
    }

    // Discard any return value if return type is explicitly Unit
    if (are_data_types_identical(explicit_data_type, &node_unit)) {
      node->data.return_statement.discard = true;
      node->data_type = &node_unit;
    } else {
      if (!are_data_types_compatible(expression->data_type,
                                     explicit_data_type)) {
        return TYPE_COMPAT_ERR;
      }
      return duplicate_type_node(expression->data_type, &node->data_type);
    }
  }

  return NO_ERR;
}

err_code_t analyse_for_do(ast_node_t *node, hashtable_t *symtable,
                          data_type_node_t *explicit_data_type,
                          bool allow_return) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_FOR_DO) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *body = node->data.for_do.body;
  ast_node_t *generator = node->data.for_do.generator;
  ast_node_t *iterator = node->data.for_do.iterator;

  if (body == NULL || generator == NULL || iterator == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  if (iterator->type != AST_PARAMETER ||
      iterator->data.parameter.name == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  if (iterator->data.parameter.data_type != NULL) {
    err = analyse_type(iterator->data.parameter.data_type, symtable);
    if (err != NO_ERR) {
      return err;
    }
  }

  err = analyse_expression(generator, symtable, NULL, true, false);

  if (err != NO_ERR) {
    return err;
  }

  if (generator->data_type == NULL ||
      (generator->data_type->type != DATA_TYPE_LIST &&
       generator->data_type->type != DATA_TYPE_RANGE)) {
    return TYPE_COMPAT_ERR;
  }

  if (generator->data_type->type == DATA_TYPE_LIST) {
    data_type_node_t *elements_type =
        generator->data_type->data.list_type.elements_type;

    if (iterator->data.parameter.data_type == NULL) {
      err = duplicate_type_node(elements_type,
                                &iterator->data.parameter.data_type);
      if (err != NO_ERR) {
        return err;
      }
    } else {
      if (!are_data_types_compatible(iterator->data.parameter.data_type,
                                     elements_type)) {
        return TYPE_COMPAT_ERR;
      }
    }
  } else {
    // Range
    if (iterator->data.parameter.data_type == NULL) {
      err = duplicate_type_node(&node_int, &iterator->data.parameter.data_type);
      if (err != NO_ERR) {
        return err;
      }
    } else {
      if (!are_data_types_identical(&node_int,
                                    iterator->data.parameter.data_type)) {
        return TYPE_COMPAT_ERR;
      }
    }
  }

  // Create new scope
  hashtable_t *current_scope;
  err = ht_init(&current_scope, local_scope_table_size);
  if (err == NO_ERR) {
    current_scope->parent = symtable;
  } else {
    return err;
  }

  err = duplicate_type_node(iterator->data.parameter.data_type,
                            &iterator->data_type);
  if (err != NO_ERR) {
    ht_destruct(&current_scope);
    return err;
  }

  err = ht_insert_item(current_scope, iterator->data.parameter.name, iterator);
  if (err != NO_ERR) {
    ht_destruct(&current_scope);
    return err;
  }

  ast_node_t *current = node->data.for_do.guards;
  while (current != NULL) {
    err = analyse_expression(current, current_scope, NULL, true, false);
    if (err != NO_ERR) {
      return err;
    }

    if (!are_data_types_identical(&node_boolean, current->data_type)) {
      return TYPE_COMPAT_ERR;
    }

    current = current->next;
  }

  err = analyse_block(body, current_scope, explicit_data_type, allow_return);
  ht_destruct(&current_scope);

  if (err == NO_ERR) {
    err = duplicate_type_node(&node_unit, &node->data_type);
  }

  return err;
}

err_code_t analyse_block(ast_node_t *node, hashtable_t *symtable,
                         data_type_node_t *explicit_data_type,
                         bool allow_return) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_BLOCK || node->data.block.statement == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  data_type_node_t *last_type = &node_unit;
  ast_node_t *current = node->data.block.statement;
  hashtable_t *current_scope;
  while (current != NULL) {
    switch (current->type) {
    case AST_FUNCTION_DECLARATION:
    case AST_PARAMETER_LIST:
    case AST_PARAMETER:
    case AST_ARGUMENT_LIST:
    case AST_CLASS_DECLARATION:
    case AST_TYPE_DECLARATION:
    case AST_EXTENSION_DECLARATION:
    case AST_MATCH_CASE:
    case AST_LIST_CONSTRUCTOR:
    case AST_BUILTIN_BODY:
      err = OTHER_SEMANTIC_ERR;
      break;
    case AST_FUNCTION_CALL:
    case AST_DOT_FUNCTION_CALL:
    case AST_BINARY_OPERATION:
    case AST_UNARY_OPERATION:
    case AST_IDENTIFIER:
    case AST_LITERAL:
    case AST_INTERPOLATED_STRING:
    case AST_IF:
    case AST_MATCH:
      err = analyse_expression(current, symtable, explicit_data_type,
                               current->next == NULL, true);
      if (err == NO_ERR) {
        last_type = current->data_type;
      }
      break;
    case AST_ASSIGNMENT:
      err = analyse_assignment(current, symtable);
      if (err == NO_ERR) {
        last_type = &node_unit;
      }
      break;
    case AST_LAMBDA_DECLARATION:
      err = analyse_expression(current, ht_get_root(symtable),
                               explicit_data_type, true, false);
      if (err == NO_ERR) {
        last_type = current->data_type;
      }
      break;
    case AST_BLOCK:
      // Create new scope
      err = ht_init(&current_scope, local_scope_table_size);
      if (err == NO_ERR) {
        current_scope->parent = symtable;
        err = analyse_block(current, current_scope, explicit_data_type, true);
        ht_destruct(&current_scope);
      }
      break;
    case AST_VARIABLE_DECLARATION:
      err = analyse_variable_declaration(current, symtable);
      if (err == NO_ERR) {
        last_type = &node_unit;
      }
      break;
    case AST_FOR_DO:
      err = analyse_for_do(current, symtable, explicit_data_type, true);
      if (err == NO_ERR) {
        last_type = &node_unit;
      }
      break;
    case AST_WHILE_DO:
      err = analyse_while_do(current, symtable, explicit_data_type);
      if (err == NO_ERR) {
        last_type = &node_unit;
      }
      break;
    case AST_RETURN:
      err = analyse_return(current, symtable, explicit_data_type, allow_return);
      if (err == NO_ERR) {
        last_type = current->data_type;
      }
      break;
    }

    if (err == NO_ERR) {
      current = current->next;
    } else {
      current = NULL;
    }
  }

  if (err == NO_ERR) {
    err = duplicate_type_node(last_type, &node->data_type);
  }

  return err;
}
