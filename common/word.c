/* 
 * word.c - provides function to normalize word
 *
 * Sarah Levesque
 * Lab 5, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
/* none */

/**************** global types ****************/
/* none */

/**************** global functions ****************/
/* that is, visible outside this file */
char* normalizeWord(char* word);

/**************** local functions ****************/
/* not visible outside this file */
/* none */

char* normalizeWord(char* word){
    // normalize word
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}