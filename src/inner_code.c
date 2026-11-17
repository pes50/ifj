/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file inner_code.c
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of functions for working with inner code
 *
 \*****************************************************************************/

#include "inner_code.h"

/********************\
 * Helper functions *
\********************/

/**
 * @brief Allocates memory for one statement
 *
 * @return A pointer to the allocated memory or NULL if allocation fails
 */
inner_statement_t *allocate_statement() {
  return calloc(1, sizeof(inner_statement_t));
}

/***************************************\
 * Inner code interface implementation *
\***************************************/

inner_symbol_t create_empty_symbol() {
  inner_symbol_t empty;
  empty.type = INNER_SYM_EMPTY;
  return empty;
}

inner_symbol_t create_var_symbol(char *identificator) {
  inner_symbol_t var;

  var.type = INNER_SYM_VAR;
  var.data.identificator = identificator;

  return var;
}

inner_symbol_t create_const_symbol(inner_datatype_t datatype,
                                   inner_value_t value) {
  inner_symbol_t constant;

  constant.type = INNER_SYM_CONST;
  constant.data.constant.datatype = datatype;
  constant.data.constant.value = value;

  return constant;
}

inner_symbol_t create_arr_symbol(char *identificator) {
  inner_symbol_t arr;

  arr.type = INNER_SYM_ARR;
  arr.data.identificator = identificator;

  return arr;
}

inner_symbol_t create_label_symbol(char *identificator) {
  inner_symbol_t label;

  label.type = INNER_SYM_LABEL;
  label.data.identificator = identificator;

  return label;
}

inner_statement_t *create_statement(inner_statement_operation_t operation,
                                    inner_symbol_t destination,
                                    inner_symbol_t source1,
                                    inner_symbol_t source2) {
  inner_statement_t *statement = allocate_statement();
  if (statement == NULL) {
    return NULL;
  }
  statement->operation = operation;
  statement->destination = destination;
  statement->first_op = source1;
  statement->second_op = source2;
  statement->is_static = false;

  statement->next_statement = NULL;

  return statement;
}

inner_statement_t *destroy_statement(inner_statement_t *statement) {
  if (statement == NULL) {
    return NULL;
  }
  inner_statement_t *next_statement = statement->next_statement;

  free(statement);

  return next_statement;
}

void destroy_statement_list(inner_statement_t *statement) {
  while (statement != NULL) {
    statement = destroy_statement(statement);
  }
}
