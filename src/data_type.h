/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file data_type.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of Data type node
 *
\*****************************************************************************/

#pragma once

#include "err_codes.h"
#include <stdbool.h>

/**
 * @brief Enumeration of all node types
 */
typedef enum {
  DATA_TYPE_CLASS,
  DATA_TYPE_LAMBDA,
  DATA_TYPE_LIST,
  DATA_TYPE_PRIMITIVE,
  DATA_TYPE_UNION,
} dt_type_t;

/**
 * @brief Enumeration of all primitives
 */
typedef enum {
  PRIMITIVE_INT,
  PRIMITIVE_DOUBLE,
  PRIMITIVE_STRING,
  PRIMITIVE_UNIT,
  PRIMITIVE_BOOLEAN,
  PRIMITIVE_ANY,
} primitive_t;

/**
 * @brief Struct for a Data type node
 */
typedef struct data_type_node_t {
  dt_type_t type;

  union {
    struct {
      char *name;
    } class_type;

    struct {
      struct data_type_node_t *parameter_types;
      struct data_type_node_t *return_type;
    } lambda_type;

    struct {
      struct data_type_node_t *elements_type;
    } list_type;

    primitive_t primitive_type;

    struct {
      struct data_type_node_t *types;
    } union_type;
  } data;

  struct data_type_node_t *next;
} data_type_node_t;

extern data_type_node_t node_int;
extern data_type_node_t node_double;
extern data_type_node_t node_string;
extern data_type_node_t node_unit;
extern data_type_node_t node_boolean;
extern data_type_node_t node_any;

/**
 * @brief Allocates a new Data type node
 *
 * @return A pointer to the allocated node or NULL if allocation fails
 */
data_type_node_t *create_type_node();

/**
 * @brief Recursively deallocates memory for a Data type node and its
 * descendants
 *
 * @param node A pointer to the Data type node to destroy. If NULL, the function
 * does nothing.
 */
void destroy_type_node(data_type_node_t *node);

/**
 * @brief Creates a data type node for class
 *
 * @param name Class name
 *
 * @return A pointer to created node or NULL if failed
 */
data_type_node_t *create_class_type_node(char *name);

/**
 * @brief Creates a data type node for lambda function
 *
 * @param parameter_types A pointer to a data type node containing the first
 * parameter type
 * @param return_type A pointer to a data type node representing the return type
 *
 * @return A pointer to created node or NULL if failed
 */
data_type_node_t *create_lambda_type_node(data_type_node_t *parameter_types,
                                          data_type_node_t *return_type);

/**
 * @brief Creates a data type node for list
 *
 * @param element_type A pointer to a data type node representing the element
 * type
 *
 * @return A pointer to created node or NULL if failed
 */
data_type_node_t *create_list_type_node(data_type_node_t *element_type);

/**
 * @brief Creates a data type node for a primitive type
 *
 * @param primitive Primitive that this node should represent
 *
 * @return A pointer to created node or NULL if failed
 */
data_type_node_t *create_primitive_type_node(primitive_t primitive);

/**
 * @brief Creates a data type node for union
 *
 * @param types A pointer to the first type in the union
 *
 * @return A pointer to created node or NULL if failed
 */
data_type_node_t *create_union_type_node(data_type_node_t *types);

/**
 * @brief Creates a deep copy of a type node
 *
 * @param original A pointer to the node to copy
 * @param copy [out] A pointer where to store the result
 *
 * @return NO_ERR on successful copy
 */
err_code_t duplicate_type_node(data_type_node_t *original,
                               data_type_node_t **copy);

/**
 * @brief Check if target type is contained within an union
 *
 * @param target_type A pointer to the type to search for
 * @param union_type A pointer to the union to search
 */
bool union_contains_type(data_type_node_t *target_type,
                         data_type_node_t *union_type);

/**
 * @brief Check if two unions represent the same type, regardless of order, for
 * example: A | B == B | A | A
 *
 * @param union_a A pointer to the first union to check
 * @param union_b A pointer to the second union to check
 */
bool are_unions_equivalent(data_type_node_t *union_a,
                           data_type_node_t *union_b);

/**
 * @brief Checks if union can be simplified to a target_type, for example: A | A
 * to A
 *
 * @param union_node A pointer to the union to check
 * @param target_type A pointer to the type to simplify to
 */
bool is_union_unnecessary(data_type_node_t *union_node,
                          data_type_node_t *target_type);

/**
 * @brief Checks if two data type nodes represent the same type
 *
 * @param data_type_a A pointer to the first node to check
 * @param data_type_b A pointer to the second node to check
 */
bool are_data_types_identical(data_type_node_t *data_type_a,
                              data_type_node_t *data_type_b);

/**
 * @brief Checks if data_type_a is identical or a subset of data_type_b
 *
 * @param data_type_a A pointer to the first node to check
 * @param data_type_b A pointer to the second node to check
 */
bool are_data_types_compatible(data_type_node_t *data_type_a,
                               data_type_node_t *data_type_b);

/**
 * @brief Converts a type to string
 *
 * @param data_type A pointer to the node to convert
 * @param result A pointer to store the result, the caller doesn't own the
 * string
 *
 * @return NO_ERR on success
 */
err_code_t type_to_string(data_type_node_t *data_type, char **result);

/**
 * @brief Duplicates a type that is the superset of the other one
 *
 * @param data_type_node_a A pointer to the first type to compare
 * @param data_type_node_b A pointer to the second type to compare
 * @param data_type_node_c [out] A pointer to the node to store the result to
 *
 * @return NO_ERR on success
 */
err_code_t duplicate_superset_type(data_type_node_t *data_type_node_a,
                                   data_type_node_t *data_type_node_b,
                                   data_type_node_t **data_type_node_c);