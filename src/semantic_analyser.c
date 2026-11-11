/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file semantic_analyser.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of functions used in semantic analysis
 *
\*****************************************************************************/

#include "semantic_analyser.h"
#include "string_concatenate_dot.h"
#include "string_duplicate.h"
#include <stdlib.h>

#pragma region First pass

#pragma region Helpers

bool is_parameter_chain(ast_node_t *parameter) {
  if (parameter == NULL) {
    return false;
  }

  if (parameter->type != AST_PARAMETER) {
    return false;
  }

  if (parameter->next != NULL) {
    return is_parameter_chain(parameter->next);
  }

  return true;
}

bool is_parameter_list_chain(ast_node_t *parameter_list) {
  if (parameter_list == NULL) {
    return false;
  }

  if (parameter_list->type != AST_PARAMETER_LIST) {
    return false;
  }

  if (parameter_list->data.parameter_list.parameters != NULL) {
    if (!is_parameter_chain(parameter_list->data.parameter_list.parameters)) {
      return false;
    }
  }

  if (parameter_list->next == NULL) {
    return true;
  }

  return is_parameter_list_chain(parameter_list->next);
}

#pragma endregion

err_code_t collect_function(ast_node_t *node, hashtable_t *symtable,
                            char **main_identifier) {
  // Check if symbol is defined
  void *data;
  err_code_t err =
      ht_get_item(symtable, node->data.function_declaration.name, &data);
  if (err != UNDEF_SYMBOL_ERR) {
    // Symbol is in table
    if (data != NULL) {
      return REDEF_SYMBOL_ERR;
    }
    return err;
  }

  // Check if this is the main function
  if (node->data.function_declaration.is_main) {
    if (*main_identifier != NULL) {
      // Multiple @main
      return REDEF_SYMBOL_ERR;
    }

    // Non empty parameter list
    if (node->data.function_declaration.parameter_lists == NULL) {
      return ARG_MISMATCH_ERR;
    }

    // No parameters
    if (node->data.function_declaration.parameter_lists->data.parameter_list
            .parameters != NULL) {
      return ARG_MISMATCH_ERR;
    }

    // Only one parameter list
    if (node->data.function_declaration.parameter_lists->next != NULL) {
      return ARG_MISMATCH_ERR;
    }

    if (!are_data_types_identical(
            &node_unit, node->data.function_declaration.return_type)) {
      return TYPE_COMPAT_ERR;
    }

    *main_identifier = string_duplicate(node->data.function_declaration.name);
    if (*main_identifier == NULL) {
      return INTERNAL_ERR;
    }
  } else {
    if (node->data.function_declaration.parameter_lists != NULL &&
        !is_parameter_list_chain(
            node->data.function_declaration.parameter_lists)) {
      return OTHER_SEMANTIC_ERR;
    }
  }

  // Set node type if defined
  if (node->data.function_declaration.return_type != NULL) {
    if (duplicate_type_node(node->data.function_declaration.return_type,
                            &node->data_type) != NO_ERR) {
      return INTERNAL_ERR;
    }
  }

  // Define symbol
  if (ht_insert_item(symtable, node->data.function_declaration.name, node) !=
      NO_ERR) {
    return INTERNAL_ERR;
  }

  return NO_ERR;
}

