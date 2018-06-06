#include "Tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG 0

/**
 *  From
 *  https://stackoverflow.com/questions/34695410/is-there-a-good-alternative-to-fgets 
 */
size_t fetchline(char **dynline)
{
    int i, c;
    size_t nalloced;  /* #chars currently alloced */

    if ((*dynline = malloc(INITALLOC)) == NULL)
        return 0;

    nalloced = INITALLOC;

    for (i = 0; (c = getchar()) != EOF && c != '\n'; ++i) {
        /* buffer is full, request more mem */
        if (i == nalloced)
            if ((*dynline = realloc(*dynline, nalloced += STEP)) == NULL)
                return 0;

        /* store the newly read character */
        (*dynline)[i] = c;
    }
    /* zero terminate the string */
    (*dynline)[i] = '\0';

    if (c == EOF)
        return 0;  /* return 0 on EOF */

    return nalloced;
}

/**
 * Stores the trimmed input string into the given output buffer, which must be
 * large enough to store the result.  If it is too small, the output is
 * truncated.
 * From https://stackoverflow.com/a/122721
 */
size_t trimwhitespace(char *out, size_t len, const char *str)
{
    if(len == 0)
        return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len-1 ? (end - str) : len-1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}


/**
 * @brief Genera tokens a partir de un string
 * @details Genera un array de string con los tokens de un string.
 * 
 * @author Emmanuel Posadas L. <emmanuel.posadasl@gmail.com>
 * 
 * @param out Array con los tokens
 * @param str El string donde buscar los tokens
 * 
 * @return cantidad de elementos del array
 */
size_t tokenize(char * out[], const char* strI) 
{
    if (DEBUG)
        printf("Init\n");

    size_t len_size =  sizeof(char) * strlen(strI);
    size_t c  = 0;

    char* str;

    // Creo una copia del strI para que el original No se modifique
    if ((str = malloc(len_size)) == NULL)
        return 0;

    strcpy(str, strI);

    // busco los tokens
    if (DEBUG)
        printf("Search for tokens in '%s'...\n", str);

    char * pch;
    pch = strtok(str," ");

    while (pch != NULL)
    {
        if (DEBUG)
            printf("FOUND! ");
        
        if ((out[c] = malloc(len_size)) == NULL)
            return c;

        // Copio el token al array
        strcpy(out[c], pch);

        if (DEBUG)
            printf("fond a token with %d of length\n", strlen(out[c]));

        // apunto al proximo token
        pch = strtok(NULL, " ");
        ++c;
    }

    if (DEBUG)
        printf("Free memory\n");


    // libero la memoria de la copia temporal
    free(str);

    return c ;
}
