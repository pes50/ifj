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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LEXER_BUF_SIZE (LEXER_MAX_LOOKAHEAD + 1)

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
static token_type_t prev_token_type = TOK_EMPTY;

static size_t comment_cnt = 0;

static bool is_binary_operator(void) {
  switch (prev_token_type) {
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

// mozna nebude potreba
static bool has_new_line() { return true; }

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
static err_code_t lexer_scan_token(token_t *token) {
  lexer_state_t state = S_START;
  bool lexer_run = true;
  int character;

  dynstr_t output_val;
  dynstr_init(&output_val);

  while (lexer_run) {

    character = getchar();

    switch (state) {

    // stav start
    case S_START:

      switch (character) {

      // WS
      case ' ':
        state = S_START;
        break;

      // KA - state COMMA
      // ','
      case ',':
        token->type = TOK_COMMA;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA -state SEMICOLON
      // ';'
      case ';':
        token->type = TOK_SEMICOLON;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - state EOF
      // EOF
      case EOF:
        token->type = TOK_EOF;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - state EOL
      // LF '\n'
      case '\n':
        token->type = TOK_EOL;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - state START PARS
      // '('
      case '(':
        token->type = TOK_L_PAREN;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - state END PARs
      // ')'
      case ')':
        token->type = TOK_R_PAREN;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - state START_BRACKETS
      // '['
      case '[':
        token->type = TOK_L_BRACKET;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA state - END_BRACKETS
      //  ']'
      case ']':
        token->type = TOK_R_BRACKET;
        dynstr_append_char(&output_val, character);
        lexer_run = false;
        break;

      // KA - END EXPRESION
      // '}'
      case '}':
        token->type = TOK_EXPRESSION_END;
        dynstr_append_char(&output_val, character);
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
        token->type = TOK_MUL;
        dynstr_append_char(&output_val, character);
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
        token->type = TOK_COLON;
        dynstr_append_char(&output_val, character);
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
        break;

      // '0'
      case '0':
        state = S_ZERO;
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
        } else if (character >= 1 && character <= 9) {
          state = S_NUM;
        } else {
          // bude tu nejaky error asi lexemovy
          continue;
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
        token->type = TOK_BINARY_MINUS;
      } else {
        token->type = TOK_UNARY_MINUS;
      }
      lexer_run = false;
      break;

    case S_PLUS:
      if (character == '+') {
        token->type = TOK_CONCAT_LIST;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);

        if (is_binary_operator()) {
          token->type = TOK_BINARY_PLUS;
        } else {
          token->type = TOK_UNARY_PLUS;
        }
      }
      lexer_run = false;
      break;

    case S_LOWER_THAN:
      if (character == '=') {
        token->type = TOK_LOWER_EQ;
        dynstr_append_char(&output_val, character);
      } else if (character == '-') {
        token->type = TOK_ARROW_LEFT;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        token->type = TOK_LOWER_T;
      }
      lexer_run = false;
      break;

    case S_GREATER_THAN:
      if (character == '=') {
        token->type = TOK_GREATER_EQ;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        token->type = TOK_GREATER_T;
      }
      lexer_run = false;
      break;

    case S_EQUAL:
      if (character == '=') {
        token->type = TOK_EQUAL;
        dynstr_append_char(&output_val, character);
      } else if (character == '>') {
        token->type = TOK_ARROW_RIGHT;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        token->type = TOK_ASSIGN;
      }
      lexer_run = false;
      break;

    case S_NOT:
      if (character == '=') {
        token->type = TOK_NOT_EQ;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        token->type = TOK_NOT;
      }
      lexer_run = false;
      break;

    case S_VERTICAL_BAR:
      if (character == '|') {
        token->type = TOK_OR;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        token->type = TOK_PIPE;
      }
      lexer_run = false;
      break;

    // POSIBLE ERROR!
    case S_AMPERSAND:
      if (character == '&') {
        token->type = TOK_AND;
        dynstr_append_char(&output_val, character);
      } else {
        ungetc(character, stdin);
        // lexikalni chyba
        token->type = TOK_ERR;
        dynstr_free(&output_val);
        return LEXICAL_ERR;
      }
      lexer_run = false;
      break;

      /**
       *  KA - Identifiers
       */

    case S_START_AT_MAIN: {
      const char *id_main = "main";
      size_t i = 0;

      while (character != '\0') {
        if (character != id_main[i]) {
          return LEXICAL_ERR;
        }
        i++;
        if (id_main[i] != '\0') {
          character = getchar();
        }
      }
    }

      character = getchar();
      if (isalnum(character) || character == '_') {
        return LEXICAL_ERR;
      }
      ungetc(character, stdin);

      token->type = TOK_AT_MAIN;
      lexer_run = false;
      break;

    case S_S_PREFIX:
      character = getchar();
      if (character == '"') {
        state = S_S_STR;
      } else if (isalnum(character) || character == '_') {
        state = S_ID;
      } else {
        ungetc(character, stdin);
        token->type = TOK_IDENTIFIER;
        lexer_run = false;
      }
      break;

    case S_UNDERSCORE:
      character = getchar();
      if (isalnum(character) || character == '_') {
        state = S_ID;
      } else {
        ungetc(character, stdin);
        token->type = TOK_UNDERSCORE;
        lexer_run = false;
      }
      break;

    case S_ID:
      character = getchar();
      if (isalnum(character) || character == '_') {
        state = S_ID;
      } else {
        // pokud je kw tak tak kw jinak id tokentype
        lexer_run = false;
      }
      break;

    case S_NUM:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_NUM;
      } else if (character == '_') {
        state = S_NUM_UNDERSCORE;
      } else if (character == '.') {
        state = S_DOT;
      } else if (character == 'e' || character == 'E') {
        state = S_E;
      } else {
        lexer_run = false;
      }
      break;

    case S_ZERO:
      character = getchar();
      if (character == '.') {
        state = S_DOT;
      } else if (character == 'e' || character == 'E') {
        state = S_E;
      } else {
        lexer_run = false;
      }
      break;

    case S_TOK_DOT:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_DEC;
      } else {
        // bud lexikalni chyba nebo je tecka nejaky koncovy stav ale nevim.
        lexer_run = false;
      }
      break;

    case S_NUM_UNDERSCORE:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_NUM;
      } else {
        // bud lexikalni chyba nebo je tecka nejaky koncovy stav ale nevim.
        lexer_run = false;
      }
      break;

    case S_DOT:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_DEC;
      } else {
        // bud lexikalni chyba nebo je tecka nejaky koncovy stav ale nevim.
        lexer_run = false;
      }
      break;

    case S_E:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_EXP;
      } else if (character == '+' || character == '-') {
        state = S_SIGNED_EXP;
      } else {
        lexer_run = false;
      }
      break;

    case S_SIGNED_EXP:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_EXP;
      } else {
        lexer_run = false;
      }
      break;

    case S_EXP:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_EXP;
      } else if (character == '_') {
        state = S_UNDERSCORE_EXP;
      } else if (character == 'D' || character == 'd') {
        token->type = TOK_LIT_DOUBLE;
      } else {
        token->type = TOK_LIT_INT;
        lexer_run = false;
      }
      break;

    case S_UNDERSCORE_EXP:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_EXP;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_DEC:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_DEC;
      } else if (character == '_') {
        state = S_DEC_UNDERSCORE;
      } else if (character == 'D' || character == 'd') {
        token->type = TOK_LIT_DOUBLE;
      } else if (character == 'e' || character == 'E') {
        state = S_E;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_DEC_UNDERSCORE:
      character = getchar();
      if (character >= 0 && character <= 9) {
        state = S_DEC;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    // komenty
    case S_DIV:
      character = getchar();
      if (character == '/') {
        state = S_LINE_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT;
      } else {
        token->type = TOK_DIV;
        lexer_run = false;
      }
      break;

    case S_LINE_COMMENT:
      character = getchar();
      if (character == '\n' || character == EOF) {
        token->type = TOK_EOL;
        lexer_run = false;
      } else {
        state = S_LINE_COMMENT;
      }
      break;

    case S_BLOCK_COMMENT:
      character = getchar();
      if (!(character == '\n' || character == EOF)) {
        state = S_BLOCK_COMMENT;
      } else if (character == '/') {
        state = S_NESTED_BLOCK_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT_END_START;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_NESTED_BLOCK_COMMENT:
      character = getchar();
      if (!(character == '*')) {
        state = S_BLOCK_COMMENT;
      } else if (character == '*') {
        state = S_BLOCK_COMMENT;
        comment_cnt++;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_BLOCK_COMMENT_END_START:
      character = getchar();
      if (character == '*') {
        state = S_BLOCK_COMMENT_END_START;
      } else if (character == '/') {
        state = S_BLOCK_COMMENT_END;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_BLOCK_COMMENT_END:
      character = getchar();
      if (comment_cnt > 0) {
        state = S_BLOCK_COMMENT;
      } else if (comment_cnt == 0 && has_new_line()) {
        token->type = TOK_EOL;
        lexer_run = false;
      } else if (comment_cnt == 0 && !has_new_line()) {
        state = S_START;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_STR:
      character = getchar();
      if (!(character == '\n' || character == EOF || character == '"')) {
        state = S_START_STRING;
      } else if (character == '"') {
        state = S_ML;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_START_STRING:
      character = getchar();
      if (!(character == '\n' || character == EOF || character == '"' ||
            character == '\\')) {
        state = S_START_STRING;
      } else if (character == '\\') {
        state = S_ESCAPE_SEQ;
      } else if (character == '"') {
        token->type = TOK_LIT_STRING;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_ESCAPE_SEQ:
      character = getchar();
      if (character == '\\' || character == '"' || character == 'n' ||
          character == '\r' || character == 't') {
        state = S_START_STRING;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_ML:
      character = getchar();
      if (character == '"') {
        state = S_START_ML;
      } else if (!(character == '"')) {
        token->type = TOK_LIT_STRING;
        lexer_run = false;
      }
      break;

    case S_START_ML:
      character = getchar();
      if (!(character == '"' || character == EOF)) {
        state = S_START_ML;
      } else if (character == '"') {
        state = S_END_ML;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_END_ML:
      character = getchar();
      if (!(character == '"' || character == EOF)) {
        state = S_START_ML;
      } else if (character == '"') {
        state = S_END_ML_2;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_END_ML_2:
      character = getchar();
      if (!(character == '"' || character == EOF)) {
        state = S_START_ML;
      } else if (character == '"') {
        token->type = TOK_LIT_M_STRING;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

      // interpolovany

    case S_S_STR:
      character = getchar();
      if (!(character == '"' || character == '\\' || character == '\n' ||
            character == EOF || character == '$')) {
        state = S_START_ML;
      } else if (character == '\\') {
        state = S_S_ESC_SEQ;
      } else if (character == '$') {
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        state = S_S_ML;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_START_S_STR:
      character = getchar();
      if (!(character == '"' || character == '\\' || character == '\n' ||
            character == EOF || character == '$')) {
        state = S_START_S_STR;
      } else if (character == '\\') {
        state = S_S_ESC_SEQ;
      } else if (character == '$') {
        state = S_DOLAR_STATE;
      } else if (character == '"') {
        token->type = TOK_LIT_INTERP_M_STRING_END;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_S_ESC_SEQ:
      character = getchar();
      if (character == '\\' || character == '"' || character == 'n' ||
          character == '\r' || character == 't') {
        state = S_START_S_STR;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_DOLAR_STATE:
      character = getchar();
      if (character == '$') {
        state = S_START_S_STR;
      } else if (isalpha(character) || character == '_') {
        state = S_READ_ID;
      } else if (character == '{') {
        token->type = TOK_EXPRESSION_START;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_READ_ID:
      character = getchar();
      if (isalnum(character) || character == '_') {
        state = S_READ_ID;
      } else {
        token->type = TOK_IDENTIFIER;
        lexer_run = false;
      }
      break;

    case S_S_ML:
      character = getchar();
      if (!(character == '"')) {
        token->type = TOK_LIT_INTERP_M_STRING_END;
        lexer_run = false;
      } else if (character == '"') {
        state = S_START_S_ML;
      } else {
        // chyba;
        lexer_run = false;
      }
      break;

    case S_START_S_ML:
      character = getchar();
      if (!(character == '"' || character == EOF || character == '$')) {
        state = S_START_S_ML;
      } else if (character == '$') {
        state = S_DOLAR_STATE_ML;
      } else if (character == '"') {
        state = S_END_S_ML;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_END_S_ML:
      character = getchar();
      if (!(character == '"' || character == EOF || character == '$')) {
        state = S_START_S_ML;
      } else if (character == '$') {
        state = S_DOLAR_STATE_ML;
      } else if (character == '"') {
        state = S_END_S_ML_2;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_END_S_ML_2:
      character = getchar();
      if (!(character == '"' || character == EOF || character == '$')) {
        state = S_START_S_ML;
      } else if (character == '$') {
        state = S_DOLAR_STATE_ML;
      } else if (character == '"') {
        token->type = TOK_LIT_INTERP_M_STRING_END;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    case S_DOLAR_STATE_ML:
      character = getchar();
      if (character == '$') {
        state = S_START_S_ML;
      } else if (isalpha(character) || character == '_') {
        state = S_READ_ID;
      } else if (character == '{') {
        token->type = TOK_EXPRESSION_START;
        lexer_run = false;
      } else {
        // chyba
        lexer_run = false;
      }
      break;

    default:
      is_keyword("HELLO");
      break;
    }
  }

  prev_token_type = token->type;

  // musim prubezne nastavovat dynamicky nejaky string jakoze ten atribut
  // tokenu a tady ho priradim do tokenu

  return NO_ERR;
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
}
