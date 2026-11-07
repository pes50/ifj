/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file lexer.c
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Implementation of lexer.
 *
\*****************************************************************************/

#include "lexer.h"

static token_t token_buffer[LEXER_MAX_LOOKAHEAD + 1];
static size_t token_buf_count = 0;

err_code_t lexer_get_token(token_t *token, eol_code_t eol_code) {
  (void)token;
  (void)eol_code;
  return 0;
}

err_code_t lexer_peek_token(token_t *token, eol_code_t eol_code,
                            size_t lookahead_offset) {
  (void)token;
  (void)eol_code;
  (void)lookahead_offset;
  return 0;
}

err_code_t lexer_destroy(void) { return 0; }
