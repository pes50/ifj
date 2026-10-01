/**************************** IFJ/IAL-Compiler-2026 ***************************\
 *
 * @file parser.h
 *
 * @author Matous Jahoda (xjahodm00)
 *
 * @brief Specification of parser interface
 *
 \*****************************************************************************/

#include "err_codes.h"

/**
 * @brief Does syntax analysis on input (stdin) code
 *
 * @return NO_ERR on succes or corresponding error code on failure
 */
err_code_t parse();