/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file token.c
 *
 * @author Simon Šmíd       (xsmidsi00)
 *
 * @brief Implementation of the token functions.
 *
\*****************************************************************************/

#include "token.h"

#include <stdlib.h>
#include <string.h>

err_code_t token_init(token_t *token) {
  // Check if given argument is valid.
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  token->type = TOK_EMPTY;
  token->value = NULL;
  token->length = 0;

  return NO_ERR;
}

token_t *token_create(void) {
  token_t *token = malloc(sizeof(token_t));
  if (token == NULL) {
    return NULL;
  }

  token_init(token);
  return token;
}

err_code_t token_set(token_t *token, token_type_t type, const char *value,
                     size_t length) {
  // Check if given arguments are valid.
  if (token == NULL || (value == NULL && length != 0)) {
    return INTERNAL_ERR;
  }

  char *value_copy = NULL;

  if (value != NULL) {
    value_copy = malloc((length + 1) * sizeof(char));

    if (value_copy == NULL) {
      return INTERNAL_ERR;
    }

    memcpy(value_copy, value, length * sizeof(char));
    value_copy[length] = '\0';
  }

  // Free token's old value.
  free((void *)token->value);

  token->type = type;
  token->value = value_copy;
  token->length = length;

  return NO_ERR;
}

void token_clear(token_t *token) {
  if (token == NULL) {
    return;
  }

  free((void *)token->value);
  token->type = TOK_EMPTY;
  token->value = NULL;
  token->length = 0;
}

void token_free(token_t *token) {
  if (token == NULL) {
    return;
  }

  token_clear(token);
  free(token);
}
