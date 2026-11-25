/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file lexer.c
 *
 * @author Simon Šmíd      (xsmidsi00)
 *
 * @brief Implementation of lexer.
 *
\*****************************************************************************/

#include "lexer.h"
#include "dynstr.h"
#include "expresion_stack.h"
#include "indent_stack.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define LEXER_BUF_SIZE (LEXER_MAX_LOOKAHEAD + 1)
#define LEXER_AT_MAIN "main"

#define IS_ASCII(c) (((c) >= ' ' && (c) <= '~') || (c) == '\t')

/**
 * @brief Dictionary for the keywords.
 */
typedef struct {
  char *key_word;
  token_type_t type;
} dictionary_t;

// Dictionary of the the keywords.
static const dictionary_t keywords[] = {
    {"def", TOK_KW_DEF},         {"do", TOK_KW_DO},
    {"Double", TOK_KW_DOUBLE},   {"else", TOK_KW_ELSE},
    {"if", TOK_KW_IF},           {"import", TOK_KW_IMPORT},
    {"Int", TOK_KW_INT},         {"return", TOK_KW_RETURN},
    {"String", TOK_KW_STRING},   {"then", TOK_KW_THEN},
    {"Unit", TOK_KW_UNIT},       {"val", TOK_KW_VAL},
    {"var", TOK_KW_VAR},         {"while", TOK_KW_WHILE},
    {"Boolean", TOK_KW_BOOLEAN}, {"case", TOK_KW_CASE},
    {"class", TOK_KW_CLASS},     {"extension", TOK_KW_EXTENSION},
    {"for", TOK_KW_FOR},         {"List", TOK_KW_LIST},
    {"match", TOK_KW_MATCH},     {"type", TOK_KW_TYPE},
    {"true", TOK_LIT_TRUE},      {"false", TOK_LIT_FALSE},
};

/**
 * @brief Enumeration of lexer states.
 */
typedef enum {
  // States achievable from S_START
  S_START,
  S_START_AT_MAIN,
  S_S_PREFIX,
  S_ID,
  S_UNDERSCORE,
  S_MINUS,
  S_PLUS,
  S_DIV,
  S_LOWER_THAN,
  S_GREATER_THAN,
  S_EQUAL,
  S_NOT,
  S_VERTICAL_BAR,
  S_AMPERSAND,
  S_TOK_DOT,
  S_NUM,
  S_ZERO,
  S_ZERO_DIGITS,
  S_STR,
  // NUMBERS, DECIMALS
  S_NUM_UNDERSCORE,
  S_DOT,
  S_E,
  S_SIGNED_EXP,
  S_EXP,
  S_UNDERSCORE_EXP,
  S_DEC,
  S_DEC_UNDERSCORE,
  // COMMENTS
  S_LINE_COMMENT,
  S_BLOCK_COMMENT,
  S_NESTED_BLOCK_COMMENT,
  S_BLOCK_COMMENT_END_START,
  S_BLOCK_COMMENT_END,
  // STRING
  S_START_STRING,
  S_ESCAPE_SEQ,
  S_ML,
  S_START_ML,
  S_END_ML,
  S_END_ML_2,
  // INTERPOLATED STRING
  S_S_STR,
  S_START_S_STR,
  S_S_ESC_SEQ,
  S_DOLAR_STATE,
  S_DOLAR_NEXT,
  S_READ_ID,
  S_S_ML,
  S_START_S_ML,
  // S_DOLAR_STATE_ML,
  S_END_S_ML,
  S_END_S_ML_2,
  // INDENTATION
  S_LINE_START,
  S_LINE_SLASH,
  S_LINE,
} lexer_state_t;

// Variables used for work with tokens.
static token_t token_buffer[LEXER_BUF_SIZE];
static size_t token_buf_head = 0;
static size_t token_buf_count = 0;
static token_type_t token_prev_type = TOK_EMPTY;

// Variables used to decide whether the lexer is in interpolated string.
static bool interpolated_ml_str = false;
static bool in_interpolated_str = false;
static expresion_stack_t expresion_stack;
static bool expresion_stack_initialized = false;

// Variables used for work with indentation stack.
static indent_stack_t indent_stack;
static bool indent_stack_initialized = false;
static size_t indent_level = 0;

static lexer_state_t next_state = S_LINE_START;
static FILE *input_stream = NULL;

// "by", "until", "to" can be identifier. Because of that we have to decide
// wheter it was used inside for loop or not.
static bool inside_for_loop = false;
static bool token_prev_for_kw = false;

// Set true when block comment containing LF ended.
static bool lf_inside_ml_com = false;

