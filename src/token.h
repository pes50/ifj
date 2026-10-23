/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file token.h
 *
 * @author Simon Smid      (xsmidsi00)
 *
 * @brief Definition of the token structure
 *
\*****************************************************************************/

#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Enumeration of all token types
 */
typedef enum {
  // Error token
  TOK_ERR,

  // Identifier
  TOK_IDENTIFIER,

  // Key words
  TOK_KW_DEF,
  TOK_KW_DO,
  TOK_KW_DOUBLE,
  TOK_KW_ELSE,
  TOK_KW_IF,
  TOK_KW_IMPORT,
  TOK_KW_INT,
  TOK_KW_RETURN,
  TOK_KW_STRING,
  TOK_KW_THEN,
  TOK_KW_UNIT,
  TOK_KW_VAL,
  TOK_KW_VAR,
  TOK_KW_WHILE,
  // Extensions
  TOK_KW_BOOLEAN,   // BOOLTHEN
  TOK_KW_CASE,      // ALGEBDT
  TOK_KW_CLASS,     // ALGEBDT
  TOK_KW_EXTENSION, // EXTKW
  TOK_KW_FOR,       // CONTROL
  TOK_KW_LIST,      // LISTS
  TOK_KW_MATCH,     // ALGEBDT
  TOK_KW_TYPE,      // ALGEBDT

  // Literals
  TOK_LIT_INT,      // 123, 1_000_000
  TOK_LIT_DOUBLE,   // 1.5, .5, 1e10, 2.5E-3d
  TOK_LIT_STRING,   // "..."
  TOK_LIT_M_STRING, // """..."""
  // Extensions
  TOK_LIT_TRUE,                 // true BOOLTHEN
  TOK_LIT_FALSE,                // false BOOLTHEN
  TOK_LIT_INTERP_STRING_PART,   // s"... INTERPOLATEDS
  TOK_LIT_INTERP_STRING_END,    // " INTERPOLATEDS
  TOK_LIT_INTERP_M_STRING_PART, // s""" INTERPOLATEDS
  TOK_LIT_INTERP_M_STRING_END,  // """ INTERPOLATEDS
  TOK_EXPRESSION_START,         // ${ INTERPOLATEDS
  TOK_EXPRESSION_END,           // } INTERPOLATEDS

  // Layout
  TOK_EOF,    // End of file
  TOK_EOL,    // End of logical line
  TOK_INDENT, // Increase in indentation level
  TOK_DEDENT, // Decrease in indentation level

  // Operators
  TOK_UNARY_PLUS,   // +a
  TOK_BINARY_PLUS,  // a + b
  TOK_UNARY_MINUS,  // -a
  TOK_BINARY_MINUS, // a - b
  TOK_MUL,          // *
  TOK_DIV,          // /
  TOK_LOWER_T,      // <
  TOK_LOWER_EQ,     // <=
  TOK_GREATER_T,    // >
  TOK_GREATER_EQ,   // >=
  TOK_EQUAL,        // ==
  TOK_NOT_EQ,       // !=
  TOK_ASSIGN,       // =
  // Extensions
  TOK_AND,         // && BOOLTHEN
  TOK_OR,          // || BOOLTHEN
  TOK_NOT,         // ! BOOLTHEN
  TOK_PIPE,        // | ALGEBDT
  TOK_ARROW_RIGHT, // => LAMBDAS and ALGEBDT
  TOK_ARROW_LEFT,  // <- CONTROL
  TOK_CONCAT_LIST, // ++ LISTS

  // Special symbols
  TOK_L_PAREN,   // (
  TOK_R_PAREN,   // )
  TOK_COMMA,     // ,
  TOK_COLON,     // :
  TOK_SEMICOLON, // ;
  TOK_AT_MAIN,   // @main
  // Extensions
  TOK_DOT,        // . VALMEM, LISTS, EXTKW, ALGEBDT
  TOK_L_BRACKET,  // [ LISTS
  TOK_R_BRACKET,  // ] LISTS
  TOK_UNDERSCORE, // _ ALGEBDT
} token_type_t;

/**
 * @brief Struct for a token
 */
typedef struct token {
  token_type_t type;
  const char *value;
  size_t length;
} token_t;

#endif
