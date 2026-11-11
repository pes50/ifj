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

char *primitive_to_string[] = {
    "Int", "Double", "String", "Unit", "Boolean", "Any",
};

data_type_node_t node_int = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_INT,
    .next = NULL,
};
data_type_node_t node_double = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_DOUBLE,
    .next = NULL,
};
data_type_node_t node_string = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_STRING,
    .next = NULL,
};
data_type_node_t node_unit = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_UNIT,
    .next = NULL,
};
data_type_node_t node_boolean = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_BOOLEAN,
    .next = NULL,
};
data_type_node_t node_any = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_ANY,
    .next = NULL,
};

data_type_node_t *create_type_node() {
  return calloc(1, sizeof(data_type_node_t));
}

void destroy_type_node(data_type_node_t *node) {
  if (node == NULL || node == &node_int || node == &node_double ||
      node == &node_string || node == &node_unit || node == &node_boolean ||
      node == &node_any) {
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

data_type_node_t *create_class_type_node(char *name) {
  // Create node
  data_type_node_t *node = create_type_node();
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

data_type_node_t *create_lambda_type_node(data_type_node_t *parameter_types,
                                          data_type_node_t *return_type) {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_LAMBDA;
  node->data.lambda_type.parameter_types = parameter_types;
  node->data.lambda_type.return_type = return_type;

  return node;
}

data_type_node_t *create_list_type_node(data_type_node_t *element_type) {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_LIST;
  node->data.list_type.elements_type = element_type;

  return node;
}

data_type_node_t *create_primitive_type_node(primitive_t primitive) {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_PRIMITIVE;
  node->data.primitive_type = primitive;

  return node;
}

data_type_node_t *create_union_type_node(data_type_node_t *types) {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_UNION;
  node->data.union_type.types = types;

  return node;
}

err_code_t duplicate_type_node(data_type_node_t *original,
                               data_type_node_t **copy) {
  if (copy == NULL) {
    return INTERNAL_ERR;
  }

  if (original == NULL) {
    *copy = NULL;
    return NO_ERR;
  }

  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return INTERNAL_ERR;
  }

  node->type = original->type;
  switch (original->type) {
  case DATA_TYPE_CLASS:
    node->data.class_type.name =
        string_duplicate(original->data.class_type.name);
    if (node->data.class_type.name == NULL) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_LAMBDA:
    if (duplicate_type_node(original->data.lambda_type.parameter_types,
                            &node->data.lambda_type.parameter_types) !=
        NO_ERR) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    if (duplicate_type_node(original->data.lambda_type.return_type,
                            &node->data.lambda_type.return_type) != NO_ERR) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_LIST:
    if (duplicate_type_node(original->data.list_type.elements_type,
                            &node->data.list_type.elements_type) != NO_ERR) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_PRIMITIVE:
    node->data.primitive_type = original->data.primitive_type;
    break;
  case DATA_TYPE_UNION:
    if (duplicate_type_node(original->data.union_type.types,
                            &node->data.union_type.types) != NO_ERR) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    break;
  }

  err_code_t err = duplicate_type_node(original->next, &node->next);
  if (err != NO_ERR) {
    destroy_type_node(node);
    return INTERNAL_ERR;
  }

  *copy = node;
  return NO_ERR;
}

bool union_contains_type(data_type_node_t *target_type,
                         data_type_node_t *union_type) {

  data_type_node_t target_no_next = *target_type;
  target_no_next.next = NULL;

  data_type_node_t *current = union_type;
  while (current != NULL) {

    data_type_node_t current_no_next = *current;
    current_no_next.next = NULL;

    if (are_data_types_identical(&target_no_next, &current_no_next)) {
      return true;
    }
    current = current->next;
  }
  return false;
}

bool is_union_subset(data_type_node_t *union_a, data_type_node_t *union_b) {
  data_type_node_t *current_a = union_a;
  while (current_a != NULL) {
    if (!union_contains_type(current_a, union_b)) {
      return false;
    }
    current_a = current_a->next;
  }
  return true;
}

bool are_unions_equivalent(data_type_node_t *union_a,
                           data_type_node_t *union_b) {
  return (is_union_subset(union_a, union_b) &&
          is_union_subset(union_b, union_a)) != 0;
}

bool is_union_unnecessary(data_type_node_t *union_node,
                          data_type_node_t *target_type) {
  data_type_node_t *current = union_node->data.union_type.types;

  // Remove .next to ignore following nodes
  data_type_node_t target_no_next = *target_type;
  target_no_next.next = NULL;

  while (current != NULL) {
    data_type_node_t current_no_next = *current;
    current_no_next.next = NULL;

    if (!are_data_types_identical(&current_no_next, &target_no_next)) {
      return false;
    }
    current = current->next;
  }
  return true;
}

bool are_data_types_identical(data_type_node_t *data_type_a,
                              data_type_node_t *data_type_b) {
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

bool are_data_types_compatible(data_type_node_t *data_type_a,
                               data_type_node_t *data_type_b) {

  if (are_data_types_identical(data_type_a, data_type_b)) {
    return true;
  }

  if (data_type_b->type == DATA_TYPE_UNION) {
    if (data_type_a->type == DATA_TYPE_UNION) {
      return is_union_subset(data_type_a, data_type_b);
    }
    return union_contains_type(data_type_a, data_type_b);
  }

  return (data_type_a->type == DATA_TYPE_PRIMITIVE &&
          data_type_a->data.primitive_type == PRIMITIVE_INT &&
          data_type_b->type == DATA_TYPE_PRIMITIVE &&
          data_type_b->data.primitive_type == PRIMITIVE_DOUBLE) != 0;
}

err_code_t type_to_string(data_type_node_t *data_type, char **result) {
  if (data_type->type == DATA_TYPE_PRIMITIVE) {
    *result = primitive_to_string[data_type->data.primitive_type];
  } else if (data_type->type == DATA_TYPE_CLASS) {
    *result = data_type->data.class_type.name;
  } else {
    // Type is not supported
    return OTHER_SEMANTIC_ERR;
  }
  return NO_ERR;
}

err_code_t duplicate_superset_type(data_type_node_t *data_type_node_a,
                                   data_type_node_t *data_type_node_b,
                                   data_type_node_t **data_type_node_c) {
  if (data_type_node_a == NULL || data_type_node_b == NULL) {
    return INTERNAL_ERR;
  }

  // TODO implement support for types other than Int

  if (are_data_types_identical(data_type_node_a, &node_int)) {
    return duplicate_type_node(data_type_node_b, data_type_node_c);
  }
  return duplicate_type_node(data_type_node_a, data_type_node_c);
}
