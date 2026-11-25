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
#include "string_concatenate_dollar.h"
#include <string.h>

#pragma region Function call

err_code_t analyse_argument_list(ast_node_t *parameter_list,
                                 ast_node_t *argument_list,
                                 hashtable_t *symtable,
                                 data_type_node_t *target_type) {

  if (symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (argument_list != NULL && argument_list->type != AST_ARGUMENT_LIST) {
    return ARG_MISMATCH_ERR;
  }

  if (parameter_list != NULL && parameter_list->type != AST_PARAMETER_LIST) {
    return ARG_MISMATCH_ERR;
  }

  ast_node_t *current_argument_list = argument_list;
  ast_node_t *current_parameter_list = parameter_list;

  ast_node_t **current_argument =
      argument_list != NULL ? &argument_list->data.argument_list.arguments
                            : NULL;
  ast_node_t *current_parameter =
      parameter_list != NULL ? parameter_list->data.parameter_list.parameters
                             : NULL;

  // dot function call
  if (target_type != NULL) {
    // All methods should have parameter list containing $target type
    if (current_parameter == NULL || current_parameter->next != NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    bool is_generic_list = are_data_types_identical(
        &generic_list_node, current_parameter->data.parameter.data_type);

    if (is_generic_list) {
      if (target_type->type != DATA_TYPE_LIST) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }
    } else
      // Check $target type
      if (!are_data_types_compatible(
              target_type, current_parameter->data.parameter.data_type)) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }

    // Current parameter list is correct
    current_parameter = NULL;

    // Update pointers so the rest can be analysed as normal function call
    current_parameter_list = current_parameter_list->next;
    if (current_parameter_list != NULL) {
      // Function has a parameter list, set current_parameter to the first
      // parameter
      current_parameter =
          current_parameter_list->data.parameter_list.parameters;
    }

    // Handle filter, takeWhile and dropWhile
    if (target_type->type == DATA_TYPE_LIST && current_parameter != NULL &&
        current_parameter->data.parameter.data_type->type == DATA_TYPE_LAMBDA &&
        current_argument != NULL && *current_argument != NULL) {

      err_code_t err =
          analyse_expression(*current_argument, symtable,
                             current_parameter->data.parameter.data_type->data
                                 .lambda_type.return_type,
                             true, false);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      if ((*current_argument)->data_type->type != DATA_TYPE_LAMBDA) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }

      if (!are_data_types_compatible(
              (*current_argument)->data_type->data.lambda_type.parameter_types,
              target_type->data.list_type.elements_type)) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }

      // Move pointers
      current_parameter = current_parameter->next;
      current_argument = &(*current_argument)->next;
      if (current_parameter == NULL) {
        current_parameter_list = current_parameter_list->next;
        if (current_parameter_list != NULL) {
          current_parameter =
              current_parameter_list->data.parameter_list.parameters;
        }
      }

      if (*current_argument == NULL) {
        err =
            duplicate_type_node(&node_unit, &current_argument_list->data_type);
        if (err != NO_ERR) {
          RETURN_ERR(err);
        }
        current_argument_list = current_argument_list->next;
        current_argument =
            current_argument_list != NULL
                ? &current_argument_list->data.argument_list.arguments
                : NULL;
      }
    }
  }

  err_code_t err = NO_ERR;
  while (current_argument_list != NULL && current_parameter_list != NULL) {
    while (*current_argument != NULL && current_parameter != NULL) {
      err = analyse_expression(*current_argument, symtable, NULL, true, false);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      if (!are_data_types_compatible(
              (*current_argument)->data_type,
              current_parameter->data.parameter.data_type)) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }
      current_argument = &(*current_argument)->next;
      current_parameter = current_parameter->next;
    }

    while (current_parameter != NULL) {
      if (current_parameter->data.parameter.optional) {
        if (current_parameter->data.parameter.create_default == NULL) {
          RETURN_ERR(INTERNAL_ERR);
        }

        ast_node_t *optional_argument = NULL;
        err = current_parameter->data.parameter.create_default(
            &optional_argument);
        if (err != NO_ERR) {
          RETURN_ERR(err);
        }

        *current_argument = optional_argument;
        current_argument = &optional_argument->next;
        current_parameter = current_parameter->next;
      } else {
        break;
      }
    }

    if (*current_argument != NULL || current_parameter != NULL) {
      RETURN_ERR(ARG_MISMATCH_ERR);
    }

    err = duplicate_type_node(&node_unit, &current_argument_list->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(INTERNAL_ERR);
    }

    current_argument_list = current_argument_list->next;
    current_parameter_list = current_parameter_list->next;

    current_argument =
        current_argument_list != NULL
            ? &current_argument_list->data.argument_list.arguments
            : NULL;
    if (current_parameter_list != NULL) {
      current_parameter =
          current_parameter_list->data.parameter_list.parameters;
    }
  }

  if (current_argument_list != NULL || current_parameter_list != NULL) {
    RETURN_ERR(ARG_MISMATCH_ERR);
  }

  RETURN_ERR(NO_ERR);
}

