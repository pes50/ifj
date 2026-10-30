/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file lexer.h
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Definition of the lexer API functions.
 *
\*****************************************************************************/

#ifndef LEXER_H
#define LEXER_H

#include "err_codes.h"
#include "token.h"

#include <stddef.h>

// Maximum number of tokens the lexer can look ahead.
#define MAX_LOOKAHEAD_OFFSET 10

/**
 * @brief Specifies whether end of line is expected, required or forbidden at
 * the current position.
 */
typedef enum {
  FORBIDDEN, // TOK_EOL must not be here. Only non EOL token can be returned,
             // otherwise an error is returned.
  ALLOWED,   // TOK_EOL may be here. Any type of token is returned.
  REQUIRED,  // TOK_EOL must be here. Any other token is an error.
} eol_code_t;

/**
 * @brief Reads the next token from stdin.
 *
 * @param token    Empty token the lexer is supposed to return filled. The lexer
 *                 allocates memory for the token->value and because of that the
 *                 caller must free the memory using lexer_free_token function.
 * @param eol_code Gives information whether TOK_EOL is forbidden, allowed or
 *                 required.
 *
 * @return NO_ERR on success.
 *         LEXICAL_ERR when lexical error is found.
 *         SYNTAX_ERR when EOL is found, but forbidden.
 *                    Or EOL is not found when required.
 *         INTERNAL_ERR on internal error.
 */
err_code_t lexer_get_token(token_t *token, eol_code_t eol_code);

/**
 * @brief Reads the number of tokens given ahead from stdin.
 *
 * @param token            Empty token the lexer is supposed to return filled.
 *                         The lexer allocates memory for the token->value.
 *                         The memory of token->value still belongs to the
 *                         lexer, so the caller must NOT call lexer_free_token
 *                         on it.
 * @param eol_code         Gives information whether TOK_EOL is forbidden,
 *                         allowed or required.
 * @param lookahead_offset Number of tokens the lexer should look ahead.
 *                         0 is the next, 1 is the second and so on. The number
 *                         must not exceed MAX_LOOKAHEAD_OFFSET!
 *
 * @return NO_ERR on success.
 *         LEXICAL_ERR when lexical error is found.
 *         SYNTAX_ERR when EOL is found, but forbidden.
 *                    Or EOL is not found when required.
 *         INTERNAL_ERR on internal error.
 */
err_code_t lexer_peek_token(token_t *token, eol_code_t eol_code,
                            size_t lookahead_offset);

/**
 * @brief Frees memory of all remaining tokens in the lexer's buffer.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on internal error.
 */
err_code_t lexer_destroy(void);

#endif
