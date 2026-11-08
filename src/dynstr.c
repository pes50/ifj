/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file dynstr.c
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Implementation of dynamic string.
 *
\*****************************************************************************/

#include "dynstr.h"

#include <stdlib.h>

#define DYNSTR_INITIAL_CAPACITY 16

err_code_t dynstr_init(dynstr_t *str) {
  if (str == NULL) {
    return INTERNAL_ERR;
  }

  str->data = malloc(sizeof(char) * DYNSTR_INITIAL_CAPACITY);
  if (str->data == NULL) {
    str->length = 0;
    str->capacity = 0;
    return INTERNAL_ERR;
  }

  str->data[0] = '\0';
  str->length = 0;
  str->capacity = DYNSTR_INITIAL_CAPACITY;

  return NO_ERR;
}

err_code_t dynstr_append_char(dynstr_t *str, char character) {
  if (str == NULL || str->data == NULL) {
    return INTERNAL_ERR;
  }

  // +2 means 1 for the new character and 1 for the terminating NUL
  if (str->length + 2 > str->capacity) {
    size_t new_capacity = str->capacity * 2;
    char *new_data = realloc(str->data, sizeof(char) * new_capacity);
    if (new_data == NULL) {
      return INTERNAL_ERR;
    }
    str->data = new_data;
    str->capacity = new_capacity;
  }

  str->data[str->length] = character;
  str->length += 1;
  str->data[str->length] = '\0';
  return NO_ERR;
}

char *dynstr_release(dynstr_t *str) {
  char *data_to_release = str->data;
  str->data = NULL;
  str->length = 0;
  str->capacity = 0;
  return data_to_release;
}

void dynstr_free(dynstr_t *str) {
  if (str == NULL) {
    return;
  }
  free(str->data);
  str->data = NULL;
  str->length = 0;
  str->capacity = 0;
}
