/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file token_stack.h
 *
 * @author Matouš Jahoda (xjahodm00)
 *
 * @brief Specification for token stack
 *
\*****************************************************************************/

#pragma once

#include "ast_node.h"
#include "token.h"
#include <stdbool.h>

/**
 * @brief Represents token stack
 */
typedef struct token_stack_t token_stack_t;

typedef enum { TOKEN, NONTERMINAL, ERROR_OR_EMPTY } token_stack_element_type_t;

/**
 * @brief Allocates and initializes a new empty token stack
 *
 * @return A pointer to the allocated stack or NULL if allocation fails
 */
token_stack_t *create_token_stack();

/**
 * @brief Destroys token stack and all its contents
 *
 * @param stack A pointer to the target stack
 */
void destroy_token_stack(token_stack_t *stack);

/**
 * @brief Pushes a new token onto the top of the stack
 *
 * @param stack A pointer to the target stack
 * @param token A pointer to the token being stored
 *
 * @return true if the element was successfully pushed, false if allocation
 * fails or stack is NULL.
 */
bool push_token(token_stack_t *stack, token_t *token);

/**
 * @brief Pushes a new nonterminal onto the top of the stack
 *
 * @param stack A pointer to the target stack
 * @param node AST node of the nonterminal
 *
 * @return true if the element was successfully pushed, false if allocation
 * fails or stack is NULL.
 */
bool push_nonterm(token_stack_t *stack, ast_node_t *node);

/**
 * @brief Reduces top token on stack to nonterminals
 *
 * @param stack A pointer to the target stack
 *
 * @return true if succesfully reduced, else false
 */
bool reduce(token_stack_t *stack);

/**
 * @brief Remove parenthesis token from the stack
 *  from the second to top position
 *
 * @param stack A pointer to the target stack
 *
 * @return true if succesfully removed, else false
 */
bool remove_paren(token_stack_t *stack);

/**
 * @brief Returns token type of the top token on the stack
 *
 * @param stack A pointer to the target stack
 *
 * @return Token type of the top token on the stack
 */
token_type_t get_top_token_type(token_stack_t *stack);

/**
 * @brief Checks if there are tokens in the token stack
 *
 * @param stack A pointer to the target stack
 *
 * @return true if there are no tokens in token stack, else false
 */
bool is_token_empty(token_stack_t *stack);

/**
 * @brief If there is only one final nonterminal in the stack,
 *      its node gets returned
 *
 * @param stack A pointer to the target stack
 *
 * @return Pointer to the node
 */
ast_node_t *return_final_node(token_stack_t *stack);

token_stack_element_type_t peek_top_element_type(token_stack_t *stack);

bool get_top_element(token_stack_t *stack, ast_node_t **node, token_t **token);