err_code_t collect_extension(ast_node_t *node, hashtable_t *symtable) {
  if (node->data.extension_declaration.parameter == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *parameter = node->data.extension_declaration.parameter;
  if (parameter->type != AST_PARAMETER) {
    return OTHER_SEMANTIC_ERR;
  }

  if (parameter->next != NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  if (parameter->data.parameter.data_type == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  if (parameter->data.parameter.name == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  char *first = NULL;
  err_code_t err = type_to_string(parameter->data.parameter.data_type, &first);
  if (err != NO_ERR) {
    return err;
  }

  ast_node_t *current = node->data.extension_declaration.function_declarations;
  while (current != NULL) {
    if (current->type != AST_FUNCTION_DECLARATION) {
      return OTHER_SEMANTIC_ERR;
    }

    if (current->data.function_declaration.name == NULL) {
      return INTERNAL_ERR;
    }

    if (current->data.function_declaration.is_main) {
      return REDEF_SYMBOL_ERR;
    }

    char *symbol =
        string_concatenate_dot(first, current->data.function_declaration.name);
    if (symbol == NULL) {
      return INTERNAL_ERR;
    }

    // Check if symbol is defined
    void *data = NULL;
    err = ht_get_item(symtable, symbol, &data);
    if (err != UNDEF_SYMBOL_ERR) {
      // Symbol is in table
      if (data != NULL) {
        return REDEF_SYMBOL_ERR;
      }
      return err;
    }

    // Insert extension parameter to function parameters
    ast_node_t *parameter_copy =
        create_parameter_node(parameter->data.parameter.name, NULL);
    if (parameter_copy == NULL) {
      free(symbol);
      return INTERNAL_ERR;
    }

    if (duplicate_type_node(parameter->data.parameter.data_type,
                            &parameter_copy->data.parameter.data_type) !=
        NO_ERR) {
      free(symbol);
      destroy_node(parameter_copy);
      return INTERNAL_ERR;
    }

    ast_node_t *parameter_list = create_parameter_list_node(parameter_copy);
    if (parameter_list == NULL) {
      free(symbol);
      destroy_node(parameter_copy);
      return INTERNAL_ERR;
    }

    if (current->data.function_declaration.parameter_lists == NULL) {
      current->data.function_declaration.parameter_lists = parameter_list;
    } else {
      if (!is_parameter_list_chain(
              current->data.function_declaration.parameter_lists)) {
        return OTHER_SEMANTIC_ERR;
      }
      parameter_list->next = current->data.function_declaration.parameter_lists;
      current->data.function_declaration.parameter_lists = parameter_list;
    }

    // Define symbol
    if (ht_insert_item(symtable, symbol, current) != NO_ERR) {
      return INTERNAL_ERR;
    }

    free(symbol);

    current = current->next;
  }

  return NO_ERR;
}

err_code_t collect_type(ast_node_t *node, hashtable_t *symtable) {
  // Check if symbol is defined
  void *data = NULL;
  err_code_t err =
      ht_get_item(symtable, node->data.type_declaration.name, &data);
  if (err != UNDEF_SYMBOL_ERR) {
    // Symbol is in table
    if (data != NULL) {
      return REDEF_SYMBOL_ERR;
    }
    return err;
  }

  // Type must be defined
  if (node->data.type_declaration.data_type == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

  // Define symbol
  if (ht_insert_item(symtable, node->data.type_declaration.name, node) !=
      NO_ERR) {
    return INTERNAL_ERR;
  }

  return NO_ERR;
}

err_code_t collect_class(ast_node_t *node, hashtable_t *symtable) {
  // Check if symbol is defined
  void *data = NULL;
  err_code_t err =
      ht_get_item(symtable, node->data.class_declaration.name, &data);
  if (err != UNDEF_SYMBOL_ERR) {
    // Symbol is in table
    if (data != NULL) {
      return REDEF_SYMBOL_ERR;
    }
    return err;
  }

  if (!is_parameter_chain(node->data.class_declaration.attributes)) {
    return OTHER_SEMANTIC_ERR;
  }

  // Define symbol
  if (ht_insert_item(symtable, node->data.class_declaration.name, node) !=
      NO_ERR) {
    return INTERNAL_ERR;
  }

  return NO_ERR;
}

err_code_t collect_symbols(ast_node_t *root, hashtable_t *symtable,
                           char **main_identifier) {
  if (root == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (root->type != AST_BLOCK) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *current = root->data.block.statement;
  while (current != NULL) {
    err_code_t err;

    switch (current->type) {
    case AST_FUNCTION_DECLARATION:
      err = collect_function(current, symtable, main_identifier);
      break;
    case AST_EXTENSION_DECLARATION:
      err = collect_extension(current, symtable);
      break;
    case AST_TYPE_DECLARATION:
      err = collect_type(current, symtable);
      break;
    case AST_CLASS_DECLARATION:
      err = collect_class(current, symtable);
      break;
    default:
      return OTHER_SEMANTIC_ERR;
    }

    if (err != NO_ERR) {
      return err;
    }
    current = current->next;
  }

  return NO_ERR;
}

#pragma endregion

#pragma region Second pass

#define table_size 100

#pragma region Specification

err_code_t analyse_block(ast_node_t *node, hashtable_t *symtable,
                         data_type_node_t *explicit_data_type,
                         bool allow_return);

err_code_t analyse_expression(ast_node_t *node, hashtable_t *symtable,
                              data_type_node_t *explicit_data_type,
                              bool is_last, bool allow_return);

err_code_t analyse_function_declaration(ast_node_t *node,
                                        hashtable_t *symtable);

#pragma endregion

err_code_t analyse_type(data_type_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = NO_ERR;
  ast_node_t *table_node;
  switch (node->type) {
  case DATA_TYPE_CLASS:
    // identifier must be in the symtable
    err = ht_get_item(ht_get_root(symtable), node->data.class_type.name,
                      (void **)&table_node);
    if (err != NO_ERR) {
      return err;
    }
    if (table_node->type == AST_CLASS_DECLARATION) {
      break;
    }

    if (table_node->type != AST_TYPE_DECLARATION) {
      return OTHER_SEMANTIC_ERR;
    }

    data_type_node_t *copy = NULL;
    err =
        duplicate_type_node(table_node->data.type_declaration.data_type, &copy);
    if (err != NO_ERR) {
      return err;
    }

    free(table_node->data.type_declaration.name);

    node->data = copy->data;
    node->type = copy->type;

    free(copy);

    return analyse_type(node, symtable);
  case DATA_TYPE_LAMBDA:
    err = analyse_type(node->data.lambda_type.parameter_types, symtable);
    if (err != NO_ERR) {
      return err;
    }
    err = analyse_type(node->data.lambda_type.return_type, symtable);
    if (err != NO_ERR) {
      return err;
    }
    break;
  case DATA_TYPE_LIST:
    err = analyse_type(node->data.list_type.elements_type, symtable);
    if (err != NO_ERR) {
      return err;
    }
    break;
  case DATA_TYPE_PRIMITIVE:
    break;
  case DATA_TYPE_UNION:
    err = analyse_type(node->data.union_type.types, symtable);
    if (err != NO_ERR) {
      return err;
    }
    break;
  }

  if (node->next == NULL) {
    return NO_ERR;
  }

  return analyse_type(node->next, symtable);
}

#pragma region Expression

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
  err = ht_init(&current_scope, table_size);
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
  err = ht_init(&current_scope, table_size);
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

  if (node->type != AST_LITERAL) {
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

  if (are_data_types_compatible(table_node->data_type, match_type)) {
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

  return NO_ERR;
}

err_code_t analyse_match_case_parameter(ast_node_t *node,
                                        hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_PARAMETER) {
    return INTERNAL_ERR;
  }

  if (node->data.parameter.name == NULL ||
      node->data.parameter.data_type == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = analyse_type(node->data.parameter.data_type, symtable);
  if (err != NO_ERR) {
    return err;
  }

  err = ht_insert_item(symtable, node->data.parameter.name, (void **)&node);

  return err;
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
  err = ht_init(&current_scope, table_size);
  if (err == !NO_ERR) {
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
    if (err == NO_ERR &&
        !are_data_types_compatible(block->data_type, match_type)) {
      err = TYPE_COMPAT_ERR;
      goto case_err;
    }

  } else {
    err = analyse_expression(block, current_scope, explicit_data_type, true,
                             allow_return);
    if (err == NO_ERR &&
        !are_data_types_compatible(block->data_type, match_type)) {
      err = TYPE_COMPAT_ERR;
      goto case_err;
    }
  }
  ht_destruct(&current_scope);
  if (err != NO_ERR) {
    return err;
  }
  if (!are_data_types_compatible(block->data_type, match_type)) {
    return TYPE_COMPAT_ERR;
  }
  return NO_ERR;
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
    current = current->next;
  }

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
        }
        free(identifier);
      }
      free(first);
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

#pragma endregion

#pragma region Block

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
      if (ht_init(&current_scope, table_size) != NO_ERR) {
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

  err_code_t err = ht_insert_item(
      symtable, node->data.variable_declaration.name, (void **)&node);
  if (err != NO_ERR) {
    return err;
  }

  err = analyse_expression(expression, symtable, NULL, true, false);
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

  err = duplicate_type_node(expression->data_type, &node->data_type);

  return err;
}

err_code_t analyse_assignement(ast_node_t *node, hashtable_t *symtable) {
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

#pragma region Function && Lambda declarations

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
  if (ht_init(&current_scope, table_size) != NO_ERR) {
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
  if (node->data_type == NULL) {
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
  if (ht_init(&current_scope, table_size) != NO_ERR) {
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

#pragma endregion

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
      generator->data_type->type != DATA_TYPE_LIST) {
    return TYPE_COMPAT_ERR;
  }

  data_type_node_t *elements_type =
      generator->data_type->data.list_type.elements_type;

  if (iterator->data.parameter.data_type == NULL) {
    err =
        duplicate_type_node(elements_type, &iterator->data.parameter.data_type);
    if (err != NO_ERR) {
      return err;
    }
  } else {
    if (!are_data_types_compatible(iterator->data.parameter.data_type,
                                   elements_type)) {
      return TYPE_COMPAT_ERR;
    }
  }

  ast_node_t *current = node->data.for_do.guards;
  while (current != NULL) {
    err = analyse_expression(current, symtable, NULL, true, false);
    if (err != NO_ERR) {
      return err;
    }

    if (!are_data_types_identical(&node_boolean, current->data_type)) {
      return TYPE_COMPAT_ERR;
    }

    current = current->next;
  }

  // Create new scope
  hashtable_t *current_scope;
  err = ht_init(&current_scope, table_size);
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

  err = ht_insert_item(current_scope, iterator->data.parameter.name,
                       (void **)&iterator);
  if (err != NO_ERR) {
    ht_destruct(&current_scope);
    return err;
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
    case AST_CLASS_DECLARATION:
    case AST_TYPE_DECLARATION:
    case AST_EXTENSION_DECLARATION:
    case AST_MATCH:
    case AST_MATCH_CASE:
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
      err = analyse_expression(current, symtable, explicit_data_type,
                               current->next == NULL, true);
      if (err == NO_ERR) {
        last_type = current->data_type;
      }
      break;
    case AST_ASSIGNMENT:
      err = analyse_assignement(current, symtable);
      if (err == NO_ERR) {
        last_type = &node_unit;
      }
      break;
    case AST_LAMBDA_DECLARATION:
      err = analyse_lambda_declaration(current, ht_get_root(symtable));
      break;
    case AST_BLOCK:
      // Create new scope
      err = ht_init(&current_scope, table_size);
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

#pragma endregion

err_code_t analyse_top_block(ast_node_t *root, hashtable_t *symtable) {
  if (root == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (root->type != AST_BLOCK) {
    return OTHER_SEMANTIC_ERR;
  }

  err_code_t err = NO_ERR;
  ast_node_t *current = root->data.block.statement;
  ast_node_t *node = NULL;
  while (current != NULL) {
    switch (current->type) {
    case AST_FUNCTION_DECLARATION:
      err = analyse_function_declaration(current, symtable);
      if (err != NO_ERR) {
        return err;
      }
      break;
    case AST_EXTENSION_DECLARATION:
      node = current->data.extension_declaration.function_declarations;
      while (node != NULL) {
        err = analyse_function_declaration(node, symtable);
        if (err == NO_ERR) {
          node = node->next;
        } else {
          return err;
        }
      }
      break;
    case AST_TYPE_DECLARATION:
      err = analyse_type(current->data.type_declaration.data_type, symtable);
      break;
    case AST_CLASS_DECLARATION:
      node = current->data.class_declaration.attributes;
      while (node != NULL && err == NO_ERR) {
        err = analyse_type(node->data.parameter.data_type, symtable);
        if (err == NO_ERR) {
          node = node->next;
        } else {
          return err;
        }
      }
      break;
    default:
      return OTHER_SEMANTIC_ERR;
    }
    if (err == NO_ERR) {
      current = current->next;
    } else {
      return err;
    }
  }
  return NO_ERR;
}

#pragma endregion

err_code_t analyse(ast_node_t *root, hashtable_t *symtable,
                   char **main_identifier) {
  if (root == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  err_code_t err = collect_symbols(root, symtable, main_identifier);
  if (err != NO_ERR) {
    return err;
  }

  if (*main_identifier == NULL) {
    return UNDEF_SYMBOL_ERR;
  }

  err = analyse_top_block(root, symtable);
  if (err != NO_ERR) {
    return err;
  }

  return NO_ERR;
}
