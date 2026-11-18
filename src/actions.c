/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file actions.c
 *
 * @author Matous Jahoda (xjahodm00)
 *
 * @brief Implementaion of action determining for expresion parsing
 *
\*****************************************************************************/

#include "actions.h"

// all the different priorities
typedef enum {
  NOT_VALID = -1,
  ID_OR_CONST = 0,
  UN_OP = 1,
  MUL_DIV = 2,
  PLUS_MIN = 3,
  GT_LW = 4,
  EQ_NEQ = 5,
  AND = 6,
  OR = 7,
  R_PAREN = 8,
} priority_t;

typedef enum {
  R_ASSOC = SHIFT,
  L_ASSOC = REDUCE,
  NO_ASSOC = ERROR,
} associativity_t;

// returns priority based on token type
priority_t get_priority(token_type_t token_type) {
  switch (token_type) {
  case TOK_IDENTIFIER:
  case TOK_LIT_INT:
  case TOK_LIT_DOUBLE:
  case TOK_LIT_STRING:
  case TOK_LIT_M_STRING:
  case TOK_LIT_TRUE:
  case TOK_LIT_FALSE:
  case TOK_L_PAREN:
    return ID_OR_CONST;

  case TOK_UNARY_MINUS:
  case TOK_UNARY_PLUS:
  case TOK_NOT:
    return UN_OP;

  case TOK_MUL:
  case TOK_DIV:
    return MUL_DIV;

  case TOK_BINARY_PLUS:
  case TOK_BINARY_MINUS:
  case TOK_CONCAT_LIST:
    return PLUS_MIN;

  case TOK_LOWER_T:
  case TOK_LOWER_EQ:
  case TOK_GREATER_T:
  case TOK_GREATER_EQ:
    return GT_LW;

  case TOK_EQUAL:
  case TOK_NOT_EQ:
    return EQ_NEQ;

  case TOK_AND:
    return AND;

  case TOK_OR:
    return OR;

  case TOK_R_PAREN:
    return R_PAREN;

  default:
    return NOT_VALID;
  }
}

associativity_t get_assoc(priority_t priority) {
  switch (priority) {
  case UN_OP:
    return R_ASSOC;

  case MUL_DIV:
  case PLUS_MIN:
  case AND:
  case OR:
    return L_ASSOC;

  default:
    return NO_ASSOC;
  }
}

actions_t get_action(token_type_t input, token_type_t stack) {
  // deal with the outlier first
  if (stack == TOK_L_PAREN) {
    if (input == TOK_R_PAREN) {
      return CONSUME_PAREN;
    }
    return SHIFT;
  }

  // if there is extra closing paren, end the loop
  if (input == TOK_R_PAREN && stack == TOK_ERR) {
    return END;
  }

  // if token stack has no tokens
  if (stack == TOK_ERR) {
    return SHIFT;
  }

  priority_t input_p = get_priority(input);
  priority_t stack_p = get_priority(stack);

  if (input_p == NOT_VALID || stack_p == NOT_VALID) {
    return ERROR;
  }
  // stack is more important
  if (input_p > stack_p) {
    return REDUCE;
  }
  if (input_p < stack_p) {
    return SHIFT;
  }
  return (actions_t)get_assoc(stack_p);
}