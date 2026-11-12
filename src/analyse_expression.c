/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file analyse_expression.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of analyse_expression function for semantic analysis
 *
\*****************************************************************************/

#include "analyse_expression.h"
#include "analyse_block.h"
#include "analyse_callable_declarations.h"
#include "analyse_types.h"
#include "string_concatenate_dot.h"

#pragma region Function call

err_code_t analyse_argument_list(ast_node_t *parameter_list,
                                 ast_node_t *argument_list,
                                 hashtable_t *symtable,
                                 data_type_node_t *target_type) {

  if (symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (argument_list == NULL && parameter_list == NULL) {
    return NO_ERR;
  }

  if (argument_list == NULL || parameter_list == NULL) {
    return ARG_MISMATCH_ERR;
  }

  if (argument_list->type != AST_PARAMETER_LIST ||
      parameter_list->type != AST_PARAMETER_LIST) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  ast_node_t *current_argument = argument_list->data.parameter_list.parameters;
  ast_node_t *current_parameter =
      parameter_list->data.parameter_list.parameters;

  if (target_type != NULL) {
    if (!are_data_types_compatible(current_parameter->data_type, target_type)) {
      return ARG_MISMATCH_ERR;
    }

    current_parameter = current_parameter->next;
  }

  while (current_argument != NULL && current_parameter != NULL) {
    err = analyse_expression(current_argument, symtable, NULL, true, false);
    if (err != NO_ERR) {
      return err;
    }
    if (!are_data_types_compatible(current_argument->data_type,
                                   current_parameter->data_type)) {
      return ARG_MISMATCH_ERR;
    }

    current_argument = current_argument->next;
    current_parameter = current_parameter->next;
  }

  if (current_argument != NULL || current_parameter != NULL) {
    return ARG_MISMATCH_ERR;
  }

  if (err == NO_ERR) {
    err = analyse_argument_list(parameter_list->next, argument_list->next,
                                symtable, NULL);
  }

  return err;
}

err_code_t analyse_function_call(char *identifier, ast_node_t *node,
                                 hashtable_t *symtable,
                                 data_type_node_t *target_type) {
  ast_node_t *table_node;
  err_code_t err = ht_get_item(symtable, identifier, (void **)&table_node);
  if (err != NO_ERR) {
    return err;
  }

  if (table_node->type != AST_FUNCTION_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  err = analyse_function_declaration(table_node, ht_get_root(symtable));
  if (err != NO_ERR) {
    return err;
  }

  err = analyse_argument_list(
      table_node->data.function_declaration.parameter_lists,
      node->data.function_call.argument_lists, symtable, target_type);
  if (err != NO_ERR) {
    return err;
  }

  return duplicate_type_node(table_node->data_type, &node->data_type);
}

#pragma endregion

err_code_t analyse_if(ast_node_t *node, hashtable_t *symtable,
                      data_type_node_t *explicit_data_type, bool is_last,
                      bool allow_return) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  ast_node_t *condition = node->data.if_statement.condition;
  ast_node_t *true_branch = node->data.if_statement.true_branch;
  ast_node_t *false_branch = node->data.if_statement.false_branch;

  if (true_branch == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  // Check expression
  err_code_t err = analyse_expression(condition, symtable, NULL, true, false);
  if (err != NO_ERR) {
    return err;
  }

  if (!are_data_types_identical(condition->data_type, &node_boolean)) {
    return TYPE_COMPAT_ERR;
  }

  // Create new scope
  hashtable_t *current_scope;
  err = ht_init(&current_scope, local_scope_table_size);
  if (err != NO_ERR) {
    return err;
  }
  current_scope->parent = symtable;

  err = analyse_block(true_branch, current_scope, explicit_data_type,
                      allow_return);
  ht_destruct(&current_scope);

  if (err != NO_ERR) {
    return err;
  }

  if (false_branch == NULL) {
    if (is_last) {
      return OTHER_SEMANTIC_ERR;
    }
    node->data_type = &node_unit;
    return NO_ERR;
  }

  // Create new scope
  err = ht_init(&current_scope, local_scope_table_size);
  if (err != NO_ERR) {
    return err;
  }
  current_scope->parent = symtable;

  err = analyse_block(false_branch, current_scope, explicit_data_type,
                      allow_return);
  ht_destruct(&current_scope);
  if (err != NO_ERR) {
    return err;
  }

  if (are_data_types_identical(explicit_data_type, &node_unit) || !is_last) {
    // Do not check type compatibility, values will be discarded
    node->data_type = &node_unit;
    return NO_ERR;
  }

  if (explicit_data_type == NULL) {
    // Both branches must have the same type
    if (!are_data_types_compatible(true_branch->data_type,
                                   false_branch->data_type) &&
        !are_data_types_compatible(false_branch->data_type,
                                   true_branch->data_type)) {
      return TYPE_COMPAT_ERR;
    }

    // Choose larger type
    err = duplicate_superset_type(true_branch->data_type,
                                  false_branch->data_type, &node->data_type);
    return err;
  }

  // Both branches must have compatible type with the return type
  if (!are_data_types_compatible(true_branch->data_type, explicit_data_type) ||
      !are_data_types_compatible(false_branch->data_type, explicit_data_type)) {
    return TYPE_COMPAT_ERR;
  }

  err = duplicate_type_node(explicit_data_type, &node->data_type);
  return err;
}

err_code_t analyse_binary_operation(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  ast_node_t *left = node->data.binary_operation.left;
  ast_node_t *right = node->data.binary_operation.right;
  if (left == NULL || right == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = analyse_expression(left, symtable, NULL, true, false);
  if (err == NO_ERR) {
    err = analyse_expression(right, symtable, NULL, true, false);
    if (err == NO_ERR &&
        !are_data_types_compatible(left->data_type, right->data_type) &&
        !are_data_types_compatible(right->data_type, left->data_type)) {
      err = TYPE_COMPAT_ERR;
    } else {
      switch (node->data.binary_operation.op) {
        // Operators
      case TOK_BINARY_PLUS:
        err = duplicate_superset_type(left->data_type, right->data_type,
                                      &node->data_type);
        if (err == NO_ERR) {
          if (!are_data_types_compatible(node->data_type, &node_double) &&
              !are_data_types_compatible(node->data_type, &node_string)) {
            err = TYPE_COMPAT_ERR;
          }
        }
        break;
      case TOK_BINARY_MINUS:
      case TOK_MUL:
      case TOK_DIV:
        err = duplicate_superset_type(left->data_type, right->data_type,
                                      &node->data_type);
        if (err == NO_ERR) {
          if (!are_data_types_compatible(node->data_type, &node_double)) {
            err = TYPE_COMPAT_ERR;
          }
        }
        break;
      case TOK_LOWER_T:
      case TOK_LOWER_EQ:
      case TOK_GREATER_T:
      case TOK_GREATER_EQ:
      case TOK_EQUAL:
      case TOK_NOT_EQ:
      case TOK_AND:
      case TOK_OR:
        err = duplicate_type_node(&node_boolean, &node->data_type);
        break;
      default:
        return INTERNAL_ERR;
      }
    }
  }

  return err;
}

#pragma region Match

err_code_t analyse_match_case_literal(ast_node_t *node, hashtable_t *symtable,
                                      data_type_node_t *match_type) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_LITERAL) {
    return INTERNAL_ERR;
  }

  if (!are_data_types_compatible(node->data.literal.data_type, match_type)) {
    return TYPE_COMPAT_ERR;
  }

  return duplicate_type_node(node->data.literal.data_type, &node->data_type);
}

err_code_t analyse_match_case_class_constructor(ast_node_t *node,
                                                hashtable_t *symtable,
                                                data_type_node_t *match_type) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_FUNCTION_CALL) {
    return INTERNAL_ERR;
  }

  err_code_t err = NO_ERR;
  ast_node_t *table_node = NULL;
  err = ht_get_item(symtable, node->data.function_call.name,
                    (void **)&table_node);
  if (err != NO_ERR || table_node == NULL) {
    return err;
  }

  if (table_node->type != AST_CLASS_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  if (!are_data_types_compatible(table_node->data_type, match_type)) {
    return TYPE_COMPAT_ERR;
  }

  ast_node_t *attribute = table_node->data.class_declaration.attributes;
  ast_node_t *argument = node->data.function_call.argument_lists;
  while (attribute != NULL && argument != NULL) {
    char *argument_name = argument->data.identifier.name;
    if (argument->type != AST_IDENTIFIER || argument_name == NULL) {
      return OTHER_SEMANTIC_ERR;
    }

    data_type_node_t *attribute_type = NULL;
    err = duplicate_type_node(attribute->data_type, &attribute_type);
    if (err != NO_ERR) {
      return err;
    }

    argument->type = AST_PARAMETER;
    argument->data.parameter.name = argument_name;
    argument->data.parameter.data_type = attribute_type;

    err = duplicate_type_node(argument->data.parameter.data_type,
                              &argument->data_type);
    if (err != NO_ERR) {
      return err;
    }

    err = ht_insert_item(symtable, argument->data.identifier.name,
                         (void **)&argument);
    if (err != NO_ERR) {
      return err;
    }

    attribute = attribute->next;
    argument = argument->next;
  }
  if (attribute != NULL || argument != NULL) {
    return ARG_MISMATCH_ERR;
  }

  return duplicate_type_node(table_node->data_type, &node->data_type);
}

