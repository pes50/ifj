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
    .type = DATA_TYPE_UNIT,
    .next = NULL,
};
data_type_node_t node_boolean = {
    .type = DATA_TYPE_PRIMITIVE,
    .data.primitive_type = PRIMITIVE_BOOLEAN,
    .next = NULL,
};
data_type_node_t node_any = {
    .type = DATA_TYPE_ANY,
    .next = NULL,
};
data_type_node_t node_generic_t = {
    .type = DATA_TYPE_GENERIC,
    .data.generic_type.name = "T",
    .next = NULL,
};
data_type_node_t node_range = {
    .type = DATA_TYPE_RANGE,
    .next = NULL,
};
data_type_node_t generic_list_node = {
    .type = DATA_TYPE_LIST,
    .data.list_type =
        {
            .elements_type = &node_generic_t,
        },
    .next = NULL,
};
data_type_node_t node_curried_function = {
    .type = DATA_TYPE_CURRIED_FUNCTION,
    .next = NULL,
};

data_type_node_t *create_type_node() {
  return calloc(1, sizeof(data_type_node_t));
}

void destroy_type_node(data_type_node_t *node) {
  if (node == NULL || node == &node_int || node == &node_double ||
      node == &node_string || node == &node_unit || node == &node_boolean ||
      node == &node_any || node == &node_generic_t || node == &node_range ||
      node == &generic_list_node || node == &node_curried_function) {
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
  case DATA_TYPE_ANY:
  case DATA_TYPE_UNIT:
  case DATA_TYPE_RANGE:
  case DATA_TYPE_CURRIED_FUNCTION:
    break;
  case DATA_TYPE_UNION:
    destroy_type_node(node->data.union_type.types);
    break;
  case DATA_TYPE_GENERIC:
    free(node->data.generic_type.name);
    break;
  }

  destroy_type_node(node->next);

  free(node);
}

data_type_node_t *create_class_type_node(const char *name,
                                         ast_node_t *declaration) {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_CLASS;
  node->data.class_type.name = string_duplicate(name);
  node->data.class_type.declaration = declaration;

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

data_type_node_t *create_unit_type_node() {
  // Create node
  data_type_node_t *node = create_type_node();
  if (node == NULL) {
    return NULL;
  }

  // Initialize
  node->type = DATA_TYPE_UNIT;

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
    node->data.class_type.declaration = original->data.class_type.declaration;
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
  case DATA_TYPE_GENERIC:
    node->data.generic_type.name =
        string_duplicate(original->data.generic_type.name);
    if (node->data.generic_type.name == NULL) {
      destroy_type_node(node);
      return INTERNAL_ERR;
    }
    break;
  case DATA_TYPE_ANY:
  case DATA_TYPE_UNIT:
  case DATA_TYPE_RANGE:
  case DATA_TYPE_CURRIED_FUNCTION:
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
  if (target_type == NULL || union_type == NULL) {
    return false;
  }

  data_type_node_t target_no_next = *target_type;
  target_no_next.next = NULL;

  data_type_node_t *current = union_type->data.union_type.types;
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
  if (union_a == NULL || union_a->type != DATA_TYPE_UNION) {
    return false;
  }

  data_type_node_t *current_a = union_a->data.union_type.types;
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
  if (current == NULL) {
    return false;
  }

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
      return is_union_unnecessary(data_type_a, data_type_b) &&
             are_data_types_identical(data_type_a->next, data_type_b->next);
    }
    // A == A | A
    if (data_type_b->type == DATA_TYPE_UNION) {
      return is_union_unnecessary(data_type_b, data_type_a) &&
             are_data_types_identical(data_type_a->next, data_type_b->next);
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
    if (!are_unions_equivalent(data_type_a, data_type_b)) {
      return false;
    }
    break;
  case DATA_TYPE_GENERIC:
    if (strcmp(data_type_a->data.generic_type.name,
               data_type_b->data.generic_type.name) != 0) {
      return false;
    }
    break;
  case DATA_TYPE_ANY:
  case DATA_TYPE_UNIT:
  case DATA_TYPE_RANGE:
    break;
  case DATA_TYPE_CURRIED_FUNCTION:
    // not supported
    return false;
  }

  return are_data_types_identical(data_type_a->next, data_type_b->next);
}

bool are_data_types_compatible(data_type_node_t *data_type_a,
                               data_type_node_t *data_type_b) {
  if (are_data_types_identical(&node_any, data_type_b)) {
    return true;
  }

  // both types are the same
  if (are_data_types_identical(data_type_a, data_type_b)) {
    return true;
  }

  if (data_type_a == NULL || data_type_b == NULL) {
    return false;
  }

  // target type is union
  if (data_type_b->type == DATA_TYPE_UNION) {
    if (data_type_a->type == DATA_TYPE_UNION) {
      return is_union_subset(data_type_a, data_type_b);
    }
    return union_contains_type(data_type_a, data_type_b);
  }

  // source type is union
  if (data_type_a->type == DATA_TYPE_UNION) {
    data_type_node_t *current = data_type_a->data.union_type.types;
    if (current == NULL) {
      return false;
    }

    while (current != NULL) {
      data_type_node_t current_no_next = *current;
      current_no_next.next = NULL;
      if (!are_data_types_compatible(&current_no_next, data_type_b)) {
        return false;
      }
      current = current->next;
    }
    return true;
  }

  // only other valid conversion is int->float
  return (data_type_a->type == DATA_TYPE_PRIMITIVE &&
          data_type_a->data.primitive_type == PRIMITIVE_INT &&
          data_type_b->type == DATA_TYPE_PRIMITIVE &&
          data_type_b->data.primitive_type == PRIMITIVE_DOUBLE) != 0;
}

err_code_t type_to_string(data_type_node_t *data_type, char **result) {
  char *src = NULL;
  switch (data_type->type) {
  case DATA_TYPE_PRIMITIVE:
    switch (data_type->data.primitive_type) {
    case PRIMITIVE_INT:
      src = "Int";
      break;
    case PRIMITIVE_DOUBLE:
      src = "Double";
      break;
    case PRIMITIVE_STRING:
      src = "String";
      break;
    case PRIMITIVE_BOOLEAN:
      src = "Boolean";
      break;
    }
    break;
  case DATA_TYPE_UNIT:
    src = "Unit";
    break;
  case DATA_TYPE_CLASS:
    src = data_type->data.class_type.name;
    break;
  case DATA_TYPE_LIST:
    src = "List$";
    break;
  case DATA_TYPE_RANGE:
    src = "$Range";
    break;
  default:
    // Type is not supported
    return TYPE_COMPAT_ERR;
  }

  char *duplicated = string_duplicate(src);
  if (duplicated == NULL) {
    return INTERNAL_ERR;
  }
  *result = duplicated;

  return NO_ERR;
}