err_code_t analyse_lambda_call(ast_node_t *node, hashtable_t *symtable,
                               data_type_node_t *data_type) {
  if (node == NULL || node->type != AST_FUNCTION_CALL || symtable == NULL ||
      data_type == NULL || data_type->type != DATA_TYPE_LAMBDA) {
    RETURN_ERR(INTERNAL_ERR);
  }

  data_type_node_t *current_type = data_type->data.lambda_type.parameter_types;
  ast_node_t *argument_list = node->data.function_call.argument_lists;
  if (argument_list == NULL || argument_list->next != NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }
  ast_node_t *current_argument = argument_list->data.argument_list.arguments;

  while (current_type != NULL && current_argument != NULL) {
    err_code_t err =
        analyse_expression(current_argument, symtable, NULL, true, false);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    data_type_node_t *next = current_type->next;
    current_type->next = NULL;
    if (!are_data_types_compatible(current_argument->data_type, current_type)) {
      current_type->next = next;
      RETURN_ERR(TYPE_COMPAT_ERR);
    }
    current_type->next = next;

    current_type = current_type->next;
    current_argument = current_argument->next;
  }

  if (current_type != NULL || current_argument != NULL) {
    RETURN_ERR(ARG_MISMATCH_ERR);
  }

  err_code_t err = duplicate_type_node(data_type->data.lambda_type.return_type,
                                       &node->data_type);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(duplicate_type_node(&node_unit, &argument_list->data_type));
}