err_code_t analyse_match_case_parameter(ast_node_t *node,
                                        hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_PARAMETER) {
    return INTERNAL_ERR;
  }

  char *name = node->data.parameter.name;
  data_type_node_t *data_type = node->data.parameter.data_type;
  if (name == NULL || data_type == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = analyse_type(data_type, symtable);
  if (err != NO_ERR) {
    return err;
  }

  err = ht_insert_item(symtable, name, (void **)&node);

  if (err != NO_ERR) {
    return err;
  }

  return duplicate_type_node(data_type, &node->data_type);
}

err_code_t analyse_match_case(ast_node_t *node, hashtable_t *symtable,
                              data_type_node_t *explicit_data_type,
                              bool allow_return, int *default_count,
                              data_type_node_t *match_type) {
  if (node == NULL || symtable == NULL || match_type == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_MATCH_CASE) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *pattern = node->data.match_case.pattern;
  ast_node_t *guard = node->data.match_case.guard;
  ast_node_t *block = node->data.match_case.block;
  if (pattern == NULL || block == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  // Create new scope
  hashtable_t *current_scope;
  err = ht_init(&current_scope, local_scope_table_size);
  if (err != NO_ERR) {
    return err;
  }
  current_scope->parent = symtable;

  if (pattern->type == AST_LITERAL &&
      are_data_types_identical(pattern->data.literal.data_type, &node_any)) {
    if (*default_count > 0) {
      return OTHER_SEMANTIC_ERR;
    }

    if (guard != NULL) {
      return OTHER_SEMANTIC_ERR;
    }

    (*default_count)++;
  } else {
    switch (pattern->type) {
    case AST_LITERAL:
      err = analyse_match_case_literal(pattern, current_scope, match_type);
      break;
    case AST_PARAMETER:
      err = analyse_match_case_parameter(pattern, current_scope);
      break;
    case AST_FUNCTION_CALL:
      err = analyse_match_case_class_constructor(pattern, current_scope,
                                                 match_type);
      break;
    default:
      goto case_err;
    }
  }
  if (err != NO_ERR) {
    goto case_err;
  }

  if (guard != NULL) {
    err = analyse_expression(guard, current_scope, explicit_data_type, true,
                             true);
    if (err != NO_ERR) {
      goto case_err;
    }

    if (!are_data_types_identical(&node_boolean, guard->data_type)) {
      goto case_err;
    }
  }

  if (block->type == AST_BLOCK) {
    err = analyse_block(block, current_scope, explicit_data_type, allow_return);
    if (err != NO_ERR) {
      goto case_err;
    }
  } else {
    err = analyse_expression(block, current_scope, explicit_data_type, true,
                             allow_return);
    if (err != NO_ERR) {
      goto case_err;
    }
  }
  ht_destruct(&current_scope);

  if (err != NO_ERR) {
    return err;
  }

  return duplicate_type_node(block->data_type, &node->data_type);
case_err:
  ht_destruct(&current_scope);
  return err;
}

err_code_t analyse_match(ast_node_t *node, hashtable_t *symtable,
                         data_type_node_t *explicit_data_type,
                         bool allow_return) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  ast_node_t *cases = node->data.match.cases;
  ast_node_t *expression = node->data.match.expression;

  if (node->type != AST_MATCH || cases == NULL || expression == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = analyse_expression(expression, symtable, NULL, true, false);
  if (err != NO_ERR) {
    return err;
  }

  int default_count = 0;
  ast_node_t *current = cases;
  while (current != NULL) {
    err =
        analyse_match_case(current, symtable, explicit_data_type, allow_return,
                           &default_count, expression->data_type);
    if (err != NO_ERR) {
      return err;
    }

    if (node->data_type == NULL) {
      node->data_type = current->data_type;
    } else {
      if (!are_data_types_compatible(current->data_type, node->data_type)) {
        if (are_data_types_compatible(node->data_type, current->data_type)) {
          node->data_type = current->data_type;
        } else {
          err = TYPE_COMPAT_ERR;
        }
      }
      if (err != NO_ERR) {
        node->data_type = NULL;
        return err;
      }
    }
    current = current->next;
  }

  data_type_node_t *data_type = NULL;
  err = duplicate_type_node(node->data_type, &data_type);
  if (err != NO_ERR) {
    node->data_type = NULL;
    return err;
  }
  node->data_type = data_type;

  if (default_count != 1) {
    return OTHER_SEMANTIC_ERR;
  }

  return NO_ERR;
}

#pragma endregion

err_code_t analyse_expression(ast_node_t *node, hashtable_t *symtable,
                              data_type_node_t *explicit_data_type,
                              bool is_last, bool allow_return) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = NO_ERR;
  ast_node_t *table_node = NULL;
  switch (node->type) {
  case AST_FUNCTION_DECLARATION:
  case AST_LAMBDA_DECLARATION:
  case AST_PARAMETER_LIST:
  case AST_PARAMETER:
  case AST_CLASS_DECLARATION:
  case AST_TYPE_DECLARATION:
  case AST_EXTENSION_DECLARATION:
  case AST_BLOCK:
  case AST_VARIABLE_DECLARATION:
  case AST_ASSIGNMENT:
  case AST_FOR_DO:
  case AST_WHILE_DO:
  case AST_RETURN:
  case AST_MATCH_CASE:
    err = OTHER_SEMANTIC_ERR;
    break;
  case AST_FUNCTION_CALL:
    err = analyse_function_call(node->data.function_call.name, node, symtable,
                                NULL);
    break;
  case AST_DOT_FUNCTION_CALL:
    err = analyse_expression(node->data.dot_function_call.target, symtable,
                             NULL, true, false);
    if (err == NO_ERR) {
      char *first;
      err = type_to_string(node->data.dot_function_call.target->data_type,
                           &first);
      if (err == NO_ERR) {
        char *identifier =
            string_concatenate_dot(first, node->data.dot_function_call.name);
        if (identifier != NULL) {
          err = analyse_function_call(
              identifier, node, symtable,
              node->data.dot_function_call.target->data_type);
          free(identifier);
        } else {
          err = INTERNAL_ERR;
        }
        free(first);
      }
    }
    break;
  case AST_IF:
    err = analyse_if(node, symtable, explicit_data_type, is_last, allow_return);
    break;
  case AST_MATCH:
    err = analyse_match(node, symtable, explicit_data_type, allow_return);
    break;
  case AST_BINARY_OPERATION:
    err = analyse_binary_operation(node, symtable);
    break;
  case AST_UNARY_OPERATION:
    err = analyse_expression(node->data.unary_operation.expression, symtable,
                             NULL, true, false);
    if (err == NO_ERR) {
      err = duplicate_type_node(
          node->data.unary_operation.expression->data_type, &node->data_type);
    }
    break;
  case AST_IDENTIFIER:
    err =
        ht_get_item(symtable, node->data.identifier.name, (void **)&table_node);
    if (err == NO_ERR) {
      if (table_node->type != AST_VARIABLE_DECLARATION &&
          table_node->type != AST_PARAMETER) {
        err = OTHER_SEMANTIC_ERR;
      } else {
        if (table_node->data_type == NULL) {
          err = OTHER_SEMANTIC_ERR;
        } else {
          err = duplicate_type_node(table_node->data_type, &node->data_type);
        }
      }
    }
    break;
  case AST_LITERAL:
    err = duplicate_type_node(node->data.literal.data_type, &node->data_type);
    break;
  case AST_INTERPOLATED_STRING:
    table_node = node->data.interpolated_string.expressions;
    while (table_node != NULL) {
      err = analyse_expression(table_node, symtable, NULL, true, false);
      if (err != NO_ERR) {
        break;
      }
      table_node = table_node->next;
    }

    if (err == NO_ERR) {
      err = duplicate_type_node(&node_string, &node->data_type);
    }
  }

  return err;
}
