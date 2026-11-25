/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file ast_printer.hpp
 *
\*****************************************************************************/

#pragma once

extern "C" {
#include "ast_node.h"
}

void dump_ast_to_file(const char *filepath, ast_node_t *root);