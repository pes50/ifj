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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

// stav lexeru
typedef enum {
  // stavy ze startu
  S_START,
  S_COMMA,
  S_SEMICOLON,
  S_EOF,
  S_EOL,
  S_START_PARS,
  S_END_PARS,
  S_START_BRACKETS,
  S_END_BRACKETS,
  S_START_BRACES,
  S_END_BRACES,
  S_START_AT_MAIN,
  S_S_PREFIX,
  S_ID,
  S_UNDERSCORE,
  S_MINUS,
  S_PLUS,
  S_DIV,
  S_ASTERISK,
  S_LOWER_THAN,
  S_GREATER_THAN,
  S_EQUAL,
  S_NOT,
  S_COLON,
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

static token_t token_buffer[LEXER_MAX_LOOKAHEAD + 1];
static size_t token_buf_count = 0;
static token_type_t prev_token_type = TOK_EMPTY;

static size_t comment_cnt = 0;

static bool prev_ends_operand(void) {
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

static bool is_keyword(const char *word) {
  (void)word;
  return true;
}
static bool has_new_line() { return true; }

// skenovaci fce (staci jedna pro lexer get token a peek)
static err_code_t lexer_scan_token(token_t *token) {
  lexer_state_t state = S_START;
  bool lexer_run = true;
  int character;

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

      // ','
      case ',':
        state = S_COMMA;
        break;

      // ';'
      case ';':
        state = S_SEMICOLON;
        break;

      // EOF
      case EOF:
        state = S_EOF;
        break;

      // LF '\n'
      case '\n':
        state = S_EOL;
        break;

      // '('
      case '(':
        state = S_START_PARS;
        break;

      // ')'
      case ')':
        state = S_END_PARS;
        break;

      // '['
      case '[':
        state = S_START_BRACKETS;
        break;

      // ']'
      case ']':
        state = S_END_BRACKETS;
        break;

      // '{'
      case '{':
        state = S_START_BRACES;
        break;

      // '}'
      case '}':
        state = S_END_BRACES;
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

      // '*'
      case '*':
        state = S_ASTERISK;
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

      // ':'
      case ':':
        state = S_COLON;
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

      // stavy

    case S_COMMA:
      token->type = TOK_COMMA;
      lexer_run = false;
      break;

    case S_SEMICOLON:
      token->type = TOK_SEMICOLON;
      lexer_run = false;
      break;

    case S_EOF:
      token->type = TOK_EOF;
      lexer_run = false;
      break;

    case S_EOL:
      token->type = TOK_EOL;
      lexer_run = false;
      break;

    case S_START_PARS:
      token->type = TOK_L_PAREN;
      lexer_run = false;
      break;

    case S_END_PARS:
      token->type = TOK_R_PAREN;
      lexer_run = false;
      break;

    case S_START_BRACKETS:
      token->type = TOK_L_BRACKET;
      lexer_run = false;
      break;

    case S_END_BRACKETS:
      token->type = TOK_R_BRACKET;
      lexer_run = false;
      break;

    case S_START_BRACES:
      // neco musim vymyslet
      // token->type = TOK_L_PAREN;
      lexer_run = false;
      break;

    case S_END_BRACES:
      // neco musim vymyslet
      // token->type = TOK_L_PAREN;
      lexer_run = false;
      break;

    case S_MINUS:
      if (prev_ends_operand()) {
        token->type = TOK_BINARY_MINUS;
      } else {
        token->type = TOK_UNARY_MINUS;
      }
      lexer_run = false;
      break;

    case S_PLUS:
      character = getchar();
      if (character == '+') {
        token->type = TOK_CONCAT_LIST;
      } else {
        ungetc(character, stdin);

        if (prev_ends_operand()) {
          token->type = TOK_BINARY_PLUS;
        } else {
          token->type = TOK_UNARY_PLUS;
        }
      }
      lexer_run = false;
      break;

    case S_ASTERISK:
      token->type = TOK_MUL;
      lexer_run = false;
      break;

    case S_LOWER_THAN:
      character = getchar();
      if (character == '=') {
        token->type = TOK_LOWER_EQ;
      } else if (character == '-') {
        token->type = TOK_ARROW_LEFT;
      } else {
        ungetc(character, stdin);
        token->type = TOK_LOWER_T;
      }
      lexer_run = false;
      break;

    case S_GREATER_THAN:
      character = getchar();
      if (character == '=') {
        token->type = TOK_GREATER_EQ;
      } else {
        ungetc(character, stdin);
        token->type = TOK_GREATER_T;
      }
      lexer_run = false;
      break;

    case S_EQUAL:
      character = getchar();
      if (character == '=') {
        token->type = TOK_EQUAL;
      } else if (character == '>') {
        token->type = TOK_ARROW_RIGHT;
      } else {
        ungetc(character, stdin);
        token->type = TOK_ASSIGN;
      }
      lexer_run = false;
      break;

    case S_NOT:
      character = getchar();
      if (character == '=') {
        token->type = TOK_NOT_EQ;
      } else {
        ungetc(character, stdin);
        token->type = TOK_NOT;
      }
      lexer_run = false;
      break;

    case S_COLON:
      token->type = TOK_COLON;
      lexer_run = false;
      break;

    case S_VERTICAL_BAR:
      character = getchar();
      if (character == '|') {
        token->type = TOK_OR;
      } else {
        ungetc(character, stdin);
        token->type = TOK_PIPE;
      }
      lexer_run = false;
      break;

    // MUZE TU NASTAT ERROR POKUD BUDE JENOM JEDEN AMPERSAND!!!
    case S_AMPERSAND:
      character = getchar();
      if (character == '&') {
        token->type = TOK_AND;
      } else {
        ungetc(character, stdin);
        // lexikalni chyba
        token->type = TOK_ERR;
      }
      lexer_run = false;
      break;

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
  (void)token;
  (void)eol_code;
  return lexer_scan_token(token);
}

err_code_t lexer_peek_token(token_t *token, eol_code_t eol_code,
                            size_t lookahead_offset) {
  (void)token;
  (void)eol_code;
  (void)lookahead_offset;
  (void)token_buf_count;
  (void)token_buffer;
  return 0;
}

err_code_t lexer_destroy(void) { return 0; }
