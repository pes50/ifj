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

err_code_t discard_stack_top(context_t *context) {
  inner_symbol_t trash_var = create_var_symbol(TRASH);
  inner_symbol_t empty_sym = create_empty_symbol();

  RETURN_ERR(
      append_statement(context, INNER_POPS, trash_var, empty_sym, empty_sym));
}

err_code_t create_helper_variable(context_t *context,
                                  inner_symbol_t *out_identifier) {
  inner_symbol_t empty_sym = create_empty_symbol();

  char *helper_var_identifier = create_unique_variable(context, "helper");
  if (helper_var_identifier == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_symbol_t non_owning_sym = create_var_symbol(helper_var_identifier);
  inner_symbol_t owning_sym = non_owning_sym;
  owning_sym.owns_identifier = true;

  err_code_t err = insert_at_function_start(context, INNER_DEFVAR, empty_sym,
                                            owning_sym, empty_sym);
  if (err != NO_ERR) {
    free(helper_var_identifier);
    *out_identifier = create_empty_symbol();
    RETURN_ERR(err);
  }

  *out_identifier = non_owning_sym;
  RETURN_ERR(NO_ERR);
}

/**
 * @brief Removes '_', 'd' and 'D' characters from numeric literals
 *
 * @param raw String form of the literal
 * @param strip_d_suffix Bool to inform that the type is double -> 'd' or 'D'
 *                       should be removed from the end of the literal
 *
 * @return A pointer to a sanitized literal or NULL if failed
 */
static char *clean_numeric_literal(const char *raw, bool strip_d_suffix) {
  if (raw == NULL) {
    return NULL;
  }

  size_t len = strlen(raw);
  char *clean = malloc(len + 1);
  if (clean == NULL) {
    return NULL;
  }

  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    // Skip underscores
    if (raw[i] == '_') {
      continue;
    }

    if (strip_d_suffix && i == len - 1 &&   // Is last
        (raw[i] == 'd' || raw[i] == 'D')) { // Suffix 'd' or 'D'
      break;
    }

    clean[j++] = raw[i];
  }

  // Add terminator
  clean[j] = '\0';

  return clean;
}

err_code_t create_literal_symbol(ast_node_t *node,
                                 inner_symbol_t *out_identifier) {
  if (node == NULL || node->type != AST_LITERAL || out_identifier == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  data_type_node_t *dtype = node->data.literal.data_type;
  if (dtype == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  inner_value_t val = {0};

  // Convert unit to nil
  if (dtype->type == DATA_TYPE_UNIT) {
    val.inner_nil = NULL;
    *out_identifier = create_const_symbol(INNER_NIL, val);
    RETURN_ERR(NO_ERR);
  }

  if (dtype->type != DATA_TYPE_PRIMITIVE) {
    RETURN_ERR(INTERNAL_ERR);
  }

  char *raw = node->data.literal.raw;
  if (raw == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  switch (dtype->data.primitive_type) {
  case PRIMITIVE_INT: {
    char *clean = clean_numeric_literal(raw, false);
    if (clean == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    char *endptr = NULL;
    val.inner_int = strtoll(clean, &endptr, 10);
    bool invalid = clean[0] == '\0' // is empty string
                   || *endptr != '\0';
    free(clean);
    if (invalid) {
      RETURN_ERR(INTERNAL_ERR);
    }

    *out_identifier = create_const_symbol(INNER_INT, val);
    RETURN_ERR(NO_ERR);
  }

  case PRIMITIVE_DOUBLE: {
    char *clean = clean_numeric_literal(raw, true);
    if (clean == NULL) {
      RETURN_ERR(INTERNAL_ERR);
    }

    char *endptr = NULL;
    val.inner_float = strtod(clean, &endptr);
    bool invalid = clean[0] == '\0' // is empty string
                   || *endptr != '\0';
    free(clean);
    if (invalid) {
      RETURN_ERR(INTERNAL_ERR);
    }

    *out_identifier = create_const_symbol(INNER_FLOAT, val);
    RETURN_ERR(NO_ERR);
  }

  case PRIMITIVE_BOOLEAN:
    val.inner_bool = strcmp(raw, "true") == 0;
    *out_identifier = create_const_symbol(INNER_BOOL, val);
    RETURN_ERR(NO_ERR);

  case PRIMITIVE_STRING:
    val.inner_string = raw;
    *out_identifier = create_const_symbol(INNER_STRING, val);
    RETURN_ERR(NO_ERR);
  }

  RETURN_ERR(INTERNAL_ERR);
}

err_code_t get_or_assign_class_id(context_t *context,
                                  ast_node_t *class_declaration,
                                  size_t *out_id) {
  if (context == NULL || class_declaration == NULL ||
      class_declaration->type != AST_CLASS_DECLARATION || out_id == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  // Assign a new id if class doesn't have one
  if (class_declaration->data.class_declaration.unique_class_id == 0) {
    class_declaration->data.class_declaration.unique_class_id =
        ++context->unique_class_counter;
  }

  // Update pointer
  *out_id = class_declaration->data.class_declaration.unique_class_id;

  RETURN_ERR(NO_ERR);
}

err_code_t retype_if_needed(context_t *context, ast_node_t *examined_operand,
                            ast_node_t *other_operand) {
  if (context == NULL || examined_operand == NULL ||
      examined_operand->data_type == NULL || other_operand == NULL ||
      other_operand->data_type == NULL) {
    RETURN_ERR(INTERNAL_ERR);
  }

  if (are_data_types_identical(&node_int, examined_operand->data_type) &&
      are_data_types_identical(&node_double, other_operand->data_type) &&
      context->tail->operation != INNER_RETURN) {
    inner_symbol_t empty_sym = create_empty_symbol();
    RETURN_ERR(append_statement(context, INNER_INT2FLOATS, empty_sym, empty_sym,
                                empty_sym));
  }

  RETURN_ERR(NO_ERR);
}