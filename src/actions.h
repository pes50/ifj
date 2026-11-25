/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file actions.h
 *
 * @author Matous Jahoda (xjahodm00)
 *
 * @brief Declaration of actions for expresion parsing
 *
 * @note Actions follow precedence table, which can be found in documentation
 *
\*****************************************************************************/

#include "token.h"

/**
 * @brief Possible actions for expr parser
 */
typedef enum { SHIFT, REDUCE, CONSUME_PAREN, ERROR, END } actions_t;

/**
 * @brief Returns action based on token types on input and top of stack
 *
 * @param input token type on input
 * @param stack token type on top of stack
 *
 * @return Action for parser, or error action on unsupported token type
 *  or incorrect token type combination
 */
actions_t get_action(token_type_t input, token_type_t stack);