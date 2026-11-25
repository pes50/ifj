/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file string_duplicate.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of string_duplicate function
 *
\*****************************************************************************/

#pragma once

/**
 * @brief Duplicates given string
 *
 * @param src A pointer to the string to duplicate
 * @return A pointer to duplicated string or NULL if failed
 */
char *string_duplicate(const char *src);