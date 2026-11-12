/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file analyse_types.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of functions used for analysing types in semantic
analysis
 *
\*****************************************************************************/

#include "analyse_types.h"

err_code_t analyse_type_declaration(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_TYPE_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  if (node->data_type != NULL) {
    return NO_ERR;
  }

  if (node->_visited) {
    return TYPE_COMPAT_ERR;
  }

  node->_visited = true;
  err_code_t err =
      analyse_type(node->data.type_declaration.data_type, symtable);
  node->_visited = false;

  if (err != NO_ERR) {
    return err;
  }

  return duplicate_type_node(node->data.type_declaration.data_type,
                             &node->data_type);
}

err_code_t analyse_class_declaration(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    return INTERNAL_ERR;
  }

  if (node->type != AST_CLASS_DECLARATION) {
    return OTHER_SEMANTIC_ERR;
  }

  if (node->data_type != NULL) {
    return NO_ERR;
  }

  if (node->_visited) {
    return TYPE_COMPAT_ERR;
  }

  node->_visited = true;
  err_code_t err = NO_ERR;
  ast_node_t *attribute = node->data.class_declaration.attributes;
  while (attribute != NULL && err == NO_ERR) {
    err = analyse_type(attribute->data.parameter.data_type, symtable);
    if (err == NO_ERR) {
      attribute = attribute->next;
    } else {
      break;
    }
  }
  node->_visited = false;

  if (err != NO_ERR) {
    return err;
  }

  data_type_node_t *data_type =
      create_class_type_node(node->data.class_declaration.name);
  if (data_type == NULL) {
    return INTERNAL_ERR;
  }

  node->data_type = data_type;

  return NO_ERR;
}

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
      err = analyse_class_declaration(table_node, symtable);
      if (err != NO_ERR) {
        return err;
      }
      break;
    }

    if (table_node->type != AST_TYPE_DECLARATION) {
      return OTHER_SEMANTIC_ERR;
    }

    if (table_node->data_type == NULL) {
      err = analyse_type_declaration(table_node, symtable);
    }
    if (err != NO_ERR) {
      return err;
    }

    data_type_node_t *copy = NULL;
    err =
        duplicate_type_node(table_node->data.type_declaration.data_type, &copy);
    if (err != NO_ERR) {
      return err;
    }

    free(node->data.class_type.name);

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
