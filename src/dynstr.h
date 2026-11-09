/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file dynstr.h
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Dynamic string used by the lexer.
 *
\*****************************************************************************/

#ifndef DYNSTR_H
#define DYNSTR_H

#include "err_codes.h"

#include <stddef.h>

/**
 * @brief Dynamic string
 */
typedef struct {
  char *data;
  size_t length;
  size_t capacity;
} dynstr_t;

/**
 * @brief Allocate initial buffer. Create empty string.
 *
 * @param str Dynamic string to initialize.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on allocation failure.
 */
err_code_t dynstr_init(dynstr_t *str);

/**
 * @brief Append one character to the string. Extends buffer when needed.
 *
 * @param str       Dynamic string to append to.
 * @param character Character to append.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on allocation failure (the existing string stays valid).
 */
err_code_t dynstr_append_char(dynstr_t *str, char character);

/**
 * @brief Release ownership of data.
 *
 * @param str Dynamic string to release.
 *
 * @return Pointer to the data. (Caller must free the data)
 */
char *dynstr_release(dynstr_t *str);

/**
 * @brief Frees the memory owned by the dynamic string.
 * itself.
 *
 * @param str String to free.
 */
void dynstr_free(dynstr_t *str);

#endif
