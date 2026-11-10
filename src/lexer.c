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
#include "indent_stack.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LEXER_BUF_SIZE (LEXER_MAX_LOOKAHEAD + 1)
#define LEXER_AT_MAIN "main"

// stav lexeru
typedef enum {
  // stavy ze startu
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
  S_S,
  S_TOK_DOT,
  S_NUM,
  S_ZERO,
  S_STR,
  // dalsi stavy
  S_S_STR,
  S_NUM_UNDERSCORE,
  S_DOT,
  S_E,
  S_SIGNED_EXP,
  S_EXP,
  S_UNDERSCORE_EXP,
  S_DEC,
  S_DEC_UNDERSCORE,
  S_LINE_COMMENT,
  S_BLOCK_COMMENT,
  S_NESTED_BLOCK_COMMENT,
  S_BLOCK_COMMENT_END_START,
  S_BLOCK_COMMENT_END,
  S_ML,
  S_START_ML,
  S_END_ML,
  S_END_ML_2,
  S_START_STRING,
  S_ESCAPE_SEQ,
  S_S_ML,
  S_START_S_STR,
  S_S_ESC_SEQ,
  S_DOLAR_STATE,
  S_READ_ID,
  S_START_S_ML,
  S_END_S_ML,
  S_END_S_ML_2,
  S_DOLAR_STATE_ML,
  S_LINE_START,
  S_LINE,
} lexer_state_t;

// slovnik pro KWs
typedef struct {
  char *key_word;
  token_type_t type;
} dictionary_t;

static const dictionary_t keywords[] = {
    {"def", TOK_KW_DEF},         {"do", TOK_KW_DO},
    {"Double", TOK_KW_DOUBLE},   {"else", TOK_KW_ELSE},
    {"if", TOK_KW_IF},           {"import", TOK_KW_IMPORT},
    {"int", TOK_KW_INT},         {"return", TOK_KW_RETURN},
    {"String", TOK_KW_STRING},   {"then", TOK_KW_THEN},
    {"Unit", TOK_KW_UNIT},       {"val", TOK_KW_VAL},
    {"var", TOK_KW_VAR},         {"while", TOK_KW_WHILE},
    {"Boolean", TOK_KW_BOOLEAN}, {"case", TOK_KW_CASE},
    {"class", TOK_KW_CLASS},     {"extension", TOK_KW_EXTENSION},
    {"for", TOK_KW_FOR},         {"List", TOK_KW_LIST},
    {"match", TOK_KW_MATCH},     {"type", TOK_KW_TYPE},
};

static token_t token_buffer[LEXER_BUF_SIZE];
static size_t token_buf_head = 0;
static size_t token_buf_count = 0;
static token_type_t token_prev_type = TOK_EMPTY;

static size_t indent_level = 0;
static bool interpolated_ml_str = false;
static bool in_interpolated_str = false;

static indent_stack_t indent_stack;
static bool indent_stack_initialized = false;

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