// Tracks open parentheses and brackets and indentation of line where the
// expression started.
static size_t bracket_depth = 0;
static size_t bracket_start_indent = 0;

/**
 * @brief Determine whether the token should have value.
 *
 * @param type Type of token.
 *
 * @return true if the token should have value.
 *         false if the token shouldn't.
 */
static bool has_value(token_type_t type) {
  switch (type) {
  case TOK_IDENTIFIER:
  case TOK_LIT_INT:
  case TOK_LIT_DOUBLE:
  case TOK_LIT_STRING:
  case TOK_LIT_M_STRING:
  case TOK_LIT_INTERP_STRING_PART:
  case TOK_LIT_INTERP_STRING_END:
  case TOK_LIT_INTERP_M_STRING_PART:
  case TOK_LIT_INTERP_M_STRING_END:
    return true;
  default:
    return false;
  }
}

/**
 * @brief Determine whether the sign should be binary or not based on previous
 * token type.
 *
 * @return true if the sign should be binary
 *         false if the sign shouldn't.
 */
static bool is_binary_operator(void) {
  if (token_prev_for_kw) {
    return false;
  }

  switch (token_prev_type) {
  case TOK_IDENTIFIER:
  case TOK_LIT_INT:
  case TOK_LIT_DOUBLE:
  case TOK_LIT_STRING:
  case TOK_LIT_M_STRING:
  case TOK_LIT_TRUE:
  case TOK_LIT_FALSE:
  case TOK_LIT_INTERP_STRING_END:
  case TOK_LIT_INTERP_M_STRING_END:
  case TOK_R_PAREN:
  case TOK_R_BRACKET:
    return true;
  default:
    return false;
  }
}

/**
 * @brief Determine whether the given string is keyword.
 *
 * @param word A string to be checked.
 *
 * @return TOK_IDENTIFIER if the token isn't keyword.
 *         otherwise return token type of the matching keyword.
 */
static token_type_t is_keyword(const char *word) {
  size_t num_of_kws = sizeof(keywords) / sizeof(keywords[0]);

  for (size_t index = 0; index < num_of_kws; index++) {
    if (strcmp(word, keywords[index].key_word) == 0) {
      return keywords[index].type;
    }
  }
  return TOK_IDENTIFIER;
}

/**
 * @brief Check if the given token type can exist after the given eol_code.
 *
 * @param type Token type.
 * @param eol_code Code specifying whether end of line is expected, required or
 * forbidden.
 *
 * @return NO_ERR when token type can exist after the given eol_code.
 *         SYNTAX_ERR when not.
 */
static err_code_t check_eol_code(token_type_t *type, eol_code_t eol_code) {
  switch (eol_code) {
  case FORBIDDEN:
    if (*type == TOK_EOL) {
      return SYNTAX_ERR;
    }
    break;

  case REQUIRED:
    if (*type != TOK_EOL) {
      return SYNTAX_ERR;
    }
    break;

  case ALLOWED:
  default:
    break;
  }
  return NO_ERR;
}

/**
 * @brief Skips the rest of a block comment.
 *
 * @param had_newline Set to true if a newline was found inside the comment.
 *
 * @return NO_ERR on success.
 *         LEXICAL_ERR if the comment is not terminated.
 */
static err_code_t skip_block_comment(bool *had_newline) {
  size_t depth = 1;
  int prev = 0;
  int ignored_char;

  while (depth > 0) {
    ignored_char = getc(input_stream);
    if (ignored_char == EOF) {
      return LEXICAL_ERR;
    }
    if (ignored_char == '\n') {
      *had_newline = true;
    }

    if (prev == '/' && ignored_char == '*') {
      depth++;
      prev = 0;
    } else if (prev == '*' && ignored_char == '/') {
      depth--;
      prev = 0;
    } else {
      prev = ignored_char;
    }
  }
  return NO_ERR;
}

/**
 * @brief Scans upcoming stream of data and determines token's type and it's
 * value.
 *
 * @param output_val Value of the token's attribute.
 * @param type       Pointer to token type which the lexer should determine.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on internal error.
 *         SYNTAX_ERR on syntax failure.
 *         LEXICAL_ERR on syntax failure.
 */
