/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file builtin.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of create_builtin_symtable function
 *
\*****************************************************************************/

#pragma once

#include "symtable.h"

/**
 * @brief Creates a symtable populated with built-in functions of IFJ26
 *
 * @return A pointer to the created table or NULL if failed
 */
hashtable_t *create_builtin_symtable();