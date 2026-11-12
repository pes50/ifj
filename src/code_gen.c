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
#include <stdio.h>
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
 */
void codegen_print_string(const char *string) {
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
      fprintf(stdout, "\\%03d", string[character]);
      // other characters normaly
    } else {
      fprintf(stdout, "%c", string[character]);
    }
    character++;
  }
}

/**
 * @brief Prints ifjcode26 instruction's name (inner code operation)
 *
 * @param operation The operation to print
 */
void codegen_print_operation(inner_statement_operation_t operation) {
  switch (operation) {
  // NOP is internal operation - is not printed
  case INNER_NOP_CUSTOM:
    break;
  // Frames and function calls instructions
  case INNER_MOVE:
    fprintf(stdout, "MOVE");
    break;
  case INNER_CREATEFRAME:
    fprintf(stdout, "CREATEFRAME");
    break;
  case INNER_PUSHFRAME:
    fprintf(stdout, "PUSHFRAME");
    break;
  case INNER_POPFRAME:
    fprintf(stdout, "POPFRAME");
    break;
  case INNER_DEFVAR:
    fprintf(stdout, "DEFVAR");
    break;
  case INNER_CALL:
    fprintf(stdout, "CALL");
    break;
  case INNER_RETURN:
    fprintf(stdout, "RETURN");
    break;

  // Stack instructions
  case INNER_PUSHS:
    fprintf(stdout, "PUSHS");
    break;
  case INNER_POPS:
    fprintf(stdout, "POPS");
    break;
  case INNER_CLEARS:
    fprintf(stdout, "CLEARS");
    break;

  // Aritmetic instructions
  case INNER_ADD:
    fprintf(stdout, "ADD");
    break;
  case INNER_SUB:
    fprintf(stdout, "SUB");
    break;
  case INNER_MUL:
    fprintf(stdout, "MUL");
    break;
  case INNER_DIV:
    fprintf(stdout, "DIV");
    break;
  case INNER_IDIV:
    fprintf(stdout, "IDIV");
    break;
  case INNER_ADDS:
    fprintf(stdout, "ADDS");
    break;
  case INNER_SUBS:
    fprintf(stdout, "SUBS");
    break;
  case INNER_MULS:
    fprintf(stdout, "MULS");
    break;
  case INNER_DIVS:
    fprintf(stdout, "DIVS");
    break;
  case INNER_IDIVS:
    fprintf(stdout, "IDIVS");
    break;

  // Relation instructions
  case INNER_LT:
    fprintf(stdout, "LT");
    break;
  case INNER_GT:
    fprintf(stdout, "GT");
    break;
  case INNER_EQ:
    fprintf(stdout, "EQ");
    break;
  case INNER_LTS:
    fprintf(stdout, "LTS");
    break;
  case INNER_GTS:
    fprintf(stdout, "GTS");
    break;
  case INNER_EQS:
    fprintf(stdout, "EQS");
    break;

  // Boolean instructions
  case INNER_AND:
    fprintf(stdout, "AND");
    break;
  case INNER_OR:
    fprintf(stdout, "OR");
    break;
  case INNER_NOT:
    fprintf(stdout, "NOT");
    break;
  case INNER_ANDS:
    fprintf(stdout, "ANDS");
    break;
  case INNER_ORS:
    fprintf(stdout, "ORS");
    break;
  case INNER_NOTS:
    fprintf(stdout, "NOTS");
    break;

  // Conversion instructions
  case INNER_INT2FLOAT:
    fprintf(stdout, "INT2FLOAT");
    break;
  case INNER_FLOAT2INT:
    fprintf(stdout, "FLOAT2INT");
    break;
  case INNER_INT2CHAR:
    fprintf(stdout, "INT2CHAR");
    break;
  case INNER_STRI2INT:
    fprintf(stdout, "STRI2INT");
    break;
  case INNER_FLOAT2STR:
    fprintf(stdout, "FLOAT2STR");
    break;
  case INNER_INT2STR:
    fprintf(stdout, "INT2STR");
    break;
  case INNER_INT2FLOATS:
    fprintf(stdout, "INT2FLOATS");
    break;
  case INNER_FLOAT2INTS:
    fprintf(stdout, "FLOAT2INTS");
    break;
  case INNER_INT2CHARS:
    fprintf(stdout, "INT2CHARS");
    break;
  case INNER_STRI2INTS:
    fprintf(stdout, "STRI2INTS");
    break;
  case INNER_FLOAT2STRS:
    fprintf(stdout, "FLOAT2STRS");
    break;
  case INNER_INT2STRS:
    fprintf(stdout, "INT2STRS");
    break;

    // I/O instructions
  case INNER_READ:
    fprintf(stdout, "READ");
    break;
  case INNER_WRITE:
    fprintf(stdout, "WRITE");
    break;

  // String instructions
  case INNER_CONCAT:
    fprintf(stdout, "CONCAT");
    break;
  case INNER_STRLEN:
    fprintf(stdout, "STRLEN");
    break;
  case INNER_GETCHAR:
    fprintf(stdout, "GETCHAR");
    break;
  case INNER_SETCHAR:
    fprintf(stdout, "SETCHAR");
    break;

  // Array instructions
  case INNER_NEWARRAY:
    fprintf(stdout, "NEWARRAY");
    break;
  case INNER_ARRAYGET:
    fprintf(stdout, "ARRAYGET");
    break;
  case INNER_ARRAYSET:
    fprintf(stdout, "ARRAYSET");
    break;

  // Datatype detection instructions
  case INNER_TYPE:
    fprintf(stdout, "TYPE");
    break;
  case INNER_ISINT:
    fprintf(stdout, "ISINT");
    break;
  case INNER_TYPES:
    fprintf(stdout, "TYPES");
    break;
  case INNER_ISINTS:
    fprintf(stdout, "ISINTS");
    break;

  // Jump instructions
  case INNER_LABEL:
    fprintf(stdout, "LABEL");
    break;
  case INNER_JUMP:
    fprintf(stdout, "JUMP");
    break;
  case INNER_JUMPIFEQ:
    fprintf(stdout, "JUMPIFEQ");
    break;
  case INNER_JUMPIFNEQ:
    fprintf(stdout, "JUMPIFNEQ");
    break;
  case INNER_JUMPIFEQS:
    fprintf(stdout, "JUMPIFEQS");
    break;
  case INNER_JUMPIFNEQS:
    fprintf(stdout, "JUMPIFNEQS");
    break;
  case INNER_EXIT:
    fprintf(stdout, "EXIT");
    break;

  // Debug instructions
  case INNER_BREAK:
    fprintf(stdout, "BREAK");
    break;
  case INNER_DPRINT:
    fprintf(stdout, "DPRINT");
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
 */
void codegen_print_constant(inner_symbol_t symbol) {
  switch (symbol.data.constant.datatype) {
  case INNER_NIL:
    fprintf(stdout, " nil@nil");
    break;
  case INNER_BOOL:
    if (symbol.data.constant.value.inner_bool) {
      fprintf(stdout, " bool@true");
    } else {
      fprintf(stdout, " bool@false");
    }
    break;
  case INNER_INT:
    fprintf(stdout, " int@%lld", symbol.data.constant.value.inner_int);
    break;
  case INNER_FLOAT:
    fprintf(stdout, " float@%a", symbol.data.constant.value.inner_float);
    break;
  case INNER_STRING:
    fprintf(stdout, " string@");
    codegen_print_string(symbol.data.constant.value.inner_string);
    break;
  }
}

/**
 * @brief Prints one symbol (variable/array/label/constant)
 *
 * @param symbol The symbol to print
 */
void codegen_print_symbol(inner_symbol_t symbol) {
  if (symbol.type == INNER_SYM_EMPTY) {
    return;
  }

  if (symbol.type == INNER_SYM_CONST) {
    codegen_print_constant(symbol);
  } else {
    fprintf(stdout, " ");
    codegen_print_string(symbol.data.identificator);
  }
}

/**
 * @brief Prints one line of ifjcode26 (one statement) and frees its memory
 *
 * @param statement The statement to print
 *
 * @return Pointer to next statement or NULL if it was the last one
 */
inner_statement_t *codegen_print_statement(inner_statement_t *statement) {
  if (statement == NULL) {
    return NULL;
  }

  codegen_print_operation(statement->operation);
  codegen_print_symbol(statement->destination);
  codegen_print_symbol(statement->first_op);
  codegen_print_symbol(statement->second_op);
  fprintf(stdout, "\n");

  return destroy_statement(statement);
}

/*******************************************\
 * Code generator interface implementation *
\*******************************************/

void codegen_print_program(inner_statement_t *statement) {
  inner_statement_t *next_statement = statement;
  do {
    next_statement = codegen_print_statement(next_statement);
  } while (next_statement != NULL);
}