static err_code_t lexer_scan_automat(dynstr_t *output_val, token_type_t *type) {
  lexer_state_t state = next_state;
  next_state = S_START;
  bool lexer_run = true;
  bool has_newline = false;
  bool tab_on_line = false;
  bool comment_seen = false;
  size_t comment_depth_cnt = 0;
  int character;

  // Initialization of indentation stack.
  if (!indent_stack_initialized) {
    if (indent_stack_init(&indent_stack) != NO_ERR) {
      return INTERNAL_ERR;
    }
    indent_stack_initialized = true;
  }

  // Initialization of epresion stack.
  if (!expresion_stack_initialized) {
    if (expresion_stack_init(&expresion_stack) != NO_ERR) {
      return INTERNAL_ERR;
    }
    expresion_stack_initialized = true;
  }

  // Main loop
  while (lexer_run) {

    if (state == S_START && in_interpolated_str) {
      if (interpolated_ml_str) {
        state = S_START_S_ML;
      } else {
        state = S_START_S_STR;
      }
    }

    character = getc(input_stream);

    switch (state) {

    // stav start
    case S_START:

      switch (character) {

      // Whitespace in the middle of a line is skipped. (indentation is
      // evaluated only at the beginning of a line)
      case ' ':
        state = S_START;
        break;

      case '\t':
        state = S_START;
        break;

      case '\r':
        state = S_START;
        break;

      case ',':
        *type = TOK_COMMA;
        lexer_run = false;
        break;

      case ';':
        *type = TOK_SEMICOLON;
        lexer_run = false;
        break;

      case EOF:
        if (!expresion_stack_empty(&expresion_stack)) {
          return LEXICAL_ERR;
        }

        // A file not ending with '\n' still needs a final EOL.
        if (token_prev_type != TOK_EOL && token_prev_type != TOK_EMPTY &&
            token_prev_type != TOK_INDENT && token_prev_type != TOK_DEDENT &&
            token_prev_type != TOK_EOF) {
          *type = TOK_EOL;
        } else if (indent_stack_top(&indent_stack) > 0) {
          // Close all remaining indentations. (one DEDENT per scan).
          indent_stack_pop(&indent_stack);
          *type = TOK_DEDENT;
          next_state = S_START;
        } else {
          *type = TOK_EOF;
        }
        lexer_run = false;
        break;

      // The scan after '\n' must start by measuring
      // the indentation of the new line.
      case '\n':
        if (!expresion_stack_empty(&expresion_stack)) {
          return LEXICAL_ERR;
        }

        indent_level = 0;
        if (bracket_depth > 0) {
          state = S_LINE_START;
          break;
        }

        *type = TOK_EOL;
        next_state = S_LINE_START;
        lexer_run = false;
        break;

      case '(':
        bracket_depth++;
        *type = TOK_L_PAREN;
        lexer_run = false;
        break;

      case ')':
        if (bracket_depth > 0) {
          bracket_depth--;
        }
        *type = TOK_R_PAREN;
        lexer_run = false;
        break;

      case '[':
        bracket_depth++;
        *type = TOK_L_BRACKET;
        lexer_run = false;
        break;

      case ']':
        if (bracket_depth > 0) {
          bracket_depth--;
        }
        *type = TOK_R_BRACKET;
        lexer_run = false;
        break;

      case '}':
        // '}' closes an interpolation expression ${ ... }
        // The lexer continues reading the interpolated string.
        if (expresion_stack_empty(&expresion_stack)) {
          return LEXICAL_ERR;
        }

        interpolated_ml_str = expresion_stack_pop(&expresion_stack);
        in_interpolated_str = true;
        *type = TOK_EXPRESSION_END;
        lexer_run = false;
        break;

      // @ (@main)
      case '@':
        state = S_START_AT_MAIN;
        break;

      // 's' can be the prefix of interpolated string or begining of an
      // identifier. This is decided in the S_S_PREFIX state.
      case 's':
        state = S_S_PREFIX;
        break;

      // '_' can be token on it's own or an indetifier
      // when followed by more characters.
      case '_':
        state = S_UNDERSCORE;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      case '-':
        state = S_MINUS;
        break;

      case '+':
        state = S_PLUS;
        break;

      // Division or the beginning of a comment.
      case '/':
        state = S_DIV;
        break;

      case '*':
        *type = TOK_MUL;
        lexer_run = false;
        break;

      case '<':
        state = S_LOWER_THAN;
        break;

      case '>':
        state = S_GREATER_THAN;
        break;

      case '=':
        state = S_EQUAL;
        break;

      case '!':
        state = S_NOT;
        break;

      case ':':
        *type = TOK_COLON;
        lexer_run = false;
        break;

      case '|':
        state = S_VERTICAL_BAR;
        break;

      case '&':
        state = S_AMPERSAND;
        break;

      // '.' is TOK_DOT or the start of a decimal number.
      case '.':
        state = S_TOK_DOT;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      case '0':
        state = S_ZERO;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      case '"':
        state = S_STR;
        break;

      default:
        // Identifier starts with a-z without 's', A-Z or integer 1-9
        if ((character >= 'a' && character <= 'z' && character != 's') ||
            (character >= 'A' && character <= 'Z')) {
          state = S_ID;
        } else if (character >= '1' && character <= '9') {
          state = S_NUM;
        } else {
          return LEXICAL_ERR;
        }

        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;
      }
      break;

      /**
       * FA - OPERATORS
       */

    // Whether the sign is binary or unary is  decided by the previous token.
    case S_MINUS:
      ungetc(character, input_stream);
      if (is_binary_operator()) {
        *type = TOK_BINARY_MINUS;
      } else {
        *type = TOK_UNARY_MINUS;
      }
      lexer_run = false;
      break;

    case S_PLUS:
      if (character == '+') {
        *type = TOK_CONCAT_LIST;
      } else {
        ungetc(character, input_stream);

        if (is_binary_operator()) {
          *type = TOK_BINARY_PLUS;
        } else {
          *type = TOK_UNARY_PLUS;
        }
      }
      lexer_run = false;
      break;

    case S_LOWER_THAN:
      if (character == '=') {
        *type = TOK_LOWER_EQ;
      } else if (character == '-') {
        *type = TOK_ARROW_LEFT;
      } else {
        ungetc(character, input_stream);
        *type = TOK_LOWER_T;
      }
      lexer_run = false;
      break;

    case S_GREATER_THAN:
      if (character == '=') {
        *type = TOK_GREATER_EQ;
      } else {
        ungetc(character, input_stream);
        *type = TOK_GREATER_T;
      }
      lexer_run = false;
      break;

    case S_EQUAL:
      if (character == '=') {
        *type = TOK_EQUAL;
      } else if (character == '>') {
        *type = TOK_ARROW_RIGHT;
      } else {
        ungetc(character, input_stream);
        *type = TOK_ASSIGN;
      }
      lexer_run = false;
      break;

    case S_NOT:
      if (character == '=') {
        *type = TOK_NOT_EQ;
      } else {
        ungetc(character, input_stream);
        *type = TOK_NOT;
      }
      lexer_run = false;
      break;

    case S_VERTICAL_BAR:
      if (character == '|') {
        *type = TOK_OR;
      } else {
        ungetc(character, input_stream);
        *type = TOK_PIPE;
      }
      lexer_run = false;
      break;

    case S_AMPERSAND:
      if (character == '&') {
        *type = TOK_AND;
      } else {
        // '&' not followed by another '&' is lexical error.
        return LEXICAL_ERR;
      }
      lexer_run = false;
      break;

      /**
       *  FA - IDENTIFIERS, KEYWORDS, _, @main
       */

    case S_START_AT_MAIN:
      for (size_t index = 0; LEXER_AT_MAIN[index] != '\0'; index++) {
        if (index > 0) {
          character = getc(input_stream);
        }
        if (character != LEXER_AT_MAIN[index]) {
          return LEXICAL_ERR;
        }
      }

      // Identifier cannot exist right after "@main".
      character = getc(input_stream);
      if (isalnum(character) || character == '_') {
        return LEXICAL_ERR;
      }
      ungetc(character, input_stream);

      *type = TOK_AT_MAIN;
      lexer_run = false;
      break;

    case S_S_PREFIX:
      if (character == '"') {
        in_interpolated_str = true;
        interpolated_ml_str = false;
        state = S_S_STR;
      } else if (isalnum(character) || character == '_') {
        // 's' is not stored from S_START in case its prefix of interpolated
        // string, which is not stored. So it needs to be stored in value in
        // addition to next character.
        if (dynstr_append_char(output_val, 's') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_ID;
      } else {
        // 's' can be identifier on its own.
        ungetc(character, input_stream);
        if (dynstr_append_char(output_val, 's') != NO_ERR) {
          return INTERNAL_ERR;
        }
        *type = TOK_IDENTIFIER;
        lexer_run = false;
      }
      break;

    case S_UNDERSCORE:
      if (isalnum(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_ID;
      } else {
        ungetc(character, input_stream);
        *type = TOK_UNDERSCORE;
        lexer_run = false;
      }
      break;

    case S_ID:
      if (isalnum(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_ID;
      } else {
        ungetc(character, input_stream);
        // Decide whether the value is keyword or identifier.
        *type = is_keyword(output_val->data);
        lexer_run = false;
      }
      break;

      /**
       * FA - NUMBERS, DECIMALS
       */

    case S_NUM:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_NUM;
      } else if (character == '_') {
        state = S_NUM_UNDERSCORE;
      } else if (character == '.') {
        int next = getc(input_stream);
        ungetc(next, input_stream);
        if (next >= '0' && next <= '9') {
          if (dynstr_append_char(output_val, '.') != NO_ERR)
            return INTERNAL_ERR;
          state = S_DOT;
        } else {
          *type = TOK_LIT_INT;
          next_state = S_TOK_DOT;
          lexer_run = false;
        }
      } else if (character == 'e' || character == 'E') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_E;
      } else {
        if (isalpha(character)) {
          return LEXICAL_ERR;
        }
        ungetc(character, input_stream);
        *type = TOK_LIT_INT;
        lexer_run = false;
      }
      break;

    case S_ZERO:
      if (character == '.') {
        int next = getc(input_stream);
        ungetc(next, input_stream);
        if (next >= '0' && next <= '9') {
          if (dynstr_append_char(output_val, '.') != NO_ERR)
            return INTERNAL_ERR;
          state = S_DOT;
        } else {
          *type = TOK_LIT_INT;
          next_state = S_TOK_DOT;
          lexer_run = false;
        }
      } else if (character == 'e' || character == 'E') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_E;
      } else if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_ZERO_DIGITS;
      } else if (character == '_') {
        return LEXICAL_ERR;
      } else {
        if (isalpha(character)) {
          return LEXICAL_ERR;
        }
        ungetc(character, input_stream);
        *type = TOK_LIT_INT;
        lexer_run = false;
      }
      break;

    case S_ZERO_DIGITS:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
      } else if (character == '.') {
        int next = getc(input_stream);
        ungetc(next, input_stream);
        if (!(next >= '0' && next <= '9')) {
          return LEXICAL_ERR;
        }
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DOT;
      } else if (character == 'e' || character == 'E') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_E;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_TOK_DOT:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DEC;
      } else {
        ungetc(character, input_stream);
        *type = TOK_DOT;
        lexer_run = false;
      }
      break;

    case S_NUM_UNDERSCORE:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_NUM;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_DOT:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DEC;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_E:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_EXP;
      } else if (character == '+' || character == '-') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_SIGNED_EXP;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_SIGNED_EXP:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_EXP;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_EXP:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_EXP;
      } else if (character == '_') {
        state = S_UNDERSCORE_EXP;
      } else if (character == 'D' || character == 'd') {
        int next = getc(input_stream);
        if (isalnum(next) || next == '_') {
          return LEXICAL_ERR;
        }
        ungetc(next, input_stream);
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      } else {
        if (isalpha(character)) {
          return LEXICAL_ERR;
        }
        ungetc(character, input_stream);
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      }
      break;

    case S_UNDERSCORE_EXP:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_EXP;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_DEC:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DEC;
      } else if (character == '_') {
        state = S_DEC_UNDERSCORE;
      } else if (character == 'D' || character == 'd') {
        int next = getc(input_stream);
        if (isalnum(next) || next == '_') {
          return LEXICAL_ERR;
        }
        ungetc(next, input_stream);
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      } else if (character == 'e' || character == 'E') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_E;
      } else {
        if (isalpha(character)) {
          return LEXICAL_ERR;
        }
        ungetc(character, input_stream);
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      }
      break;

    case S_DEC_UNDERSCORE:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DEC;
      } else {
        return LEXICAL_ERR;
      }
      break;

      /**
       * FA - COMMENTS
       */

    case S_DIV:
      if (character == '/') {
        state = S_LINE_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT;
      } else {
        ungetc(character, input_stream);
        *type = TOK_DIV;
        lexer_run = false;
      }
      break;

    case S_LINE_COMMENT:
      if (character == '\n' || character == EOF) {
        // Leave EOF in the stream so the lexer
        // can properly handle it on the next call.
        if (character == EOF) {
          ungetc(character, input_stream);
        }

        indent_level = 0;

        if (bracket_depth > 0 && character == '\n') {
          state = S_LINE_START;
        } else {
          *type = TOK_EOL;
          next_state = S_LINE_START;
          lexer_run = false;
        }
      } else {
        state = S_LINE_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT:
      if (character == '/') {
        state = S_NESTED_BLOCK_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT_END_START;
      } else if (character == '\n') {
        has_newline = true;
      } else if (character == EOF) {
        return LEXICAL_ERR;
      }
      break;

    case S_NESTED_BLOCK_COMMENT:
      if (character == '*') {
        comment_depth_cnt++;
        state = S_BLOCK_COMMENT;
      } else {
        ungetc(character, input_stream);
        state = S_BLOCK_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT_END_START:
      if (character == '*') {
        state = S_BLOCK_COMMENT_END_START;
      } else if (character == '/') {
        state = S_BLOCK_COMMENT_END;
      } else {
        ungetc(character, input_stream);
        state = S_BLOCK_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT_END:
      ungetc(character, input_stream);
      if (comment_depth_cnt > 0) {
        // Nested comment closed.
        comment_depth_cnt--;
        state = S_BLOCK_COMMENT;
      } else if (comment_depth_cnt == 0 && has_newline) {
        // Top-level comment closed.
        lf_inside_ml_com = true;
        indent_level = 0;

        if (bracket_depth > 0) {
          state = S_LINE_START;
        } else {
          *type = TOK_EOL;
          next_state = S_LINE_START;
          lexer_run = false;
        }
      } else {
        // Single-line block comment closed.
        state = S_START;
      }
      break;

      /**
       * FA - STRINGS
       */

    case S_STR:
      if (character == '\\') {
        state = S_ESCAPE_SEQ;
      } else if (IS_ASCII(character) && character != '"') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_STRING;
      } else if (character == '"') {
        state = S_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_START_STRING:
      if (IS_ASCII(character) && character != '"' && character != '\\') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_STRING;
      } else if (character == '\\') {
        state = S_ESCAPE_SEQ;
      } else if (character == '"') {
        *type = TOK_LIT_STRING;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_ESCAPE_SEQ:
      if (character == '\\') {
        character = '\\';
      } else if (character == '"') {
        character = '"';
      } else if (character == 'n') {
        character = '\n';
      } else if (character == 'r') {
        character = '\r';
      } else if (character == 't') {
        character = '\t';
      } else {
        return LEXICAL_ERR;
      }

      if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
        return INTERNAL_ERR;
      }
      state = S_START_STRING;
      break;

    case S_ML:
      if (character == '"') {
        state = S_START_ML;
      } else if (!(character == '"')) {
        ungetc(character, input_stream);
        *type = TOK_LIT_STRING;
        lexer_run = false;
      }
      break;

    case S_START_ML:
      if (character == EOF) {
        return LEXICAL_ERR;
      }

      if (character == '"') {
        state = S_END_ML;
      } else if (IS_ASCII(character) || character == '\n' ||
                 character == '\r') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_END_ML:
      if (character == EOF) {
        return LEXICAL_ERR;
      }

      if (character == '"') {
        state = S_END_ML_2;
      } else if (IS_ASCII(character) || character == '\n' ||
                 character == '\r') {
        // The quote wasn't stored, because end of string was suspected.
        // That is why it is added in addition to the current character.
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_END_ML_2:
      if (character == EOF) {
        return LEXICAL_ERR;
      }

      if (character == '"') {
        *type = TOK_LIT_M_STRING;
        lexer_run = false;
      } else if (IS_ASCII(character) || character == '\n' ||
                 character == '\r') {
        // The quotes were not stored, because end of string was suspected.
        // That is why they were added in addition to the current character.
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

      /**
       * FA - INTERPOLATED STRINGS
       */

    case S_S_STR:
      if (!(character == '"' || character == '\\' || character == '\n' ||
            character == EOF || character == '$')) {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_STR;
      } else if (character == '\\') {
        state = S_S_ESC_SEQ;
      } else if (character == '$') {
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        state = S_S_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_START_S_STR:
      in_interpolated_str = true;
      if (!(character == '"' || character == '\\' || character == '\n' ||
            character == EOF || character == '$')) {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_STR;
      } else if (character == '\\') {
        state = S_S_ESC_SEQ;
      } else if (character == '$') {
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        *type = TOK_LIT_INTERP_STRING_END;
        in_interpolated_str = false;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_S_ESC_SEQ:
      if (character == '\\') {
        character = '\\';
      } else if (character == '"') {
        character = '"';
      } else if (character == 'n') {
        character = '\n';
      } else if (character == 'r') {
        character = '\r';
      } else if (character == 't') {
        character = '\t';
      } else {
        return LEXICAL_ERR;
      }

      if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
        return INTERNAL_ERR;
      }
      state = S_START_S_STR;
      break;

    case S_DOLAR_STATE:
      // Two consecutive dollars '$'.
      if (character == '$') {
        if (dynstr_append_char(output_val, '$') != NO_ERR) {
          return INTERNAL_ERR;
        }

        if (interpolated_ml_str) {
          state = S_START_S_ML;
        } else {
          state = S_START_S_STR;
        }
      } else if (isalpha(character) || character == '_' || character == '{') {
        // A single '$' indicates start of an identifier or expression.
        // Return the accumulated string part and pause string parsing.
        ungetc(character, input_stream);

        if (interpolated_ml_str) {
          *type = TOK_LIT_INTERP_M_STRING_PART;
        } else {
          *type = TOK_LIT_INTERP_STRING_PART;
        }

        next_state = S_DOLAR_NEXT;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_DOLAR_NEXT:
      if (character == '{') {
        // Go to expression reading state.
        if (expresion_stack_push(&expresion_stack, interpolated_ml_str) !=
            NO_ERR) {
          return INTERNAL_ERR;
        }

        in_interpolated_str = false;
        *type = TOK_EXPRESSION_START;
        lexer_run = false;
      } else {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_READ_ID;
      }
      break;

    case S_READ_ID:
      if (isalnum(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_READ_ID;
      } else {
        ungetc(character, input_stream);
        *type = TOK_IDENTIFIER;
        lexer_run = false;
      }
      break;

    case S_S_ML:
      if (character == '"') {
        interpolated_ml_str = true;
        state = S_START_S_ML;
      } else {
        ungetc(character, input_stream);
        *type = TOK_LIT_INTERP_STRING_END;
        in_interpolated_str = false;
        lexer_run = false;
      }
      break;

    case S_START_S_ML:
      interpolated_ml_str = true;
      if (character == '$') {
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        state = S_END_S_ML;
      } else if (!(character == '"' || character == EOF || character == '$')) {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_END_S_ML:
      if (!(character == '"' || character == EOF || character == '$')) {
        // The quote wasn't stored, because end of string was suspected.
        // That is why it is added in addition to the current character.
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else if (character == '$') {
        if (dynstr_append_char(output_val, '"') != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        state = S_END_S_ML_2;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_END_S_ML_2:
      if (!(character == '"' || character == EOF || character == '$')) {
        // The quotes were not stored, because end of string was suspected.
        // That is why they were added in addition to the current character.
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else if (character == '$') {
        // The quotes were not stored, because end of string was suspected.
        // Then go to S_DOLAR_STATE.
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, '"') != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        *type = TOK_LIT_INTERP_M_STRING_END;
        in_interpolated_str = false;
        interpolated_ml_str = false;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

      /**
       * FA - INDENTATION
       */

    case S_LINE_START:
      if (character == ' ') {
        if (!comment_seen) {
          indent_level++;
        }
      } else if (character == '\n' || character == '\r') {
        indent_level = 0;
        tab_on_line = false;
        comment_seen = false;

        if (character == '\n') {
          lf_inside_ml_com = false;
        }
      } else if (character == '\t') {
        tab_on_line = true;
      } else if (character == EOF) {
        indent_level = 0;
        tab_on_line = false;
        lf_inside_ml_com = false;
        ungetc(character, input_stream);
        state = S_LINE;
      } else if (character == '/') {
        state = S_LINE_SLASH;
      } else {
        // Tabulators for indentation are forbidden.
        if (tab_on_line) {
          return LEXICAL_ERR;
        }

        // Multi line comment followed by code is forbidden.
        if (lf_inside_ml_com) {
          return LEXICAL_ERR;
        }

        ungetc(character, input_stream);
        state = S_LINE;
      }
      break;

    case S_LINE_SLASH:
      if (character == '/') {
        // It is a comment. Ignore all characters until the end of the line.
        int ignored_char;
        while ((ignored_char = getc(input_stream)) != '\n' &&
               ignored_char != EOF) {
        }

        if (ignored_char == EOF) {
          ungetc(ignored_char, input_stream);
        }

        indent_level = 0;
        tab_on_line = false;
        comment_seen = false;
        lf_inside_ml_com = false;
        state = S_LINE_START;
      } else if (character == '*') {
        // Block comment at the beginning of a line.
        bool had_newline = false;

        if (skip_block_comment(&had_newline) != NO_ERR) {
          return LEXICAL_ERR;
        }

        if (had_newline) {
          // The text after "*/" starts a new line.
          indent_level = 0;
          tab_on_line = false;
          comment_seen = false;
          lf_inside_ml_com = true;
        } else {
          comment_seen = true;
        }
        state = S_LINE_START;
      } else if (bracket_depth > 0) {
        if (tab_on_line || indent_level < bracket_start_indent + 1) {
          return LEXICAL_ERR;
        }
        ungetc(character, input_stream);
        *type = TOK_DIV;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_LINE:
      // Return the first non-whitespace character back to the stream.
      ungetc(character, input_stream);

      if (bracket_depth > 0) {
        bool is_closing = (character == ')' || character == ']');
        size_t min_indent =
            is_closing ? bracket_start_indent : bracket_start_indent + 1;

        if (character == EOF || indent_level < min_indent) {
          return LEXICAL_ERR;
        }
        state = S_START;
        continue;
      }

      // Start of a new logical line.
      bracket_start_indent = indent_level;

      // Indentation increased.
      if (indent_level > indent_stack_top(&indent_stack)) {
        if (indent_stack_push(&indent_stack, indent_level) != NO_ERR) {
          return INTERNAL_ERR;
        }
        *type = TOK_INDENT;
        next_state = S_START;
        lexer_run = false;

        // Indentation decreased.
      } else if (indent_level < indent_stack_top(&indent_stack)) {
        indent_stack_pop(&indent_stack);

        if (indent_level > indent_stack_top(&indent_stack)) {
          return LEXICAL_ERR;
        }
        *type = TOK_DEDENT;
        next_state = S_LINE;
        lexer_run = false;

        // Indentation matches the current block.
      } else if (indent_level == indent_stack_top(&indent_stack)) {
        state = S_START;
        continue;
      } else {
        return LEXICAL_ERR;
      }
      break;

    default:
      // Unknown state.
      return LEXICAL_ERR;
    }
  }

  return NO_ERR;
}

/**
 * @brief Get token with it's correct type and value.
 *
 * @param token Empty token.
 *
 * @return NO_ERR on success.
 *         INTERNAL_ERR on failure.
 */
static err_code_t lexer_scan_token(token_t *token) {
  dynstr_t output_val;
  if (dynstr_init(&output_val) != NO_ERR) {
    return INTERNAL_ERR;
  }

  token_type_t type = TOK_EMPTY;

  err_code_t err = lexer_scan_automat(&output_val, &type);

  // Decide whether the token's value (attribue) should be filled.
  if (err == NO_ERR) {
    if (has_value(type)) {
      err = token_set(token, type, output_val.data, output_val.length);
    } else {
      err = token_set(token, type, NULL, 0);
    }
  }

  if (err == NO_ERR) {
    token_prev_type = type;

    if (type == TOK_KW_FOR) {
      inside_for_loop = true;
    } else if (type == TOK_KW_DO) {
      inside_for_loop = false;
    }

    if (inside_for_loop && type == TOK_IDENTIFIER &&
        (strcmp(output_val.data, "by") == 0 ||
         strcmp(output_val.data, "to") == 0 ||
         strcmp(output_val.data, "until") == 0)) {
      token_prev_for_kw = true;
    } else {
      token_prev_for_kw = false;
    }
  }
  dynstr_free(&output_val);
  return err;
}

err_code_t lexer_get_token(token_t *token, eol_code_t eol_code) {
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  // First return tokens in the buffer, then scan new ones.
  if (token_buf_count > 0) {
    *token = token_buffer[token_buf_head];
    token_init(&token_buffer[token_buf_head]);

    token_buf_head = (token_buf_head + 1) % LEXER_BUF_SIZE;
    token_buf_count -= 1;
    return check_eol_code(&token->type, eol_code);
  }

  err_code_t err_code = lexer_scan_token(token);
  if (err_code != NO_ERR) {
    return err_code;
  }
  return check_eol_code(&token->type, eol_code);
}

err_code_t lexer_peek_token(token_t *token, eol_code_t eol_code,
                            size_t lookahead_offset) {
  if (token == NULL || lookahead_offset > LEXER_MAX_LOOKAHEAD) {
    return INTERNAL_ERR;
  }

  // Scan tokens until the seeked one is in the buffer
  while (token_buf_count <= lookahead_offset) {
    size_t index = (token_buf_head + token_buf_count) % LEXER_BUF_SIZE;

    err_code_t err = lexer_scan_token(&token_buffer[index]);

    if (err != NO_ERR) {
      token_clear(&token_buffer[index]);
      return err;
    }
    token_buf_count += 1;
  }

  const token_t *src =
      &token_buffer[(token_buf_head + lookahead_offset) % LEXER_BUF_SIZE];

  if (token_set(token, src->type, src->value, src->length) != NO_ERR) {
    return INTERNAL_ERR;
  }

  return check_eol_code(&token->type, eol_code);
}

void lexer_destroy(void) {
  for (size_t i = 0; i < token_buf_count; i++) {
    size_t index = (token_buf_head + i) % LEXER_BUF_SIZE;
    token_clear(&token_buffer[index]);
  }
  token_buf_head = 0;
  token_buf_count = 0;
  next_state = S_LINE_START;
  token_prev_type = TOK_EMPTY;
  inside_for_loop = false;
  token_prev_for_kw = false;
  indent_level = 0;
  bracket_depth = 0;
  bracket_start_indent = 0;
  in_interpolated_str = false;
  interpolated_ml_str = false;
  lf_inside_ml_com = false;
  if (indent_stack_initialized) {
    indent_stack_free(&indent_stack);
    indent_stack_initialized = false;
  }
  if (expresion_stack_initialized) {
    expresion_stack_free(&expresion_stack);
    expresion_stack_initialized = false;
  }
}

void lexer_init(FILE *stream) {
  lexer_destroy();
  if (stream == NULL) {
    input_stream = stdin;
  } else {
    input_stream = stream;
  }
}
