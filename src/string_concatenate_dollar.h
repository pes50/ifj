/************* Implementace překladače imperativního jazyka IFJ26 ************\
 *
 * @file string_concatenate_dollar.h
 *
 * @author Roman Skoupý      (xskoupr00)
 *
 * @brief Specification of string_concatenate_dollar function
 *
\*****************************************************************************/

#pragma once

/**
 * @brief Concatenate two strings and put $ between them
 *
 * @param first A pointer to the first string
 * @param second A pointer to the second string
 *
 * @return A pointer to concatenated string or NULL if failed
 */
char *string_concatenate_dollar(const char *first, const char *second);