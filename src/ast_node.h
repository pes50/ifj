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
  bool _visited;

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
      t_data_type_node *data_type;
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

/**
 * @brief Creates an AST node for function declaration
 *
 * @param name Function identifier
 * @param parameter_lists A pointer to the AST node representing the first
 * parameter list. NULL for a function without a parameter list
 * @param body A pointer to the AST block node representing the function body
 * @param return_type Type of the return value
 * @param is_main True if the function is the entrypoint of the program
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_function_declaration_node(char *name,
                                             t_ast_node *parameter_lists,
                                             t_ast_node *body,
                                             t_data_type_node *return_type,
                                             bool is_main);

/**
 * @brief Creates an AST node for function call
 *
 * @param name Function identifier
 * @param argument_lists A pointer to the AST node representing the first
 * argument list. NULL for a function without an argument list
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_function_call_node(char *name, t_ast_node *argument_lists);

/**
 * @brief Creates an AST node for dot function call
 *
 * @param name Function identifier
 * @param target A pointer to the AST node representing the expression before
 * the dot
 * @param argument_lists A pointer to the AST node representing the first
 * argument list. NULL for a function without an argument list
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_dot_function_call_node(char *name, t_ast_node *target,
                                          t_ast_node *argument_lists);

/**
 * @brief Creates an AST node for lambda function declaration
 *
 * @param parameters A pointer to the AST node representing the first parameter
 * @param body A pointer to the AST block node representing the function body
 * @param return_type Type of the return value
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_lambda_declaration_node(t_ast_node *parameters,
                                           t_ast_node *body,
                                           t_data_type_node *return_type);

/**
 * @brief Creates an AST node for function parameter list
 *
 * @param parameters A pointer to the AST node representing the first parameter.
 * NULL for a list without parameters
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_parameter_list_node(t_ast_node *parameters);

/**
 * @brief Creates an AST node for function parameter
 *
 * @param name Parameter name
 * @param data_type Type of the parameter
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_parameter_node(char *name, t_data_type_node *data_type);

/**
 * @brief Creates an AST node for class declaration
 *
 * @param name Class identifier
 * @param attributes A pointer to the AST node representing the first attribute
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_class_declaration_node(char *name, t_ast_node *attributes);

/**
 * @brief Creates an AST node for type declaration
 *
 * @param name Type identifier
 * @param attributes A pointer to the Data type node
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_type_declaration_node(char *name,
                                         t_data_type_node *data_type);

/**
 * @brief Creates an AST node for extension declaration
 *
 * @param parameters A pointer to the AST node representing the parameter
 * @param function_declarations A pointer to the AST node representing the first
 * function declaration
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *
create_extension_declaration_node(t_ast_node *parameter,
                                  t_ast_node *function_declarations);

/**
 * @brief Creates an AST node for code block
 *
 * @param statement A pointer to the AST node representing the first statement
 * in the block
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_block_node(t_ast_node *statement);

/**
 * @brief Creates an AST node for variable declaration
 *
 * @param name Variable name
 * @param data_type Type of the variable
 * @param is_const If true, variable is declared as a constant
 * @param expression A pointer to the AST node representing the expression to
 * assign
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_variable_declaration_node(char *name,
                                             t_data_type_node *data_type,
                                             bool is_const,
                                             t_ast_node *expression);

/**
 * @brief Creates an AST node for assignment
 *
 * @param name A destination identifier
 * @param expression A pointer to the AST node representing the expression to
 * assign
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_assignment_node(char *name, t_ast_node *expression);

/**
 * @brief Creates an AST node for if statement
 *
 * @param condition A pointer to the AST node representing the condition
 * expression
 * @param true_branch A pointer to the AST block node representing the true
 * branch
 * @param false_branch A pointer to the AST block node representing the false
 * branch
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_if_node(t_ast_node *condition, t_ast_node *true_branch,
                           t_ast_node *false_branch);

/**
 * @brief Creates an AST node for for loop
 *
 * @param generator A pointer to the AST expression node containing generator
 * @param guards A pointer to the AST expression node representing the guards
 * @param body A pointer to the AST block node representing the loop body
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_for_do_node(t_ast_node *generator, t_ast_node *guards,
                               t_ast_node *body);

/**
 * @brief Creates an AST node for while loop
 *
 * @param condition A pointer to the AST node representing the condition
 * expression
 * @param body A pointer to the AST block node representing the loop body
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_while_do_node(t_ast_node *condition, t_ast_node *body);

/**
 * @brief Creates an AST node for return statement
 *
 * @param expression A pointer to the AST node representing the return
 * expression
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_return_node(t_ast_node *expression);

/**
 * @brief Creates an AST node for match
 *
 * @param expression A pointer to the AST node representing the value to match
 * @param cases A pointer to the AST match case node representing the first
 * branch
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_match_node(t_ast_node *expression, t_ast_node *cases);

/**
 * @brief Creates an AST node for match case
 *
 * @param pattern A pointer to the AST node representing the value to match
 * @param guard An optional pointer to the AST expression node representing a
 * guard
 * @param block A pointer to the AST node representing the branch
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_match_case_node(t_ast_node *pattern, t_ast_node *guard,
                                   t_ast_node *block);

/**
 * @brief Creates an AST node for binary operation
 *
 * @param operation The token type representing the binary operator
 * @param left A pointer to the AST node representing the left operand
 * @param right A pointer to the AST node representing the right operand
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_binary_operation_node(token_type_t operation,
                                         t_ast_node *left, t_ast_node *right);

/**
 * @brief Creates an AST node for unary operation
 *
 * @param operation The token type representing the unary operator
 * @param expression A pointer to the AST node representing the operand
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_unary_operation_node(token_type_t operation,
                                        t_ast_node *expression);

/**
 * @brief Creates an AST node from given token
 *
 * @param token A token of an identifier or a literal
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_leaf_node(token_t *token);

/**
 * @brief Creates an AST node for interpolated string
 *
 * @param expressions A pointer to the AST node representing the first part of
 * the string
 *
 * @return A pointer to created node or NULL if failed
 */
t_ast_node *create_interpolated_string_node(t_ast_node *expressions);