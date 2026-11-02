/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file inner_code.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Data structure of inner code
 *
 \*****************************************************************************/

#ifndef INNER_CODE_H
#define INNER_CODE_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Enumeration of all possible inner code operations
 */
typedef enum {
  // Non-existent instruction - skip while printing result
  INNER_NOP_CUSTOM,

  // Frames and function calls instructions
  INNER_MOVE,
  INNER_CREATEFRAME,
  INNER_PUSHFRAME,
  INNER_POPFRAME,
  INNER_DEFVAR,
  INNER_CALL,
  INNER_RETURN,

  // Stack instructions
  INNER_PUSHS,
  INNER_POPS,
  INNER_CLEARS,

  // Aritmetic instructions
  INNER_ADD,
  INNER_SUB,
  INNER_MUL,
  INNER_DIV,
  INNER_IDIV,
  INNER_ADDS,
  INNER_SUBS,
  INNER_MULS,
  INNER_DIVS,
  INNER_IDIVS,

  // Relation instructions
  INNER_LT,
  INNER_GT,
  INNER_EQ,
  INNER_LTS,
  INNER_GTS,
  INNER_EQS,

  // Boolean instructions
  INNER_AND,
  INNER_OR,
  INNER_NOT,
  INNER_ANDS,
  INNER_ORS,
  INNER_NOTS,

  // Conversion instructions
  INNER_INT2FLOAT,
  INNER_FLOAT2INT,
  INNER_INT2CHAR,
  INNER_STRI2INT,
  INNER_FLOAT2STR,
  INNER_INT2STRING,
  INNER_INT2FLOATS,
  INNER_FLOAT2INTS,
  INNER_INT2CHARS,
  INNER_STRI2INTS,
  INNER_FLOAT2STRS,
  INNER_INT2STRINGS,

  // I/O instructions
  INNER_READ,
  INNER_WRITE,

  // String instructions
  INNER_CONCAT,
  INNER_STRLEN,
  INNER_GETCHAR,
  INNER_SETCHAR,

  // Array instructions
  INNER_NEWARRAY,
  INNER_ARRAYGET,
  INNER_ARRAYSET,

  // Datatype detection instructions
  INNER_TYPE,
  INNER_ISINT,
  INNER_TYPES,
  INNER_ISINTS,

  // Jump instructions
  INNER_LABEL,
  INNER_JUMP,
  INNER_JUMPIFEQ,
  INNER_JUMPIFNEQ,
  INNER_JUMPIFEQS,
  INNER_JUMPIFNEQS,
  INNER_EXIT,

  // Debug instructions
  INNER_BREAK,
  INNER_DPRINT

  // TODO others
} inner_statement_operation_t;

/**
 * @brief Types of symbols in inner code
 */
typedef enum {
  INNER_SYM_CONST,
  INNER_SYM_VAR,
  INNER_SYM_ARR,
  INNER_SYM_LABEL,
} inner_symbol_type_t;

/**
 * @brief Datatypes in inner code
 */
typedef enum {
  INNER_NIL,
  INNER_BOOL,
  INNER_INT,
  INNER_FLOAT,
  INNER_STRING,
  INNER_ARR_REF,
} inner_datatype_t;
/**
 * @brief Union for value in given datatype
 */
typedef union {
  bool inner_bool;
  long long int inner_int;
  double inner_float;
  char *inner_string;
} inner_value_t;

/**
 * @brief Symbols that can be used as instruction operands
 */
typedef struct inner_symbol {
  inner_symbol_type_t type;

  // possible types of symbols
  union {
    // variable data
    struct {
      char *identificator;
      inner_datatype_t datatype;
      inner_value_t value;
    } variable;

    // constant data
    struct {
      inner_datatype_t datatype;
      inner_value_t value;
    } constant;

    // array data
    struct {
      char *identificator;
      long long int ref_value;

      inner_datatype_t items_datatype;
      struct inner_symbol *items; // (array or const of given datatype)
    } array;

    // label data
    struct {
      char *identificator;
    } label;

  } data;

} inner_symbol_t;

/**
 * @brief Inner code statement
 */
typedef struct inner_statement {
  inner_statement_operation_t operation;

  // are not always used
  inner_symbol_t destination;
  inner_symbol_t first_op;
  inner_symbol_t second_op;

  // options for next state
  // (for optimalization purposes - moving through the inner code)

  // next statement
  struct inner_statement *next_statement;

  // alternative next statement if condition was not satisfied
  struct inner_statement *cond_unsat_next_statement;

} inner_statement_t;

#endif
