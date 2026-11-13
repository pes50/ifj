/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file collect_symbols.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of collect_symbols function used in semantic analysis
 *
\*****************************************************************************/

#include "collect_symbols.h"
#include "string_concatenate_dot.h"

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
                            ast_node_t **main_node) {
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
    if (*main_node != NULL) {
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

    *main_node = node;
  } else {
    if (node->data.function_declaration.parameter_lists != NULL &&
        !is_parameter_list_chain(
            node->data.function_declaration.parameter_lists)) {
      return OTHER_SEMANTIC_ERR;
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
  ast_node_t *parameter = node->data.extension_declaration.parameter;

  if (parameter == NULL) {
    return OTHER_SEMANTIC_ERR;
  }

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
        free(symbol);
        return REDEF_SYMBOL_ERR;
      }
      free(symbol);
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

    ast_node_t *parameter_list_copy =
        create_parameter_list_node(parameter_copy);
    if (parameter_list_copy == NULL) {
      free(symbol);
      destroy_node(parameter_copy);
      return INTERNAL_ERR;
    }

    ast_node_t *parameter_lists =
        current->data.function_declaration.parameter_lists;
    if (parameter_lists == NULL) {
      current->data.function_declaration.parameter_lists = parameter_list_copy;
    } else {
      if (!is_parameter_list_chain(parameter_lists)) {
        free(symbol);
        destroy_node(parameter_list_copy);
        return OTHER_SEMANTIC_ERR;
      }
      parameter_list_copy->next = parameter_lists;
      current->data.function_declaration.parameter_lists = parameter_list_copy;
    }

    // Define symbol
    if (ht_insert_item(symtable, symbol, current) != NO_ERR) {
      free(symbol);
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
                           ast_node_t **main_node) {
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
      err = collect_function(current, symtable, main_node);
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
