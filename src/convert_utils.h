/**************************** IFJ/IAL-Compiler-2026 ***************************\
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