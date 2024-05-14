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
char** tokenize(char* query, const char* delimiter, size_t lengthOfQuery);

/**************** local functions ****************/
/* not visible outside this file */
/* none */

char* normalizeWord(char* word){
    // normalize word
    if(word == NULL){
        return NULL;
    }
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}

char** tokenize(char* query, const char* delimiter, size_t lengthOfQuery){
    if(lengthOfQuery < 3){
        fprintf(stderr, "Error: invalid query, try again");
        return NULL;
    }
    char** tokens = malloc((lengthOfQuery +1) * sizeof(char*)); // allocate memory for tokens array
    if(tokens == NULL || query == NULL || delimiter == NULL) {
        return NULL; // failure to allocate memory
    }
    int count = 0;
    char* firstWord = query;
    char* lastWord = strpbrk(firstWord, delimiter); // start to break by delimiter
    
    while (lastWord != NULL) {
        size_t tokenLength = lastWord - firstWord;

        if(tokenLength > 0){
            char* word = malloc((tokenLength + 1) * sizeof(char));
            if(word == NULL){
                for (int i = 0; i < count; i++) {
                    free(tokens[i]);
                }
                free(tokens);
                return NULL; // failure to allocate memory for word
            }

            strncpy(word, firstWord, tokenLength);
            word = normalizeWord(word);
            if(word == NULL) return NULL;
            word[tokenLength] = '\0'; // add an ending the word's characters

            tokens[count] = word;
            count++;
        }
        firstWord = lastWord + 1; // adjust the start pointer after reading word
        lastWord = strpbrk(firstWord, delimiter);
    }

    // check that we have read every token, if not, read the last one
    if(*firstWord != '\0'){
        size_t l = strlen(firstWord);

        char* word= malloc((l + 1) * sizeof(char));
        if (word == NULL) {
            // free the array
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return NULL; // failed to allocate memory
        }

        strcpy(word, firstWord);
        tokens[count] = word;
        count++;
    }
    tokens[count] = NULL; // make it so we have a NULL word in the array
    // tells us where to stop reading

    return tokens; // return array
}
    