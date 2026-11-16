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
#include <stdio.h>

// Maximum number of tokens the lexer can look ahead.
#define LEXER_MAX_LOOKAHEAD 10

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
 * @brief Sets lexer's input stream and resets lexer.
 *
 * Must be called before the first lexer_get_token()/lexer_peek_token().
 *
 * @param stream Input stream. If NULL, stdin is used.
 */
void lexer_init(FILE *stream);

/**
 * @brief Reads the next token from given stream.
 *
 * @param token    Empty token the lexer is supposed to return filled with copy
 *                 of the peeked token. The lexer allocates memory for the
 *                 token->value and because of that the caller must free the
 *                 memory using token_free() function.
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
 * @brief Reads the number of tokens given ahead from stream.
 *
 * @param token            Empty token the lexer is supposed to return filled.
 *                         It must be initialized (empty) before the call.
 *                         The caller owns token->value and releases it
 *                         as for lexer_get_token().
 * @param eol_code         Gives information whether TOK_EOL is forbidden,
 *                         allowed or required.
 * @param lookahead_offset Number of tokens the lexer should look ahead.
 *                         0 is the next, 1 is the second and so on. The number
 *                         must not exceed LEXER_MAX_LOOKAHEAD!
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
 */
void lexer_destroy(void);

#endif
