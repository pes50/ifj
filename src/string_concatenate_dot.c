/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file string_concatenate_dot.c
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Implementation of string_concatenate_dot function
 *
\*****************************************************************************/

#include <stdlib.h>
#include <string.h>

char *string_concatenate_dot(const char *first, const char *second) {
  if (first == NULL || second == NULL) {
    return NULL;
  }

  size_t length_a = strlen(first);
  size_t length_b = strlen(second);

  char *concatenated = malloc((length_a + length_b + 2) * sizeof(char));
  if (concatenated == NULL) {
    return NULL;
  }

  memcpy(concatenated, first, length_a);
  concatenated[length_a] = '.';
  memcpy(concatenated + length_a + 1, second, length_b);
  concatenated[length_a + length_b + 1] = '\0';

  return concatenated;
}