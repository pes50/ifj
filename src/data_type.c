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
#include "err_codes.h"
#include "string_duplicate.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

t_data_type_node node_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = NULL,
};
t_data_type_node node_double = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_DOUBLE,
    .next = NULL,
};
t_data_type_node node_string = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_STRING,
    .next = NULL,
};
t_data_type_node node_unit = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_UNIT,
    .next = NULL,
};
t_data_type_node node_boolean = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_BOOLEAN,
    .next = NULL,
};
t_data_type_node node_any = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_ANY,
    .next = NULL,
};

t_data_type_node *create_type_node() {
  return calloc(1, sizeof(t_data_type_node));
}

void destroy_type_node(t_data_type_node *node) {
  if (node == NULL || node == (t_data_type_node *)&node_int ||
      node == (t_data_type_node *)&node_double ||
      node == (t_data_type_node *)&node_string ||
      node == (t_data_type_node *)&node_unit ||
      node == (t_data_type_node *)&node_boolean ||
      node == (t_data_type_node *)&node_any) {
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

  free(node);
}

t_data_type_node *create_class_type_node(char *name) {
  // Create node
  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_CLASS;
  node->data.class_type.name = string_duplicate(name);

  // Check if string_duplicate failed
  if (node->data.class_type.name == NULL) {
    destroy_type_node(node);
    return NULL;
  }

  return node;
}

t_data_type_node *create_lambda_type_node(t_data_type_node *parameter_types,
                                          t_data_type_node *return_type) {
  // Create node
  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_LAMBDA;
  node->data.lambda_type.parameter_types = parameter_types;
  node->data.lambda_type.return_type = return_type;

  return node;
}

t_data_type_node *create_list_type_node(t_data_type_node *element_type) {
  // Create node
  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_LIST;
  node->data.list_type.elements_type = element_type;

  return node;
}

t_data_type_node *create_primitive_type_node(t_primitive primitive) {
  // Create node
  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_PRIMITIVE;
  node->data.primitive_type = primitive;

  return node;
}

t_data_type_node *create_union_type_node(t_data_type_node *types) {
  // Create node
  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_UNION;
  node->data.union_type.types = types;

  return node;
}

err_code_t duplicate_type_node(t_data_type_node *original,
                               t_data_type_node **copy) {
  if (copy == NULL) {
    return INTERNAL_ERR;
  }

  if (original == NULL) {
    *copy = NULL;
    return NO_ERR;
  }

  t_data_type_node *node = create_type_node();
  if (node == NULL) {
    return INTERNAL_ERR;
  }

  node->type = original->type;
  switch (original->type) {
  case DATA_TYPE_CLASS:
    node->data.class_type.name =
        string_duplicate(original->data.class_type.name);
    if (node->data.class_type.name == NULL) {
      free(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_LAMBDA:
    if (duplicate_type_node(original->data.lambda_type.parameter_types,
                            &node->data.lambda_type.parameter_types) !=
        NO_ERR) {
      free(node);
      return INTERNAL_ERR;
    }
    if (duplicate_type_node(original->data.lambda_type.return_type,
                            &node->data.lambda_type.return_type) != NO_ERR) {
      free(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_LIST:
    if (duplicate_type_node(original->data.list_type.elements_type,
                            &node->data.list_type.elements_type) != NO_ERR) {
      free(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_PRIMITIVE:
    node->data.primitive_type = original->data.primitive_type;
    break;
  case DATA_TYPE_UNION:
    if (duplicate_type_node(original->data.union_type.types,
                            &node->data.union_type.types) != NO_ERR) {
      free(node);
      return INTERNAL_ERR;
    }
    break;
  }

  err_code_t err = duplicate_type_node(original->next, &node->next);
  if (err != NO_ERR) {
    free(node);
    return INTERNAL_ERR;
  }

  *copy = node;
  return NO_ERR;
}

bool union_contains_type(t_data_type_node *target_type,
                         t_data_type_node *union_type) {

  t_data_type_node target_no_next = *target_type;
  target_no_next.next = NULL;

  t_data_type_node *current = union_type;
  while (current != NULL) {

    t_data_type_node current_no_next = *current;
    current_no_next.next = NULL;

    if (are_data_types_identical(&target_no_next, &current_no_next)) {
      return true;
    }
    current = current->next;
  }
  return false;
}

bool are_unions_equivalent(t_data_type_node *union_a,
                           t_data_type_node *union_b) {
  t_data_type_node *current_a = union_a;
  while (current_a != NULL) {
    if (!union_contains_type(current_a, union_b)) {
      return false;
    }
    current_a = current_a->next;
  }

  t_data_type_node *curr_b = union_b;
  while (curr_b != NULL) {
    if (!union_contains_type(curr_b, union_a)) {
      return false;
    }
    curr_b = curr_b->next;
  }

  return true;
}

bool is_union_unnecessary(t_data_type_node *union_node,
                          t_data_type_node *target_type) {
  t_data_type_node *current = union_node->data.union_type.types;

  // Remove .next to ignore following nodes
  t_data_type_node target_no_next = *target_type;
  target_no_next.next = NULL;

  while (current != NULL) {
    t_data_type_node current_no_next = *current;
    current_no_next.next = NULL;

    if (!are_data_types_identical(&current_no_next, &target_no_next)) {
      return false;
    }
    current = current->next;
  }
  return true;
}

bool are_data_types_identical(t_data_type_node *data_type_a,
                              t_data_type_node *data_type_b) {
  // Both NULL or same pointer
  if (data_type_a == data_type_b) {
    return true;
  }

  // One is NULL
  if (data_type_a == NULL || data_type_b == NULL) {
    return false;
  }

  // Different node types
  if (data_type_a->type != data_type_b->type) {
    // A | A == A
    if (data_type_a->type == DATA_TYPE_UNION) {
      return is_union_unnecessary(data_type_a, data_type_b);
    }
    // A == A | A
    if (data_type_b->type == DATA_TYPE_UNION) {
      return is_union_unnecessary(data_type_b, data_type_a);
    }
    return false;
  }

  switch (data_type_a->type) {
  case DATA_TYPE_CLASS:
    if (strcmp(data_type_a->data.class_type.name,
               data_type_b->data.class_type.name) != 0) {
      return false;
    }
    break;
  case DATA_TYPE_LAMBDA:
    if (!are_data_types_identical(
            data_type_a->data.lambda_type.parameter_types,
            data_type_b->data.lambda_type.parameter_types) ||
        !are_data_types_identical(data_type_a->data.lambda_type.return_type,
                                  data_type_b->data.lambda_type.return_type)) {
      return false;
    }
    break;
  case DATA_TYPE_LIST:
    if (!are_data_types_identical(data_type_a->data.list_type.elements_type,
                                  data_type_b->data.list_type.elements_type)) {
      return false;
    }
    break;
  case DATA_TYPE_PRIMITIVE:
    if (data_type_a->data.primitive_type != data_type_b->data.primitive_type) {
      return false;
    }
    break;
  case DATA_TYPE_UNION:
    if (!are_unions_equivalent(data_type_a->data.union_type.types,
                               data_type_b->data.union_type.types)) {
      return false;
    }
    break;
  }

  return are_data_types_identical(data_type_a->next, data_type_b->next);
}