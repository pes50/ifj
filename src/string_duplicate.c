/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file string_duplicate.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of string_duplicate function
 *
\*****************************************************************************/

#include <stdlib.h>
#include <string.h>

char *string_duplicate(const char *src) {
  if (src == NULL) {
    return NULL;
  }

  size_t len = strlen(src) + 1;
  char *dst = malloc(len);
  if (dst == NULL) {
    return NULL;
  }

  memcpy(dst, src, len);

  return dst;
}