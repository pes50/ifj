/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file code_gen.c
 *
 * @author Tomáš Peňás      (xpenast00)
 *
 * @brief Implementation of code generator (3AC statements -> ifjcode26)
 *
\******************************************************************************/

#include "code_gen.h"
#include <string.h>

/********************\
 * Helper functions *
\********************/

#define ESCAPABLE_CHAR_LOWER_BOUND 0
#define ESCAPABLE_CHAR_UPPER_BOUND 32
#define HASHTAG_ASCII 35
#define BACKSLASH_ASCII 92

/**
 * @brief Prints string using ifjcode26 escape sequences
 *
 * @param string String with non-escaped characters or NULL (does nothing)
 * @param stream Where to print the data (stdout,stderr,file,...)
 */
void codegen_print_string(const char *string, FILE *stream) {
  if (string == NULL) {
    return;
  }
  size_t character = 0;
  while (string[character] != '\0') {
    // prints escapeable character as \xxx escape sequence
    if ((string[character] >= ESCAPABLE_CHAR_LOWER_BOUND &&
         string[character] <= ESCAPABLE_CHAR_UPPER_BOUND) ||
        string[character] == HASHTAG_ASCII ||
        string[character] == BACKSLASH_ASCII) {
      fprintf(stream, "\\%03d", string[character]);
      // other characters normaly
    } else {
      fprintf(stream, "%c", string[character]);
    }
    character++;
  }
}

/**
 * @brief Prints ifjcode26 instruction's name (inner code operation)
 *
 * @param operation The operation to print
 * @param stream Where to print the data (stdout,stderr,file,...)
 */
void codegen_print_operation(inner_statement_operation_t operation,
                             FILE *stream) {
  switch (operation) {
  // NOP is internal operation - is not printed
  case INNER_NOP_CUSTOM:
    break;
  // Frames and function calls instructions
  case INNER_MOVE:
    fprintf(stream, "MOVE");
    break;
  case INNER_CREATEFRAME:
    fprintf(stream, "CREATEFRAME");
    break;
  case INNER_PUSHFRAME:
    fprintf(stream, "PUSHFRAME");
    break;
  case INNER_POPFRAME:
    fprintf(stream, "POPFRAME");
    break;
  case INNER_DEFVAR:
    fprintf(stream, "DEFVAR");
    break;
  case INNER_CALL:
    fprintf(stream, "CALL");
    break;
  case INNER_RETURN:
    fprintf(stream, "RETURN");
    break;

  // Stack instructions
  case INNER_PUSHS:
    fprintf(stream, "PUSHS");
    break;
  case INNER_POPS:
    fprintf(stream, "POPS");
    break;
  case INNER_CLEARS:
    fprintf(stream, "CLEARS");
    break;

  // Aritmetic instructions
  case INNER_ADD:
    fprintf(stream, "ADD");
    break;
  case INNER_SUB:
    fprintf(stream, "SUB");
    break;
  case INNER_MUL:
    fprintf(stream, "MUL");
    break;
  case INNER_DIV:
    fprintf(stream, "DIV");
    break;
  case INNER_IDIV:
    fprintf(stream, "IDIV");
    break;
  case INNER_ADDS:
    fprintf(stream, "ADDS");
    break;
  case INNER_SUBS:
    fprintf(stream, "SUBS");
    break;
  case INNER_MULS:
    fprintf(stream, "MULS");
    break;
  case INNER_DIVS:
    fprintf(stream, "DIVS");
    break;
  case INNER_IDIVS:
    fprintf(stream, "IDIVS");
    break;

  // Relation instructions
  case INNER_LT:
    fprintf(stream, "LT");
    break;
  case INNER_GT:
    fprintf(stream, "GT");
    break;
  case INNER_EQ:
    fprintf(stream, "EQ");
    break;
  case INNER_LTS:
    fprintf(stream, "LTS");
    break;
  case INNER_GTS:
    fprintf(stream, "GTS");
    break;
  case INNER_EQS:
    fprintf(stream, "EQS");
    break;

  // Boolean instructions
  case INNER_AND:
    fprintf(stream, "AND");
    break;
  case INNER_OR:
    fprintf(stream, "OR");
    break;
  case INNER_NOT:
    fprintf(stream, "NOT");
    break;
  case INNER_ANDS:
    fprintf(stream, "ANDS");
    break;
  case INNER_ORS:
    fprintf(stream, "ORS");
    break;
  case INNER_NOTS:
    fprintf(stream, "NOTS");
    break;

  // Conversion instructions
  case INNER_INT2FLOAT:
    fprintf(stream, "INT2FLOAT");
    break;
  case INNER_FLOAT2INT:
    fprintf(stream, "FLOAT2INT");
    break;
  case INNER_INT2CHAR:
    fprintf(stream, "INT2CHAR");
    break;
  case INNER_STRI2INT:
    fprintf(stream, "STRI2INT");
    break;
  case INNER_FLOAT2STR:
    fprintf(stream, "FLOAT2STR");
    break;
  case INNER_INT2STR:
    fprintf(stream, "INT2STR");
    break;
  case INNER_INT2FLOATS:
    fprintf(stream, "INT2FLOATS");
    break;
  case INNER_FLOAT2INTS:
    fprintf(stream, "FLOAT2INTS");
    break;
  case INNER_INT2CHARS:
    fprintf(stream, "INT2CHARS");
    break;
  case INNER_STRI2INTS:
    fprintf(stream, "STRI2INTS");
    break;
  case INNER_FLOAT2STRS:
    fprintf(stream, "FLOAT2STRS");
    break;
  case INNER_INT2STRS:
    fprintf(stream, "INT2STRS");
    break;

    // I/O instructions
  case INNER_READ:
    fprintf(stream, "READ");
    break;
  case INNER_WRITE:
    fprintf(stream, "WRITE");
    break;

  // String instructions
  case INNER_CONCAT:
    fprintf(stream, "CONCAT");
    break;
  case INNER_STRLEN:
    fprintf(stream, "STRLEN");
    break;
  case INNER_GETCHAR:
    fprintf(stream, "GETCHAR");
    break;
  case INNER_SETCHAR:
    fprintf(stream, "SETCHAR");
    break;

  // Array instructions
  case INNER_NEWARRAY:
    fprintf(stream, "NEWARRAY");
    break;
  case INNER_ARRAYGET:
    fprintf(stream, "ARRAYGET");
    break;
  case INNER_ARRAYSET:
    fprintf(stream, "ARRAYSET");
    break;

  // Datatype detection instructions
  case INNER_TYPE:
    fprintf(stream, "TYPE");
    break;
  case INNER_ISINT:
    fprintf(stream, "ISINT");
    break;
  case INNER_TYPES:
    fprintf(stream, "TYPES");
    break;
  case INNER_ISINTS:
    fprintf(stream, "ISINTS");
    break;

  // Jump instructions
  case INNER_LABEL:
    fprintf(stream, "LABEL");
    break;
  case INNER_JUMP:
    fprintf(stream, "JUMP");
    break;
  case INNER_JUMPIFEQ:
    fprintf(stream, "JUMPIFEQ");
    break;
  case INNER_JUMPIFNEQ:
    fprintf(stream, "JUMPIFNEQ");
    break;
  case INNER_JUMPIFEQS:
    fprintf(stream, "JUMPIFEQS");
    break;
  case INNER_JUMPIFNEQS:
    fprintf(stream, "JUMPIFNEQS");
    break;
  case INNER_EXIT:
    fprintf(stream, "EXIT");
    break;

  // Debug instructions
  case INNER_BREAK:
    fprintf(stream, "BREAK");
    break;
  case INNER_DPRINT:
    fprintf(stream, "DPRINT");
    break;

  // Default case should not happen
  default:
    break;
  }
}

