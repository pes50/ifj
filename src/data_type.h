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
} t_dt_type;

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
} t_primitive;

/**
 * @brief Struct for a Data type node
 */
typedef struct t_data_type_node {
  t_dt_type type;

  union {
    struct {
      char *name;
    } class_type;

    struct {
      struct t_data_type_node *parameter_types;
      struct t_data_type_node *return_type;
    } lambda_type;

    struct {
      struct t_data_type_node *elements_type;
    } list_type;

    t_primitive primitive_type;

    struct {
      struct t_data_type_node *types;
    } union_type;
  } data;

  struct t_data_type_node *next;
} t_data_type_node;

extern t_data_type_node node_int;
extern t_data_type_node node_double;
extern t_data_type_node node_string;
extern t_data_type_node node_unit;
extern t_data_type_node node_boolean;
extern t_data_type_node node_any;

/**
 * @brief Allocates a new Data type node
 *
 * @return A pointer to the allocated node or NULL if allocation fails
 */
t_data_type_node *create_type_node();

/**
 * @brief Recursively deallocates memory for a Data type node and its
 * descendants
 *
 * @param node A pointer to the Data type node to destroy. If NULL, the function
 * does nothing.
 */
void destroy_type_node(t_data_type_node *node);

/**
 * @brief Creates a data type node for class
 *
 * @param name Class name
 *
 * @return A pointer to created node or NULL if failed
 */
t_data_type_node *create_class_type_node(char *name);

/**
 * @brief Creates a data type node for lambda function
 *
 * @param parameter_types A pointer to a data type node containing the first
 * parameter type
 * @param return_type A pointer to a data type node representing the return type
 *
 * @return A pointer to created node or NULL if failed
 */
t_data_type_node *create_lambda_type_node(t_data_type_node *parameter_types,
                                          t_data_type_node *return_type);

/**
 * @brief Creates a data type node for list
 *
 * @param element_type A pointer to a data type node representing the element
 * type
 *
 * @return A pointer to created node or NULL if failed
 */
t_data_type_node *create_list_type_node(t_data_type_node *element_type);

/**
 * @brief Creates a data type node for a primitive type
 *
 * @param primitive Primitive that this node should represent
 *
 * @return A pointer to created node or NULL if failed
 */
t_data_type_node *create_primitive_type_node(t_primitive primitive);

/**
 * @brief Creates a data type node for union
 *
 * @param types A pointer to the first type in the union
 *
 * @return A pointer to created node or NULL if failed
 */
t_data_type_node *create_union_type_node(t_data_type_node *types);

/**
 * @brief Creates a deep copy of a type node
 *
 * @param original A pointer to the node to copy
 * @param copy [out] A pointer where to store the result
 *
 * @return NO_ERR on successful copy
 */
err_code_t duplicate_type_node(t_data_type_node *original,
                               t_data_type_node **copy);

/**
 * @brief Check if target type is contained within an union
 *
 * @param target_type A pointer to the type to search for
 * @param union_type A pointer to the union to search
 */
bool union_contains_type(t_data_type_node *target_type,
                         t_data_type_node *union_type);

/**
 * @brief Check if two unions represent the same type, regardless of order, for
 * example: A | B == B | A | A
 *
 * @param union_a A pointer to the first union to check
 * @param union_b A pointer to the second union to check
 */
bool are_unions_equivalent(t_data_type_node *union_a,
                           t_data_type_node *union_b);

/**
 * @brief Checks if union can be simplified to a target_type, for example: A | A
 * to A
 *
 * @param union_node A pointer to the union to check
 * @param target_type A pointer to the type to simplify to
 */
bool is_union_unnecessary(t_data_type_node *union_node,
                          t_data_type_node *target_type);

/**
 * @brief Checks if two data type nodes represent the same type
 *
 * @param data_type_a A pointer to the first node to check
 * @param data_type_b A pointer to the second node to check
 */
bool are_data_types_identical(t_data_type_node *data_type_a,
                              t_data_type_node *data_type_b);