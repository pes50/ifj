/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file parser.c
 *
 * @authors Matous Jahoda (xjahodm00)
 * @authors Roman Skoupy (xskoupr00)
 *
 * @brief Implementation of parser
 *
 \*****************************************************************************/

#include "parser.h"
#include "scanner.h"
#include "token.h"
#include <stdio.h>

// placeholder
int parse() {
  token_t token;
  do {
    token = get_token();
    printf("%i ", token.type);
  } while (token.type != TOK_ERR);
  printf("\n");
  return 0;
}
