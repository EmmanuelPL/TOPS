
#include <stdio.h>
#include <stdlib.h>

#define INITALLOC  16  /* #chars initally alloced */
#define STEP        8  /* #chars to realloc by */

/**
 *  From
 *  https://stackoverflow.com/questions/34695410/is-there-a-good-alternative-to-fgets 
 */
size_t fetchline(char **dynline);

/**
 * Stores the trimmed input string into the given output buffer, which must be
 * large enough to store the result.  If it is too small, the output is
 * truncated.
 * From https://stackoverflow.com/a/122721
 */
size_t trimwhitespace(char *out, size_t len, const char *str);

/**
 * @brief Genera tokens a partir de un string
 * @details Genera un array de string con los tokens de un string.
 * @author Emmanuel Posadas L. <emmanuel.posadasl@gmail.com>
 * 
 * @param out Array con los tokens
 * @param str El string donde buscar los tokens
 * 
 * @return cantidad de elementos del array
 */
size_t tokenize(char * out[], const char* str);