static bool is_binary_operator(void) {
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

static token_type_t is_keyword(const char *word) {
  size_t num_of_kws = sizeof(keywords) / sizeof(keywords[0]);

  for (size_t index = 0; index < num_of_kws; index++) {
    if (strcmp(word, keywords[index].key_word) == 0) {
      return keywords[index].type;
    }
  }
  return TOK_IDENTIFIER;
}

static err_code_t check_eol_code(token_t *token, eol_code_t eol_code) {
  switch (eol_code) {
  case FORBIDDEN:
    if (token->type == TOK_EOL) {
      return SYNTAX_ERR;
    }
    break;

  case REQUIRED:
    if (token->type != TOK_EOL) {
      return SYNTAX_ERR;
    }
    break;

  case ALLOWED:
  default:
    break;
  }
  return NO_ERR;
}

// skenovaci fce (staci jedna pro lexer get token a peek)
static err_code_t lexer_scan_automat(dynstr_t *output_val, token_type_t *type) {
  static lexer_state_t next_start_state = S_LINE_START;
  lexer_state_t state = next_start_state;
  next_start_state = S_START;
  bool lexer_run = true;
  bool has_newline = false;
  bool tab_on_line = false;
  size_t comment_depth_cnt = 0;
  int character;

  if (!indent_stack_initialized) {
    if (indent_stack_init(&indent_stack) != NO_ERR) {
      return INTERNAL_ERR;
    }
    indent_stack_initialized = true;
  }

  while (lexer_run) {

    if (state == S_START && in_interpolated_str) {
      if (interpolated_ml_str) {
        state = S_START_S_ML;
      } else {
        state = S_START_S_STR;
      }
    }

    character = getchar();

    switch (state) {

    // stav start
    case S_START:

      switch (character) {

      // WS
      case ' ':
        state = S_LINE_START;
        break;

      // KA - state COMMA
      // ','
      case ',':
        *type = TOK_COMMA;
        lexer_run = false;
        break;

      // KA -state SEMICOLON
      // ';'
      case ';':
        *type = TOK_SEMICOLON;
        lexer_run = false;
        break;

      // KA - state EOF
      // EOF
      case EOF:
        if (indent_stack_top(&indent_stack) > 0) {
          indent_stack_pop(&indent_stack);
          *type = TOK_DEDENT;
          next_start_state = S_START;
        } else {
          *type = TOK_EOF;
        }
        lexer_run = false;
        break;
      // KA - state EOL
      // LF '\n'
      case '\n':
        *type = TOK_EOL;
        next_start_state = S_LINE_START;
        indent_level = 0;
        lexer_run = false;
        break;

      // KA - state START PARS
      // '('
      case '(':
        *type = TOK_L_PAREN;
        lexer_run = false;
        break;

      // KA - state END PARs
      // ')'
      case ')':
        *type = TOK_R_PAREN;
        lexer_run = false;
        break;

      // KA - state START_BRACKETS
      // '['
      case '[':
        *type = TOK_L_BRACKET;
        lexer_run = false;
        break;

      // KA state - END_BRACKETS
      //  ']'
      case ']':
        *type = TOK_R_BRACKET;
        lexer_run = false;
        break;

      // KA - END EXPRESION
      // '}'
      case '}':
        *type = TOK_EXPRESSION_END;
        lexer_run = false;
        break;

      // @ (@main)
      case '@':
        state = S_START_AT_MAIN;
        break;

      // 's'
      case 's':
        state = S_S_PREFIX;
        break;

      // '-'
      case '_':
        state = S_UNDERSCORE;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      // '-'
      case '-':
        state = S_MINUS;
        break;

      // '+'
      case '+':
        state = S_PLUS;
        break;

      // '/'
      case '/':
        state = S_DIV;
        break;

      // KA - state *
      // '*'
      case '*':
        *type = TOK_MUL;
        lexer_run = false;
        break;

      // '<'
      case '<':
        state = S_LOWER_THAN;
        break;

      // '>'
      case '>':
        state = S_GREATER_THAN;
        break;

      // '='
      case '=':
        state = S_EQUAL;
        break;

      // '!'
      case '!':
        state = S_NOT;
        break;

      // KA - state :
      // ':'
      case ':':
        *type = TOK_COLON;
        lexer_run = false;
        break;

      // '|'
      case '|':
        state = S_VERTICAL_BAR;
        break;

      // '&'
      case '&':
        state = S_AMPERSAND;
        break;

      // '.'
      case '.':
        state = S_TOK_DOT;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      // '0'
      case '0':
        state = S_ZERO;
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;

      // '"'
      case '"':
        state = S_STR;
        break;

      // a-z \ {s}, A-Z
      default:
        if ((character >= 'a' && character <= 'z' && character != 's') ||
            (character >= 'A' && character <= 'Z')) {
          state = S_ID;
        } else if (character >= '1' && character <= '9') {
          state = S_NUM;
        } else {
          // bude tu nejaky error asi lexemovy
          return LEXICAL_ERR;
        }

        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        break;
      }
      break;

      /**
       * KA - OPERATORS
       */

    case S_MINUS:
      ungetc(character, stdin);
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
        ungetc(character, stdin);

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
        ungetc(character, stdin);
        *type = TOK_LOWER_T;
      }
      lexer_run = false;
      break;

    case S_GREATER_THAN:
      if (character == '=') {
        *type = TOK_GREATER_EQ;
      } else {
        ungetc(character, stdin);
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
        ungetc(character, stdin);
        *type = TOK_ASSIGN;
      }
      lexer_run = false;
      break;

    case S_NOT:
      if (character == '=') {
        *type = TOK_NOT_EQ;
      } else {
        ungetc(character, stdin);
        *type = TOK_NOT;
      }
      lexer_run = false;
      break;

    case S_VERTICAL_BAR:
      if (character == '|') {
        *type = TOK_OR;
      } else {
        ungetc(character, stdin);
        *type = TOK_PIPE;
      }
      lexer_run = false;
      break;

    // POSSIBLE ERROR!
    case S_AMPERSAND:
      if (character == '&') {
        *type = TOK_AND;
      } else {
        ungetc(character, stdin);
        // lexikalni chyba
        *type = TOK_ERR;
        return LEXICAL_ERR;
      }
      lexer_run = false;
      break;

      /**
       *  KA - Identifiers
       */

    case S_START_AT_MAIN:
      for (size_t index = 0; LEXER_AT_MAIN[index] != '\0'; index++) {
        if (index > 0) {
          character = getchar();
        }
        if (character != LEXER_AT_MAIN[index]) {
          return LEXICAL_ERR;
        }
      }

      // za "@main" nesmí pokračovat identifikátor
      character = getchar();
      if (isalnum(character) || character == '_') {
        return LEXICAL_ERR;
      }
      ungetc(character, stdin);

      *type = TOK_AT_MAIN;
      lexer_run = false;
      break;

    case S_S_PREFIX:
      if (character == '"') {
        state = S_S_STR;
      } else if (isalnum(character) || character == '_') {
        if (dynstr_append_char(output_val, 's') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_ID;
      } else {
        ungetc(character, stdin);
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
        ungetc(character, stdin);
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
        ungetc(character, stdin);
        // pokud je kw tak tak kw jinak id tokentype
        *type = is_keyword(output_val->data);
        lexer_run = false;
      }
      break;

      /**
       * KA - NUMBERS, DECIMALS
       */

    case S_NUM:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        // state = S_NUM je zbytecne ale zatim nechavam pro lepsi citelnost
        state = S_NUM;
      } else if (character == '_') {
        state = S_NUM_UNDERSCORE;
      } else if (character == '.') {
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
        ungetc(character, stdin);
        *type = TOK_LIT_INT;
        lexer_run = false;
      }
      break;

    case S_ZERO:
      if (character == '.') {
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
        ungetc(character, stdin);
        *type = TOK_LIT_INT;
        lexer_run = false;
      }
      break;

    case S_TOK_DOT:
      if (character >= '0' && character <= '9') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_DEC;
      } else {
        ungetc(character, stdin);
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
        // state = S_EXP je zbytecne ale zatim nechavam pro lepsi citelnost
        state = S_EXP;
      } else if (character == '_') {
        state = S_UNDERSCORE_EXP;
      } else if (character == 'D' || character == 'd') {
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      } else {
        ungetc(character, stdin);
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
        *type = TOK_LIT_DOUBLE;
        lexer_run = false;
      } else if (character == 'e' || character == 'E') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_E;
      } else {
        ungetc(character, stdin);
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
       * KA - COMMENTS
       */

    case S_DIV:
      if (character == '/') {
        state = S_LINE_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT;
      } else {
        ungetc(character, stdin);
        *type = TOK_DIV;
        lexer_run = false;
      }
      break;

    case S_LINE_COMMENT:
      if (character == '\n' || character == EOF) {
        *type = TOK_EOL;
        lexer_run = false;
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
        ungetc(character, stdin);
        state = S_BLOCK_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT_END_START:
      if (character == '*') {
        state = S_BLOCK_COMMENT_END_START;
      } else if (character == '/') {
        state = S_BLOCK_COMMENT_END;
      } else {
        ungetc(character, stdin);
        state = S_BLOCK_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT_END:
      ungetc(character, stdin);
      if (comment_depth_cnt > 0) {
        comment_depth_cnt--;
        state = S_BLOCK_COMMENT;
      } else if (comment_depth_cnt == 0 && has_newline) {
        *type = TOK_EOL;
        indent_level = 0;
        next_start_state = S_LINE_START;
        lexer_run = false;
      } else {
        state = S_START;
      }
      break;

      /**
       * KA - STRING
       */

    case S_STR:
      if (!(character == '\n' || character == EOF || character == '"')) {
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

      // OD TUDMA KONTROLA STAVU + NEZAPOMENOUT NA INDENT

    case S_START_STRING:
      if (!(character == '\n' || character == EOF || character == '"' ||
            character == '\\')) {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_STRING;
      } else if (character == '\\') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
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
        ungetc(character, stdin);
        *type = TOK_LIT_STRING;
        lexer_run = false;
      }
      break;

    case S_START_ML:
      if (character == EOF) {
        return LEXICAL_ERR;
      } else if (character == '"') {
        state = S_END_ML;
      } else {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
      }
      break;

    case S_END_ML:
      if (character == '"') {
        state = S_END_ML_2;
      } else if (character == EOF) {
        return LEXICAL_ERR;
      } else {
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_ML;
      }
      break;

    case S_END_ML_2:
      if (character == '"') {
        *type = TOK_LIT_M_STRING;
        lexer_run = false;
      } else if (character == EOF) {
        return LEXICAL_ERR;
      } else {
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_ML;
      }
      break;

      /**
       * KA - INTERPOLATED STRING
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
        *type = TOK_LIT_INTERP_M_STRING_END;
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
      if (character == '$') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_STR;
      } else if (token_prev_type != TOK_LIT_INTERP_STRING_PART) {
        *type = TOK_LIT_INTERP_STRING_PART;
        lexer_run = false;
      } else if (isalpha(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_READ_ID;
      } else if (character == '{') {
        *type = TOK_EXPRESSION_START;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_READ_ID:
      if (isalnum(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_READ_ID;
      } else if (character == '}') {
        *type = TOK_EXPRESSION_END;
        if (interpolated_ml_str) {
          state = S_START_S_ML;
        } else {
          state = S_START_S_STR;
        }
        lexer_run = false;
      } else {
        ungetc(character, stdin);
        *type = TOK_IDENTIFIER;
        lexer_run = false;
      }
      break;

    case S_S_ML:
      if (!(character == '"')) {
        ungetc(character, stdin);
        *type = TOK_LIT_INTERP_M_STRING_END;
        lexer_run = false;
      } else if (character == '"') {
        state = S_START_S_ML;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_START_S_ML:
      interpolated_ml_str = true;
      if (character == '$') {
        state = S_DOLAR_STATE_ML;
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
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else if (character == '$') {
        state = S_DOLAR_STATE_ML;
      } else if (character == '"') {
        state = S_END_S_ML_2;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_END_S_ML_2:
      if (!(character == '"' || character == EOF || character == '$')) {
        if (dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, '"') != NO_ERR ||
            dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else if (character == '$') {
        state = S_DOLAR_STATE_ML;
      } else if (character == '"') {
        *type = TOK_LIT_INTERP_M_STRING_END;
        interpolated_ml_str = false;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

    case S_DOLAR_STATE_ML:
      if (character == '$') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_START_S_ML;
      } else if (isalpha(character) || character == '_') {
        if (dynstr_append_char(output_val, (char)character) != NO_ERR) {
          return INTERNAL_ERR;
        }
        state = S_READ_ID;
      } else if (character == '{') {
        *type = TOK_EXPRESSION_START;
        lexer_run = false;
      } else {
        return LEXICAL_ERR;
      }
      break;

      /**
       * INDENTACE
       */

    case S_LINE_START:
      if (character == ' ') {
        indent_level++;
      } else if (character == '\n' || character == '\r') {
        indent_level = 0;
        tab_on_line = false;
      } else if (character == '\t') {
        tab_on_line = true;
      } else if (character == EOF) {
        tab_on_line = false;
        ungetc(character, stdin);
        state = S_LINE;
      } else {
        if (tab_on_line) {
          return LEXICAL_ERR;
        }
        ungetc(character, stdin);
        state = S_LINE;
      }
      break;

    case S_LINE:
      ungetc(character, stdin);
      if (indent_level > indent_stack_top(&indent_stack)) {
        if (indent_stack_push(&indent_stack, indent_level) != NO_ERR) {
          return INTERNAL_ERR;
        }
        *type = TOK_INDENT;
        next_start_state = S_START;
        lexer_run = false;
      } else if (indent_level < indent_stack_top(&indent_stack)) {
        indent_stack_pop(&indent_stack);

        if (indent_level > indent_stack_top(&indent_stack)) {
          return LEXICAL_ERR;
        }
        *type = TOK_DEDENT;
        next_start_state = S_LINE;
        lexer_run = false;
      } else if (indent_level == indent_stack_top(&indent_stack)) {
        state = S_START;
        continue;
      } else {
        return LEXICAL_ERR;
      }

    default:
      break;
    }
  }

  return NO_ERR;
}

static err_code_t lexer_scan_token(token_t *token) {
  dynstr_t output_val;
  if (dynstr_init(&output_val) != NO_ERR) {
    return INTERNAL_ERR;
  }

  token_type_t type = TOK_EMPTY;

  err_code_t err = lexer_scan_automat(&output_val, &type);

  if (err == NO_ERR) {
    if (has_value(type)) {
      err = token_set(token, type, output_val.data, output_val.length);
    } else {
      err = token_set(token, type, NULL, 0);
    }
  }

  if (err == NO_ERR) {
    token_prev_type = type;
  }
  dynstr_free(&output_val);
  return err;
}

err_code_t lexer_get_token(token_t *token, eol_code_t eol_code) {
  if (token == NULL) {
    return INTERNAL_ERR;
  }

  if (token_buf_count > 0) {
    *token = token_buffer[token_buf_head];
    token_init(&token_buffer[token_buf_head]);

    token_buf_head = (token_buf_head + 1) % LEXER_BUF_SIZE;
    token_buf_count -= 1;
    return check_eol_code(token, eol_code);
  }

  err_code_t err_code = lexer_scan_token(token);
  if (err_code != NO_ERR) {
    return err_code;
  }
  return check_eol_code(token, eol_code);
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

  return check_eol_code(token, eol_code);
}

void lexer_destroy(void) {
  for (size_t i = 0; i < token_buf_count; i++) {
    size_t index = (token_buf_head + i) % LEXER_BUF_SIZE;
    token_clear(&token_buffer[index]);
  }
  token_buf_head = 0;
  token_buf_count = 0;
  if (indent_stack_initialized) {
    indent_stack_free(&indent_stack);
    indent_stack_initialized = false;
  }
}
