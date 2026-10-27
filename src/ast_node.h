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

#include "data_type.h"
#include "token.h"
#include <stdbool.h>

/**
 * @brief Enumeration of all AST node types
 */
typedef enum {
  AST_FUNCTION_DECLARATION,
  AST_FUNCTION_CALL,
  AST_DOT_FUNCTION_CALL,
  AST_LAMBDA_DECLARATION,
  AST_PARAMETER_LIST,
  AST_PARAMETER,
  AST_CLASS_DECLARATION,
  AST_TYPE_DECLARATION,
  AST_EXTENSION_DECLARATION,
  AST_BLOCK,
  AST_VARIABLE_DECLARATION,
  AST_ASSIGNMENT,
  AST_IF,
  AST_WHILE_DO,
  AST_FOR_DO,
  AST_RETURN,
  AST_MATCH,
  AST_MATCH_CASE,
  AST_BINARY_OPERATION,
  AST_UNARY_OPERATION,
  AST_IDENTIFIER,
  AST_LITERAL,
  AST_INTERPOLATED_STRING,
} t_ast_type;

/**
 * @brief Struct for an AST node
 */
typedef struct t_ast_node {
  t_ast_type type;
  struct t_ast_node *next;
  t_data_type_node *data_type;

  union {
    struct {
      char *name;
      struct t_ast_node *parameter_lists;
      struct t_ast_node *body;
      t_data_type_node *return_type;
      bool is_main;
    } function_declaration;

    struct {
      char *name;
      struct t_ast_node *argument_lists;
    } function_call;

    struct {
      char *name;
      struct t_ast_node *target;
      struct t_ast_node *argument_lists;
    } dot_function_call;

    struct {
      struct t_ast_node *parameters;
      struct t_ast_node *body;
      t_data_type_node *return_type;
    } lambda_declaration;

    struct {
      struct t_ast_node *parameters;
    } parameter_list;

    struct {
      char *name;
      t_data_type_node *data_type;
    } parameter;

    struct {
      char *name;
      struct t_ast_node *attributes;
    } class_declaration;

    struct {
      char *name;
      t_data_type_node *data_type;
    } type_declaration;

    struct {
      struct t_ast_node *parameter;
      struct t_ast_node *function_declarations;
    } extension_declaration;

    struct {
      struct t_ast_node *statement;
    } block;

    struct {
      char *name;
      bool is_const;
      struct t_ast_node *expression;
      t_data_type_node *data_type;
    } variable_declaration;

    struct {
      char *name;
      struct t_ast_node *expression;
    } assignment;

    struct {
      struct t_ast_node *generator;
      struct t_ast_node *guards;
      struct t_ast_node *body;
    } for_do;

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
      struct t_ast_node *expression;
      struct t_ast_node *cases;
    } match;

    struct {
      struct t_ast_node *pattern;
      struct t_ast_node *guard;
      struct t_ast_node *block;
    } match_case;

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
      t_primitive primitive;
    } literal;

    struct {
      struct t_ast_node *expressions;
    } interpolated_string;
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