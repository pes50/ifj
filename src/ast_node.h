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
  AST_ARGUMENT_LIST,
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
  AST_LIST_CONSTRUCTOR,
  AST_BUILTIN_BODY,
} ast_type_t;

/**
 * @brief Struct for an AST node
 */
typedef struct ast_node_t {
  ast_type_t type;
  struct ast_node_t *next;
  data_type_node_t *data_type;
  bool _visited;

  union {
    struct {
      char *name;
      struct ast_node_t *parameter_lists;
      struct ast_node_t *body;
      data_type_node_t *return_type;
      bool is_main;
      bool is_static;
    } function_declaration;

    struct {
      char *name;
      struct ast_node_t *argument_lists;
      struct ast_node_t *calling;
    } function_call;

    struct {
      char *name;
      struct ast_node_t *target;
      struct ast_node_t *argument_lists;
      struct ast_node_t *calling;
    } dot_function_call;

    struct {
      struct ast_node_t *parameters;
      struct ast_node_t *body;
    } lambda_declaration;

    struct {
      struct ast_node_t *parameters;
    } parameter_list;

    struct {
      char *name;
      data_type_node_t *data_type;
      bool optional;
    } parameter;

    struct {
      struct ast_node_t *arguments;
    } argument_list;

    struct {
      char *name;
      struct ast_node_t *attribute_list;
      size_t unique_class_id;
    } class_declaration;

    struct {
      char *name;
      data_type_node_t *data_type;
    } type_declaration;

    struct {
      struct ast_node_t *parameter;
      struct ast_node_t *function_declarations;
    } extension_declaration;

    struct {
      struct ast_node_t *statement;
    } block;

    struct {
      char *name;
      bool is_const;
      struct ast_node_t *expression;
      data_type_node_t *data_type;
      char *unique_id;
    } variable_declaration;

    struct {
      char *name;
      struct ast_node_t *expression;
      struct ast_node_t *calling;
    } assignment;

    struct {
      struct ast_node_t *iterator;
      struct ast_node_t *generator;
      struct ast_node_t *guards;
      struct ast_node_t *body;
    } for_do;

    struct {
      struct ast_node_t *condition;
      struct ast_node_t *body;
    } while_do;

    struct {
      struct ast_node_t *condition;
      struct ast_node_t *true_branch;
      struct ast_node_t *false_branch;
    } if_statement;

    struct {
      struct ast_node_t *expression;
      bool discard;
    } return_statement;

    struct {
      struct ast_node_t *expression;
      struct ast_node_t *cases;
    } match;

    struct {
      struct ast_node_t *pattern;
      struct ast_node_t *guard;
      struct ast_node_t *block;
    } match_case;

    struct {
      token_type_t op;
      struct ast_node_t *left;
      struct ast_node_t *right;
    } binary_operation;

    struct {
      token_type_t op;
      struct ast_node_t *expression;
    } unary_operation;

    struct {
      char *name;
      struct ast_node_t *calling;
    } identifier;

    struct {
      char *raw;
      data_type_node_t *data_type;
    } literal;

    struct {
      struct ast_node_t *expressions;
    } interpolated_string;

    struct {
      struct inner_statement *first_instruction;
    } builtin_body;
  } data;
} ast_node_t;

/**
 * @brief Allocates a new AST node
 *
 * @return A pointer to the allocated node or NULL if allocation fails
 */
ast_node_t *create_node();

/**
 * @brief Recursively deallocates memory for an AST node and its descendants
 *
 * @param node A pointer to the AST node to destroy. If NULL, the function does
 * nothing.
 */
