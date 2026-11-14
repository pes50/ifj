#pragma once

#include "ast_node.h"
#include "inner_code.h"

/**
 * @brief Converts analysed AST to 3AC
 *
 * @param main_node A pointer to an AST node representing the entrypoint of the
 * program
 * @param code [out] A pointer to store the generated code
 *
 * @return NO_ERR on success
 */
err_code_t ast_convert(ast_node_t *main_node, inner_statement_t **code);