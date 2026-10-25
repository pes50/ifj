/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file data_type.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of Data type node
 *
\*****************************************************************************/

#include "data_type.h"
#include <stdlib.h>

t_data_type_node *create_type_node() {
  return calloc(1, sizeof(t_data_type_node));
}

void destroy_type_node(t_data_type_node *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case DATA_TYPE_CLASS:
    free(node->data.class_type.name);
    break;
  case DATA_TYPE_LAMBDA:
    destroy_type_node(node->data.lambda_type.parameter_types);
    destroy_type_node(node->data.lambda_type.return_type);
    break;
  case DATA_TYPE_LIST:
    destroy_type_node(node->data.list_type.elements_type);
    break;
  case DATA_TYPE_PRIMITIVE:
    break;
  case DATA_TYPE_UNION:
    destroy_type_node(node->data.union_type.types);
    break;
  }

  destroy_type_node(node->next);
}