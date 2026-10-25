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