void destroy_node(ast_node_t *node);

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
ast_node_t *create_function_declaration_node(char *name,
                                             ast_node_t *parameter_lists,
                                             ast_node_t *body,
                                             data_type_node_t *return_type,
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
ast_node_t *create_function_call_node(char *name, ast_node_t *argument_lists);

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
ast_node_t *create_dot_function_call_node(char *name, ast_node_t *target,
                                          ast_node_t *argument_lists);

/**
 * @brief Creates an AST node for lambda function declaration
 *
 * @param parameters A pointer to the AST node representing the first parameter
 * @param body A pointer to the AST block node representing the function body
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_lambda_declaration_node(ast_node_t *parameters,
                                           ast_node_t *body);

/**
 * @brief Creates an AST node for function parameter list
 *
 * @param parameters A pointer to the AST node representing the first parameter.
 * NULL for a list without parameters
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_parameter_list_node(ast_node_t *parameters);

/**
 * @brief Creates an AST node for function parameter
 *
 * @param name Parameter name
 * @param data_type Type of the parameter
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_parameter_node(char *name, data_type_node_t *data_type);

/**
 * @brief Creates an AST node for argument list
 *
 * @param arguments A pointer to the AST node representing the first argument
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_argument_list_node(ast_node_t *arguments);

/**
 * @brief Creates an AST node for class declaration
 *
 * @param name Class identifier
 * @param attributes A pointer to the AST node representing the attribute list
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_class_declaration_node(char *name, ast_node_t *attributes);

/**
 * @brief Creates an AST node for type declaration
 *
 * @param name Type identifier
 * @param attributes A pointer to the Data type node
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_type_declaration_node(char *name,
                                         data_type_node_t *data_type);

/**
 * @brief Creates an AST node for extension declaration
 *
 * @param parameters A pointer to the AST node representing the parameter
 * @param function_declarations A pointer to the AST node representing the first
 * function declaration
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *
create_extension_declaration_node(ast_node_t *parameter,
                                  ast_node_t *function_declarations);

/**
 * @brief Creates an AST node for code block
 *
 * @param statement A pointer to the AST node representing the first statement
 * in the block
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_block_node(ast_node_t *statement);

/**
 * @brief Appends node to the and of a node chain
 *
 * @param entry_node A pointer to an AST node to start from
 * @param node_to_append A pointer to an AST node that will be appended
 */
void node_append(ast_node_t *entry_node, ast_node_t *node_to_append);

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
ast_node_t *create_variable_declaration_node(char *name,
                                             data_type_node_t *data_type,
                                             bool is_const,
                                             ast_node_t *expression);

/**
 * @brief Creates an AST node for assignment
 *
 * @param name A destination identifier
 * @param expression A pointer to the AST node representing the expression to
 * assign
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_assignment_node(char *name, ast_node_t *expression);

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
ast_node_t *create_if_node(ast_node_t *condition, ast_node_t *true_branch,
                           ast_node_t *false_branch);

/**
 * @brief Creates an AST node for for loop
 *
 * @param iterator A pointer to the AST parameter node
 * @param generator A pointer to the AST expression node containing generator
 * @param guards A pointer to the AST expression node representing the guards
 * @param body A pointer to the AST block node representing the loop body
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_for_do_node(ast_node_t *iterator, ast_node_t *generator,
                               ast_node_t *guards, ast_node_t *body);

/**
 * @brief Creates an AST node for while loop
 *
 * @param condition A pointer to the AST node representing the condition
 * expression
 * @param body A pointer to the AST block node representing the loop body
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_while_do_node(ast_node_t *condition, ast_node_t *body);

/**
 * @brief Creates an AST node for return statement
 *
 * @param expression A pointer to the AST node representing the return
 * expression
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_return_node(ast_node_t *expression);

/**
 * @brief Creates an AST node for match
 *
 * @param expression A pointer to the AST node representing the value to match
 * @param cases A pointer to the AST match case node representing the first
 * branch
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_match_node(ast_node_t *expression, ast_node_t *cases);

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
ast_node_t *create_match_case_node(ast_node_t *pattern, ast_node_t *guard,
                                   ast_node_t *block);

/**
 * @brief Creates an AST node for binary operation
 *
 * @param operation The token type representing the binary operator
 * @param left A pointer to the AST node representing the left operand
 * @param right A pointer to the AST node representing the right operand
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_binary_operation_node(token_type_t operation,
                                         ast_node_t *left, ast_node_t *right);

/**
 * @brief Creates an AST node for unary operation
 *
 * @param operation The token type representing the unary operator
 * @param expression A pointer to the AST node representing the operand
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_unary_operation_node(token_type_t operation,
                                        ast_node_t *expression);

/**
 * @brief Creates an AST node from given token
 *
 * @param token A token of an identifier or a literal
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_leaf_node(token_t *token);

/**
 * @brief Creates an AST node for interpolated string
 *
 * @param expressions A pointer to the AST node representing the first part of
 * the string
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_interpolated_string_node(ast_node_t *expressions);

/**
 * @brief Creates an AST node for UNIT literal
 *
 * @return A pointer to created node or NULL if failed
 */
ast_node_t *create_unit_literal_node();