/**
 * @brief Prints constant's value on
 *
 * @param symbol The symbol containing the constant to print
 * @param stream Where to print the data (stdout,stderr,file,...)
 */
void codegen_print_constant(inner_symbol_t symbol, FILE *stream) {
  switch (symbol.data.constant.datatype) {
  case INNER_NIL:
    fprintf(stream, " nil@nil");
    break;
  case INNER_BOOL:
    if (symbol.data.constant.value.inner_bool) {
      fprintf(stream, " bool@true");
    } else {
      fprintf(stream, " bool@false");
    }
    break;
  case INNER_INT:
    fprintf(stream, " int@%lld", symbol.data.constant.value.inner_int);
    break;
  case INNER_FLOAT:
    fprintf(stream, " float@%a", symbol.data.constant.value.inner_float);
    break;
  case INNER_STRING:
    fprintf(stream, " string@");
    codegen_print_string(symbol.data.constant.value.inner_string, stream);
    break;
  }
}

/**
 * @brief Prints one symbol (variable/array/label/constant)
 *
 * @param symbol The symbol to print
 * @param stream Where to print the data (stdout,stderr,file,...)
 */
void codegen_print_symbol(inner_symbol_t symbol, FILE *stream) {
  if (symbol.type == INNER_SYM_EMPTY) {
    return;
  }

  if (symbol.type == INNER_SYM_CONST) {
    codegen_print_constant(symbol, stream);
  } else {
    fprintf(stream, " ");
    codegen_print_string(symbol.data.identificator, stream);
  }
}

/**
 * @brief Prints one line of ifjcode26 (one statement) and frees its memory
 *
 * @param statement The statement to print
 * @param stream Where to print the data (stdout,stderr,file,...)
 *
 * @return Pointer to next statement or NULL if it was the last one
 */
inner_statement_t *codegen_print_statement(inner_statement_t *statement,
                                           FILE *stream) {
  if (statement == NULL) {
    return NULL;
  }

  codegen_print_operation(statement->operation, stream);
  codegen_print_symbol(statement->destination, stream);
  codegen_print_symbol(statement->first_op, stream);
  codegen_print_symbol(statement->second_op, stream);
  fprintf(stream, "\n");

  return destroy_statement(statement);
}

/*******************************************\
 * Code generator interface implementation *
\*******************************************/

void codegen_print_program(inner_statement_t *statement, FILE *stream) {
  fprintf(stream, ".IFJcode26\n");

  inner_statement_t *next_statement = statement;
  do {
    next_statement = codegen_print_statement(next_statement, stream);
  } while (next_statement != NULL);
}