err_code_t replace_generic_type(data_type_node_t **target_type,
                                data_type_node_t **expression_type) {
  if (target_type == NULL || *target_type == NULL || expression_type == NULL ||
      *expression_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if ((*target_type)->type != DATA_TYPE_LIST) {
    RETURN_ERR(NO_ERR);
  }

  if (are_data_types_identical(&generic_list_node, *expression_type)) {
    destroy_type_node(*expression_type);
    *expression_type = NULL;
    RETURN_ERR(duplicate_type_node(*target_type, expression_type));
  }
  if (are_data_types_identical(&node_generic_t, *expression_type)) {
    destroy_type_node(*expression_type);
    *expression_type = NULL;
    RETURN_ERR(duplicate_type_node((*target_type)->data.list_type.elements_type,
                                   expression_type));
  }

  RETURN_ERR(NO_ERR);
}

err_code_t analyse_function_call(char *identifier, ast_node_t *node,
                                 hashtable_t *symtable,
                                 data_type_node_t *target_type) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->type != AST_FUNCTION_CALL && node->type != AST_DOT_FUNCTION_CALL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *table_node;
  err_code_t err = ht_get_item(symtable, identifier, (void **)&table_node);
  if (err != NO_ERR) {
    // There is no macro because it could be Any.function call
    return err;
  }

  // Handle lambda parameter
  if (table_node->type == AST_PARAMETER) {
    if (table_node->data.parameter.data_type != NULL) {
      if (table_node->data.parameter.data_type->type == DATA_TYPE_LAMBDA) {
        node->data.function_call.calling = table_node;
        RETURN_ERR(analyse_lambda_call(node, symtable,
                                       table_node->data.parameter.data_type));
      }
    }
    return OTHER_SEMANTIC_ERR;
  }

  if (table_node->type == AST_VARIABLE_DECLARATION) {
    data_type_node_t *data_type = table_node->data_type;

    if (data_type == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    // Handle lambda variable
    if (data_type->type == DATA_TYPE_LAMBDA) {
      node->data.function_call.calling = table_node;
      RETURN_ERR(analyse_lambda_call(node, symtable, data_type));
    }

    // Handle list element access
    if (data_type->type == DATA_TYPE_LIST) {
      ast_node_t *argument_list = node->data.function_call.argument_lists;
      if (argument_list == NULL || argument_list->next != NULL) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }
      ast_node_t *argument = argument_list->data.argument_list.arguments;
      if (argument == NULL || argument->next != NULL) {
        RETURN_ERR(ARG_MISMATCH_ERR);
      }

      node->data.function_call.calling = table_node;

      err_code_t err =
          analyse_expression(argument, symtable, NULL, true, false);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      if (!are_data_types_identical(&node_int, argument->data_type)) {
        RETURN_ERR(TYPE_COMPAT_ERR);
      }

      err = duplicate_type_node(&node_unit, &argument_list->data_type);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      RETURN_ERR(duplicate_type_node(data_type->data.list_type.elements_type,
                                     &node->data_type));
    }
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  // Handle list constructor
  if (table_node->type == AST_LIST_CONSTRUCTOR) {
    ast_node_t *argument_list = node->data.function_call.argument_lists;
    if (argument_list == NULL || argument_list->next != NULL) {
      RETURN_ERR(ARG_MISMATCH_ERR);
    }

    if (argument_list->type != AST_ARGUMENT_LIST) {
      RETURN_ERR(INTERNAL_ERR);
    }

    ast_node_t *current = argument_list->data.argument_list.arguments;
    node->data.function_call.calling = table_node;

    // No arguments, type of elements is unknown
    if (current == NULL) {
      if (target_type == NULL) {
        RETURN_ERR(TYPE_COMPAT_ERR);
      }
      err = duplicate_type_node(&node_unit, &argument_list->data_type);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      RETURN_ERR(duplicate_type_node(target_type, &node->data_type));
    }

    data_type_node_t *data_type = NULL;
    while (current != NULL) {
      err = analyse_expression(current, symtable, NULL, true, false);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }

      if (data_type == NULL) {
        data_type = current->data_type;
      } else {
        bool new_to_old =
            are_data_types_compatible(current->data_type, data_type);
        bool old_to_new =
            are_data_types_compatible(data_type, current->data_type);
        if (new_to_old && !old_to_new) {
          data_type = current->data_type;
        } else if (!new_to_old && old_to_new) {
          RETURN_ERR(TYPE_COMPAT_ERR);
        } else if (!new_to_old && !old_to_new) {
          RETURN_ERR(TYPE_COMPAT_ERR);
        }
      }
      current = current->next;
    }

    err = duplicate_type_node(&node_unit, &argument_list->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    data_type_node_t *list_element_type = NULL;
    err = duplicate_type_node(data_type, &list_element_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    data_type_node_t *list_type = create_list_type_node(list_element_type);
    if (list_type == NULL) {
      destroy_type_node(list_element_type);
      return INTERNAL_ERR;
    }

    node->data_type = list_type;

    RETURN_ERR(NO_ERR);
  }

  if (table_node->type != AST_FUNCTION_DECLARATION &&
      table_node->type != AST_CLASS_DECLARATION) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  // Handle constructors
  if (table_node->type == AST_CLASS_DECLARATION) {
    if (node->type == AST_DOT_FUNCTION_CALL) {
      RETURN_ERR(OTHER_SEMANTIC_ERR);
    }

    err = analyse_class_declaration(table_node, ht_get_root(symtable));
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = analyse_argument_list(
        table_node->data.class_declaration.attribute_list,
        node->data.function_call.argument_lists, symtable, target_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = duplicate_type_node(table_node->data_type, &node->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    node->data.function_call.calling = table_node;
  } else {
    // Handle functions
    bool target_is_static = false;
    if (node->type == AST_DOT_FUNCTION_CALL) {
      ast_node_t *target = node->data.dot_function_call.target;
      target_is_static =
          (target != NULL && target->type == AST_IDENTIFIER &&
           target->data.identifier.calling != NULL &&
           target->data.identifier.calling->type == AST_LIST_CONSTRUCTOR);
    }

    if (target_is_static != table_node->data.function_declaration.is_static) {
      RETURN_ERR(OTHER_SEMANTIC_ERR);
    }

    err = analyse_function_declaration(table_node, ht_get_root(symtable));
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (table_node->data_type != NULL &&
        table_node->data_type->type == DATA_TYPE_LAMBDA &&
        node->type == AST_FUNCTION_CALL &&
        node->data.function_call.argument_lists == NULL) {
      RETURN_ERR(duplicate_type_node(table_node->data_type, &node->data_type));
    }

    err = analyse_argument_list(
        table_node->data.function_declaration.parameter_lists,
        node->type == AST_FUNCTION_CALL
            ? node->data.function_call.argument_lists
            : node->data.dot_function_call.argument_lists,
        symtable,
        (node->type == AST_DOT_FUNCTION_CALL &&
         !table_node->data.function_declaration.is_static)
            ? target_type
            : NULL);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = duplicate_type_node(table_node->data.function_declaration.return_type,
                              &node->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
  }

  if (node->type == AST_FUNCTION_CALL) {
    node->data.function_call.calling = table_node;
  } else {
    node->data.dot_function_call.calling = table_node;
  }

  return NO_ERR;
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
    RETURN_ERR(err);
  }

  if (!are_data_types_identical(condition->data_type, &node_boolean)) {
    return TYPE_COMPAT_ERR;
  }

  // Create new scope
  hashtable_t *current_scope;
  err = ht_init(&current_scope, local_scope_table_size);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  current_scope->parent = symtable;

  if (true_branch->type == AST_BLOCK) {
    err = analyse_block(true_branch, current_scope, explicit_data_type,
                        allow_return);
  } else {
    err = analyse_expression(true_branch, current_scope, explicit_data_type,
                             is_last, allow_return);
  }
  ht_destruct(&current_scope);

  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (false_branch == NULL) {
    if (is_last && explicit_data_type != NULL &&
        !are_data_types_identical(explicit_data_type, &node_unit)) {
      RETURN_ERR(TYPE_COMPAT_ERR);
    }
    RETURN_ERR(duplicate_type_node(&node_unit, &node->data_type));
  }

  // Create new scope
  err = ht_init(&current_scope, local_scope_table_size);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  current_scope->parent = symtable;

  if (false_branch->type == AST_BLOCK) {
    err = analyse_block(false_branch, current_scope, explicit_data_type,
                        allow_return);
  } else {
    err = analyse_expression(false_branch, current_scope, explicit_data_type,
                             is_last, allow_return);
  }
  ht_destruct(&current_scope);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (are_data_types_identical(explicit_data_type, &node_unit) || !is_last) {
    // Do not check type compatibility, values will be discarded
    RETURN_ERR(duplicate_type_node(&node_unit, &node->data_type));
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
    if (are_data_types_identical(true_branch->data_type, &node_int)) {
      err = duplicate_type_node(false_branch->data_type, &node->data_type);
    } else {
      err = duplicate_type_node(true_branch->data_type, &node->data_type);
    }

    RETURN_ERR(err);
  }

  // Both branches must have compatible type with the return type
  if (!are_data_types_compatible(true_branch->data_type, explicit_data_type) ||
      !are_data_types_compatible(false_branch->data_type, explicit_data_type)) {
    return TYPE_COMPAT_ERR;
  }

  err = duplicate_type_node(explicit_data_type, &node->data_type);
  RETURN_ERR(err);
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
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }
  err = analyse_expression(right, symtable, NULL, true, false);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (!are_data_types_compatible(left->data_type, right->data_type) &&
      !are_data_types_compatible(right->data_type, left->data_type)) {
    return TYPE_COMPAT_ERR;
  }
  // TODO check type compatibility
  switch (node->data.binary_operation.op) {
    // Operators
  case TOK_BINARY_PLUS:
    if (are_data_types_identical(left->data_type, &node_int)) {
      err = duplicate_type_node(right->data_type, &node->data_type);
    } else {
      err = duplicate_type_node(left->data_type, &node->data_type);
    }
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (!are_data_types_compatible(node->data_type, &node_double) &&
        !are_data_types_compatible(node->data_type, &node_string)) {
      return TYPE_COMPAT_ERR;
    }
    break;
  case TOK_BINARY_MINUS:
  case TOK_MUL:
  case TOK_DIV:
    if (are_data_types_identical(left->data_type, &node_int)) {
      err = duplicate_type_node(right->data_type, &node->data_type);
    } else {
      err = duplicate_type_node(left->data_type, &node->data_type);
    }
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (!are_data_types_compatible(node->data_type, &node_double)) {
      return TYPE_COMPAT_ERR;
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
    return duplicate_type_node(&node_boolean, &node->data_type);
  case TOK_CONCAT_LIST:
    if (left->data_type->type != DATA_TYPE_LIST ||
        right->data_type->type != DATA_TYPE_LIST) {
      RETURN_ERR(TYPE_COMPAT_ERR);
    }
    if (are_data_types_compatible(left->data_type, right->data_type)) {
      RETURN_ERR(duplicate_type_node(right->data_type, &node->data_type));
    }
    if (are_data_types_compatible(right->data_type, left->data_type)) {
      RETURN_ERR(duplicate_type_node(left->data_type, &node->data_type));
    }
    RETURN_ERR(TYPE_COMPAT_ERR);
  default:
    return INTERNAL_ERR;
  }

  return NO_ERR;
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
    RETURN_ERR(err);
  }

  if (table_node->type != AST_CLASS_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  if (!are_data_types_compatible(table_node->data_type, match_type)) {
    return TYPE_COMPAT_ERR;
  }

  ast_node_t *attribute_list =
      table_node->data.class_declaration.attribute_list;
  if (attribute_list == NULL || attribute_list->type != AST_PARAMETER_LIST) {
    return OTHER_SEMANTIC_ERR;
  }

  ast_node_t *argument_list = node->data.function_call.argument_lists;
  if (argument_list == NULL || argument_list->type != AST_ARGUMENT_LIST) {
    return ARG_MISMATCH_ERR;
  }

  ast_node_t *attribute = attribute_list->data.parameter_list.parameters;
  ast_node_t *argument = argument_list->data.argument_list.arguments;
  while (attribute != NULL && argument != NULL) {
    char *argument_name = argument->data.identifier.name;
    if (argument->type != AST_IDENTIFIER || argument_name == NULL) {
      return OTHER_SEMANTIC_ERR;
    }

    data_type_node_t *attribute_type = NULL;
    err = duplicate_type_node(attribute->data_type, &attribute_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    argument->type = AST_PARAMETER;
    argument->data.parameter.name = argument_name;
    argument->data.parameter.data_type = attribute_type;

    err = duplicate_type_node(argument->data.parameter.data_type,
                              &argument->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = ht_insert_item(symtable, argument->data.identifier.name, argument);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    attribute = attribute->next;
    argument = argument->next;
  }
  if (attribute != NULL || argument != NULL) {
    return ARG_MISMATCH_ERR;
  }

  err = duplicate_type_node(&node_unit, &argument_list->data_type);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  node->data.function_call.calling = table_node;

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
    RETURN_ERR(err);
  }

  err = ht_insert_item(symtable, name, node);

  if (err != NO_ERR) {
    RETURN_ERR(err);
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
    RETURN_ERR(err);
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

    err = duplicate_type_node(&node_any, &pattern->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
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
    RETURN_ERR(err);
  }

  return duplicate_type_node(block->data_type, &node->data_type);
case_err:
  ht_destruct(&current_scope);
  RETURN_ERR(err);
}

err_code_t analyse_match(ast_node_t *node, hashtable_t *symtable,
                         data_type_node_t *explicit_data_type,
                         bool allow_return) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  ast_node_t *cases = node->data.match.cases;
  ast_node_t *expression = node->data.match.expression;

  if (node->type != AST_MATCH || cases == NULL || expression == NULL) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  err_code_t err = analyse_expression(expression, symtable, NULL, true, false);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  int default_count = 0;
  ast_node_t *current = cases;
  data_type_node_t *data_type = NULL;
  while (current != NULL) {
    err =
        analyse_match_case(current, symtable, explicit_data_type, allow_return,
                           &default_count, expression->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (data_type == NULL) {
      data_type = current->data_type;
    } else {
      if (!are_data_types_compatible(current->data_type, data_type)) {
        if (are_data_types_compatible(data_type, current->data_type)) {
          data_type = current->data_type;
        } else {
          err = TYPE_COMPAT_ERR;
        }
      }
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
    }
    current = current->next;
  }

  err = duplicate_type_node(data_type, &node->data_type);
  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  if (default_count != 1) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  RETURN_ERR(NO_ERR);
}

#pragma endregion

err_code_t analyse_expression(ast_node_t *node, hashtable_t *symtable,
                              data_type_node_t *explicit_data_type,
                              bool is_last, bool allow_return) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->data_type != NULL) {
    destroy_type_node(node->data_type);
    node->data_type = NULL;
  }

  err_code_t err = NO_ERR;
  ast_node_t *table_node = NULL;
  switch (node->type) {
  case AST_FUNCTION_DECLARATION:
  case AST_PARAMETER_LIST:
  case AST_PARAMETER:
  case AST_ARGUMENT_LIST:
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
  case AST_LIST_CONSTRUCTOR:
  case AST_BUILTIN_BODY:
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  case AST_LAMBDA_DECLARATION:
    RETURN_ERR(analyse_lambda_declaration(node, symtable, explicit_data_type));
  case AST_FUNCTION_CALL:
    RETURN_ERR(analyse_function_call(node->data.function_call.name, node,
                                     symtable, explicit_data_type));
  case AST_DOT_FUNCTION_CALL: {
    ast_node_t *target = node->data.dot_function_call.target;
    err = analyse_expression(target, symtable, NULL, true, false);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    // Attribute access
    if (target->type == AST_IDENTIFIER) {
      ast_node_t *calling = target->data.identifier.calling;
      if (calling != NULL && calling->data_type != NULL &&
          calling->data_type->type == DATA_TYPE_CLASS) {
        ast_node_t *class_declaration =
            calling->data_type->data.class_type.declaration;
        if (class_declaration != NULL &&
            class_declaration->type == AST_CLASS_DECLARATION) {
          ast_node_t *attribute =
              class_declaration->data.class_declaration.attribute_list->data
                  .parameter_list.parameters;
          // Search for the attribute
          size_t attribute_index = 0;
          while (attribute != NULL) {
            if (strcmp(attribute->data.parameter.name,
                       node->data.dot_function_call.name) == 0) {
              // Found
              node->data.dot_function_call.attribute_index =
                  attribute_index + 1;

              RETURN_ERR(duplicate_type_node(
                  attribute->data.parameter.data_type, &node->data_type));
            }

            attribute_index++;
            attribute = attribute->next;
          }
          // Unknown attribute
          RETURN_ERR(OTHER_SEMANTIC_ERR);
        }
      }
    }

    bool target_is_static =
        (target->type == AST_IDENTIFIER &&
         target->data.identifier.calling != NULL &&
         target->data.identifier.calling->type == AST_LIST_CONSTRUCTOR);

    char *first = NULL;
    err = type_to_string(target->data_type, &first);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    char *identifier =
        string_concatenate_dollar(first, node->data.dot_function_call.name);
    free(first);
    if (identifier == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }
    err = analyse_function_call(identifier, node, symtable, target->data_type);
    free(identifier);
    if (err != UNDEF_SYMBOL_ERR || target_is_static) {
      if (err == NO_ERR) {
        RETURN_ERR(replace_generic_type(&target->data_type, &node->data_type));
      }
      RETURN_ERR(err);
    }

    // Try once more with Any
    identifier =
        string_concatenate_dollar("Any", node->data.dot_function_call.name);
    if (identifier == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }
    err = analyse_function_call(identifier, node, symtable,
                                node->data.dot_function_call.target->data_type);
    free(identifier);
    RETURN_ERR(err);
  }
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
  case AST_IDENTIFIER: {
    char *identifier = node->data.identifier.name;
    err = ht_get_item(symtable, identifier, (void **)&table_node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    if (table_node->data_type == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    if (table_node->type == AST_LIST_CONSTRUCTOR) {
      node->data.identifier.calling = table_node;
      err = duplicate_type_node(&generic_list_node, &node->data_type);
      break;
    }

    if (table_node->type == AST_FUNCTION_DECLARATION) {
      node->type = AST_FUNCTION_CALL;
      node->data.function_call.argument_lists = NULL;
      node->data.function_call.name = identifier;
      node->data.function_call.calling = table_node;
      RETURN_ERR(analyse_function_call(identifier, node, symtable, NULL));
    }

    if (table_node->type == AST_VARIABLE_DECLARATION ||
        table_node->type == AST_PARAMETER) {
      node->data.identifier.calling = table_node;
      RETURN_ERR(duplicate_type_node(table_node->data_type, &node->data_type));
    }
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }
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

  RETURN_ERR(err);
}
