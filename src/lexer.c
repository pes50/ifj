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
  S_PREFIX,
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
} lexer_state_t;

static token_t token_buffer[LEXER_MAX_LOOKAHEAD + 1];
static size_t token_buf_count = 0;

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
        state = S_PREFIX;
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

    // stav comma
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

    // stav defaultni
    default:
      break;
    }
  }

  // musim prubezne nastavovat dynamicky nejaky string jakoze ten atribut tokenu
  // a tady ho priradim do tokenu

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
