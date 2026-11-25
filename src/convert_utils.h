/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file convert_utils.h
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Specification of helper functions used in AST to 3AC conversion
 *
\*****************************************************************************/

#pragma once

#include "ast_convert.h"
#include "inner_code.h"

/**
 * @brief Inserts a statement at the function start
 *
 * @param context Context structure of the function in which the node is
 * @param op Operation code for the statement
 * @param dest Destination operand symbol
 * @param src1 First source operand symbol
 * @param src2 Second source operand symbol
 *
 * @return NO_ERR on success
 */
err_code_t insert_at_function_start(context_t *context,
                                    inner_statement_operation_t op,
                                    inner_symbol_t dest, inner_symbol_t src1,
                                    inner_symbol_t src2);

/**
 * @brief Creates unique variable name
 *
 * @param context Context structure of the function in which the node is
 * @param identifier Variable name
 *
 * @return A pointer to the unique variable name
 */
char *create_unique_variable(context_t *context, const char *identifier);

/**
 * @brief Creates unique label name
 *
 * @param context Context structure of the function in which the node is
 * @param identifier Variable name
 *
 * @return A pointer to the unique label
 */
char *create_unique_label(context_t *context, const char *identifier);

/**
 * @brief Pops and discards the top value from the data stack
 *
 * @param context Context structure of the function
 *
 * @return NO_ERR on success
 */
err_code_t discard_stack_top(context_t *context);

/**
 * @brief Creates a unique helper variable symbol
 *
 * @param context Context structure of the function
 * @param[out] out_identifier A pointer to store the created symbol
 *
 * @return NO_ERR on success
 */
err_code_t create_helper_variable(context_t *context,
                                  inner_symbol_t *out_identifier);

/**
 * @brief Converts an AST literal node to an inner symbol
 *
 * @param node A pointer to the AST literal node
 * @param[out] out_identifier A pointer to store the created symbol
 *
 * @return NO_ERR on success
 */
err_code_t create_literal_symbol(ast_node_t *node,
                                 inner_symbol_t *out_identifier);

/**
 * @brief Gets unique id for supplied class, creates one if class doesn't have
 * one
 *
 * @param context Context structure of the function in which the node is
 * @param class_declaration A pointer to the AST class declaration node
 * @param out_id A pointer to store the id
 *
 * @return NO_ERR on success
 */
err_code_t get_or_assign_class_id(context_t *context,
                                  ast_node_t *class_declaration,
                                  size_t *out_id);

/**
 * @brief Appends INT2FLOATS if conversion from int to double is needed
 *
 * @param context Context structure of the function in which the node is
 * @param examined_operand A pointer to the first AST node to check the type of
 * @param other_operand A pointer to the second AST node to check the type of
 *
 * @return NO_ERR on success
 */
err_code_t retype_if_needed(context_t *context, ast_node_t *examined_operand,
                            ast_node_t *other_operand);