/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file inner_code.h
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Data structure of inner code and interface specification
 *
 \*****************************************************************************/

#ifndef INNER_CODE_H
#define INNER_CODE_H

#include <stdbool.h>
#include <stdlib.h>

/*************************************\
 * Data structure elements definiton *
\*************************************/

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
  INNER_INT2STR,
  INNER_INT2FLOATS,
  INNER_FLOAT2INTS,
  INNER_INT2CHARS,
  INNER_STRI2INTS,
  INNER_FLOAT2STRS,
  INNER_INT2STRS,

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
} inner_statement_operation_t;

/**
 * @brief Types of symbols in inner code
 */
typedef enum {
  INNER_SYM_EMPTY,
  INNER_SYM_CONST,
  INNER_SYM_VAR,
  INNER_SYM_ARR,
  INNER_SYM_LABEL,
} inner_symbol_type_t;

/**
 * @brief Datatypes of constants in inner code
 */
typedef enum {
  INNER_NIL,
  INNER_BOOL,
  INNER_INT,
  INNER_FLOAT,
  INNER_STRING,
} inner_datatype_t;
/**
 * @brief Union for constant value in given datatype
 */
typedef union {
  char *inner_nil;
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
  bool owns_identifier;

  // possible types of symbols
  union {
    // variable / array / label
    char *identificator;

    // constant
    struct {
      inner_datatype_t datatype;
      inner_value_t value;
    } constant;
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

  bool is_static;

  // next statement
  struct inner_statement *next_statement;
} inner_statement_t;

/***********************\
 * Interface functions *
\***********************/

/**
 * @brief Creates empty symbol
 *
 * @return Returns the empty symbol
 */
inner_symbol_t create_empty_symbol();

/**
 * @brief Creates variable symbol with given parameters
 *
 * @param identificator Identificator of the variable (preallocated)
 *
 * @return Symbol with set parameters
 */
inner_symbol_t create_var_symbol(char *identificator);

/**
 * @brief Creates constant symbol with given parameters
 *
 * @param datatype Datatype of the constant
 * @param value Value of the constant - using datatype as the one provided,
 *              can be anything if datatype is null
 *
 * @return Symbol with set parameters
 */
inner_symbol_t create_const_symbol(inner_datatype_t datatype,
                                   inner_value_t value);

/**
 * @brief Creates array symbol with given parameters
 *
 * @param identificator Identificator of the array (preallocated)
 *
 * @return Symbol with set parameters
 */
inner_symbol_t create_arr_symbol(char *identificator);

/**
 * @brief Creates label symbol with given parameters
 *
 * @param identificator Identificator of the label (preallocated)
 *
 * @return Symbol with set parameters
 */
inner_symbol_t create_label_symbol(char *identificator);

/**
 * @brief Allocates memory for statement and fills its values using parameters
 *
 * @param operation Operation type of the statement (instruction in ifj26code)
 * @param destination Symbol that is the destination of the operation,
 *                    empty symbol if not needed
 * @param source1 Symbol which is the first (or only) operand of the operation
 *                or empty symbol if not needed
 * @param source2 Second operand or empty symbol if not needed
 *
 * @return Pointer to allocated statement or
 *         NULL if memory allocation failed or necesary operands were NULL
 */
inner_statement_t *create_statement(inner_statement_operation_t operation,
                                    inner_symbol_t destination,
                                    inner_symbol_t source1,
                                    inner_symbol_t source2);

/**
 * @brief Deallocates all memory held by the statement
 *        and returns the next statement pointer.
 *        Frees identifier only when  owns_identifier == true
 *        for respective symbols
 *
 * @param statement Pointer to the statement struct memory
 *                  is NOT set tu NULL -> caller mustn't access
 *                  memory that it points at
 *
 * @return Next statement pointer or
 *         NULL if the pointer at statement was NULL
 */
inner_statement_t *destroy_statement(inner_statement_t *statement);

/**
 * @brief Deallocates whole list of statements
 *
 * @param statement Pointer to the first statement
 *                  ! is not set to NULL
 */
void destroy_statement_list(inner_statement_t *statement);

/**
 * @brief Duplicates a statement and its entire chain of next statements
 *
 * @param statement Pointer to the head of the statement list to duplicate
 * @param out_tail [out] Pointer to the tail of the duplicated statement list
 *
 * @return A pointer to the head of the duplicated statement list or NULL if
 * failed
 */
inner_statement_t *duplicate_statement_list(inner_statement_t *statement,
                                            inner_statement_t **out_tail);

#endif
