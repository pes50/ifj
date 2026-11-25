/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file parser.h
 *
 * @author Matouš Jahoda (xjahodm00)
 *
 * @brief Specification of parser interface
 *
\*****************************************************************************/

#include "ast_node.h"
#include "err_codes.h"

#include <stdio.h>

/**
 * @brief Does syntax analysis on input (stdin) code
 *
 * @return NO_ERR on succes or corresponding error code on failure
 */
err_code_t parse(ast_node_t **node, FILE *stream);