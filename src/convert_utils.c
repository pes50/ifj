/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file convert_utils.c
 *
 * @author Roman Skoupý     (xskoupr00)
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of helper functions used in AST to 3AC conversion
 *
 \*****************************************************************************/

#include "convert_utils.h"
#include "convert_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

err_code_t insert_at_function_start(context_t *context,
                                    inner_statement_operation_t op,
                                    inner_symbol_t dest, inner_symbol_t src1,
                                    inner_symbol_t src2) {
  if (context == NULL || context->function_start == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_statement_t *statement = create_statement(op, dest, src1, src2);
  if (statement == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  statement->next_statement = context->function_start->next_statement;
  context->function_start->next_statement = statement;

  // If no body statements were appended yet, tail must move forward too
  if (context->tail == context->function_start) {
    context->tail = statement;
  }

  RETURN_ERR(NO_ERR);
}

// Creates a unique LF@$number$identifier variable name and increments the
// counter
char *create_unique_variable(context_t *context, const char *identifier) {
  if (context == NULL || identifier == NULL) {
    return NULL;
  }

  unsigned long long unique_number =
      (unsigned long long)context->unique_var_counter;

  int len = snprintf(NULL, 0, "LF@$%llu$%s", unique_number, identifier);
  if (len < 0) {
    return NULL;
  }

  char *result = malloc((size_t)len + 1);
  if (result == NULL) {
    return NULL;
  }

  snprintf(result, (size_t)len + 1, "LF@$%llu$%s", unique_number, identifier);
  context->unique_var_counter++;
  return result;
}

char *create_unique_label(context_t *context, const char *identifier) {
  if (context == NULL || identifier == NULL) {
    return NULL;
  }

  size_t unique_number = context->unique_var_counter++;
  int len = snprintf(NULL, 0, "$%llu$%s", (unsigned long long)unique_number,
                     identifier);
  if (len < 0) {
    return NULL;
  }

  char *result = malloc((size_t)len + 1);
  if (result == NULL) {
    return NULL;
  }

  snprintf(result, (size_t)len + 1, "$%llu$%s",
           (unsigned long long)unique_number, identifier);
  return result;
}
