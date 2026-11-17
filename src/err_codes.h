/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file err_codes.h
 *
 * @author Matous Jahoda      (xjahodm00)
 *
 * @brief Contains error codes enum
 *
\*****************************************************************************/

#pragma once

/**
 * @brief Enumeration of all error codes
 */
typedef enum err_codes {
  NO_ERR = 0,           // successful execution
  LEXICAL_ERR = 1,      // malformed token, comment, or indentation
  SYNTAX_ERR = 2,       // bad syntax
  UNDEF_SYMBOL_ERR = 3, // use of undefined symbol; missing @main entry
  REDEF_SYMBOL_ERR = 4, // illegal re‑definition
  ARG_MISMATCH_ERR = 5, // wrong number or type of arguments in a function call
  TYPE_COMPAT_ERR = 6,  // type‑compatibility/derivation errors
  ASSIGN_CONST_ERR = 7, // assign to a read‑only variable or formal parameter
  OTHER_SEMANTIC_ERR = 10, // other semantic errors
  INTERNAL_ERR = 99,       // internal compiler error
} err_code_t;

#ifdef DEBUG
#define RETURN_ERR(err)                                                        \
  do {                                                                         \
    int _err_val = (err);                                                      \
    if (_err_val == INTERNAL_ERR) {                                            \
      __asm__ volatile("int $3");                                              \
    }                                                                          \
    return _err_val;                                                           \
  } while (0)
#else
#define RETURN_ERR(err) return (err)
#endif