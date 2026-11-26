/************* Implementace překladače imperativního jazyka IFJ26 ************\
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
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->type != AST_TYPE_DECLARATION) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  if (node->data_type != NULL) {
    RETURN_ERR(NO_ERR);
  }

  if (node->_visited) {
    RETURN_ERR(TYPE_COMPAT_ERR);
  }

  node->_visited = true;
  err_code_t err =
      analyse_type(node->data.type_declaration.data_type, symtable);
  node->_visited = false;

  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  RETURN_ERR(duplicate_type_node(node->data.type_declaration.data_type,
                                 &node->data_type));
}

err_code_t analyse_class_declaration(ast_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (node->data_type != NULL) {
    RETURN_ERR(NO_ERR);
  }

  if (node->type != AST_CLASS_DECLARATION) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  ast_node_t *attribute_list = node->data.class_declaration.attribute_list;
  if (attribute_list == NULL || attribute_list->next != NULL ||
      attribute_list->type != AST_PARAMETER_LIST ||
      attribute_list->data.parameter_list.parameters == NULL) {
    RETURN_ERR(OTHER_SEMANTIC_ERR);
  }

  if (node->data_type != NULL) {
    RETURN_ERR(NO_ERR);
  }

  if (node->_visited) {
    RETURN_ERR(TYPE_COMPAT_ERR);
  }

  node->_visited = true;
  err_code_t err = NO_ERR;
  ast_node_t *attribute = attribute_list->data.parameter_list.parameters;
  size_t attribute_count = 0;
  while (attribute != NULL && err == NO_ERR) {
    err = analyse_type(attribute->data.parameter.data_type, symtable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    err = duplicate_type_node(attribute->data.parameter.data_type,
                              &attribute->data_type);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    attribute = attribute->next;
    attribute_count++;
  }
  node->data.class_declaration.attribute_count = attribute_count;
  node->_visited = false;

  err = duplicate_type_node(&node_unit, &attribute_list->data_type);

  if (err != NO_ERR) {
    RETURN_ERR(err);
  }

  data_type_node_t *data_type =
      create_class_type_node(node->data.class_declaration.name, node);
  if (data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  node->data_type = data_type;

  RETURN_ERR(NO_ERR);
}

err_code_t analyse_type(data_type_node_t *node, hashtable_t *symtable) {
  if (node == NULL || symtable == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  err_code_t err = NO_ERR;
  ast_node_t *table_node;
  switch (node->type) {
  case DATA_TYPE_CLASS:
    // identifier must be in the symtable
    err = ht_get_item(ht_get_root(symtable), node->data.class_type.name,
                      (void **)&table_node);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    if (table_node->type == AST_CLASS_DECLARATION) {
      err = analyse_class_declaration(table_node, symtable);
      if (err != NO_ERR) {
        RETURN_ERR(err);
      }
      break;
    }

    if (table_node->type != AST_TYPE_DECLARATION) {
      RETURN_ERR(OTHER_SEMANTIC_ERR);
    }

    if (table_node->data_type == NULL) {
      err = analyse_type_declaration(table_node, symtable);
    }
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    data_type_node_t *copy = NULL;
    err =
        duplicate_type_node(table_node->data.type_declaration.data_type, &copy);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }

    free(node->data.class_type.name);

    node->data = copy->data;
    node->type = copy->type;

    free(copy);

    RETURN_ERR(analyse_type(node, symtable));
  case DATA_TYPE_LAMBDA:
    err = analyse_type(node->data.lambda_type.parameter_types, symtable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    err = analyse_type(node->data.lambda_type.return_type, symtable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    break;
  case DATA_TYPE_LIST:
    err = analyse_type(node->data.list_type.elements_type, symtable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    break;
  case DATA_TYPE_PRIMITIVE:
    break;
  case DATA_TYPE_UNION:
    err = analyse_type(node->data.union_type.types, symtable);
    if (err != NO_ERR) {
      RETURN_ERR(err);
    }
    break;
  case DATA_TYPE_GENERIC:
  case DATA_TYPE_ANY:
  case DATA_TYPE_UNIT:
  case DATA_TYPE_RANGE:
  case DATA_TYPE_CURRIED_FUNCTION:
    break;
  }

  if (node->next == NULL) {
    RETURN_ERR(NO_ERR);
  }

  RETURN_ERR(analyse_type(node->next, symtable));
}
