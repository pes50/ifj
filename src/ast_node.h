/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file ast_node.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of abstract syntax tree node
 *
\*****************************************************************************/

#pragma once

#include "token.h"
#include <stdbool.h>

/**
 * @brief Enumeration of all AST node types
 */
typedef enum {
  AST_FUNCTION_DECLARATION,
  AST_FUNCTION_CALL,
  AST_PARAMETER,
  AST_BLOCK,
  AST_VARIABLE_DECLARATION,
  AST_ASSIGNMENT,
  AST_IF,
  AST_WHILE_DO,
  AST_RETURN,
  AST_BINARY_OPERATION,
  AST_UNARY_OPERATION,
  AST_IDENTIFIER,
  AST_LITERAL,
} AST_TYPE;

/**
 * @brief Enumeration of all data types
 */
typedef enum {
  DT_UNKNOWN,
  DT_INT,
  DT_DOUBLE,
  DT_STRING,
  DT_UNIT,
  DT_BOOLEAN,
  DT_LIST,
} DATA_TYPE;

/**
 * @brief Struct for an AST node
 */
typedef struct t_ast_node {
  AST_TYPE type;
  struct t_ast_node *next;
  DATA_TYPE data_type;

  union {
    struct {
      char *name;
      struct t_ast_node *parameters;
      struct t_ast_node *body;
      DATA_TYPE return_type;
    } function_declaration;

    struct {
      char *name;
      struct t_ast_node *arguments;
    } function_call;

    struct {
      char *name;
      DATA_TYPE data_type;
    } parameter;

    struct {
      struct t_ast_node *statement;
    } block;

    struct {
      char *name;
      bool is_const;
      struct t_ast_node *expression;
      DATA_TYPE data_type;
    } variable_declaration;

    struct {
      char *name;
      struct t_ast_node *expression;
    } assignment;

    struct {
      struct t_ast_node *condition;
      struct t_ast_node *body;
    } while_do;

    struct {
      struct t_ast_node *condition;
      struct t_ast_node *true_branch;
      struct t_ast_node *false_branch;
    } if_statement;

    struct {
      struct t_ast_node *expression;
    } return_statement;

    struct {
      token_type_t op;
      struct t_ast_node *left;
      struct t_ast_node *right;
    } binary_operation;

    struct {
      token_type_t op;
      struct t_ast_node *expression;
    } unary_operation;

    struct {
      char *name;
    } identifier;

    struct {
      char *raw;
      DATA_TYPE data_type;
    } literal;
  } data;
} t_ast_node;

/**
 * @brief Allocates a new AST node
 *
 * @return A pointer to the allocated node or NULL if allocation fails
 */
t_ast_node *create_node();

/**
 * @brief Recursively deallocates memory for an AST node and its descendants
 *
 * @param node A pointer to the AST node to destroy. If NULL, the function does
 * nothing.
 */
void destroy_node(t_ast_node *node);