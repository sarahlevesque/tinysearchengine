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
char** tokenize(char* query, const char* delimeter, size_t lengthOfQuery);

/**************** local functions ****************/
/* not visible outside this file */
/* none */

char* normalizeWord(char* word){
    // normalize word
    for (int i = 0; word[i] != '\0'; i++) {
        if(!isalpha(word[i])){
            fprintf(stderr, "Error: please provide a query without any special characters");
            return NULL;
        }
        word[i] = tolower(word[i]);
    }
    return word;
}

char** tokenize(char* query, const char* delimeter, size_t lengthOfQuery){
    char** tokens = malloc((lengthOfQuery +1) * sizeof(char*)); // allocate memory for tokens array
    if(tokens == NULL) {
        return NULL; // failure to allocate memory
    }
    int count = 0;
    char* firstWord = query;
    char* lastWord = NULL;

    while((lastWord = strpbrk(firstWord, delimeter)) != NULL) {
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
            word[tokenLength] = '\0'; // add an ending the word's characters

            tokens[count] = word;
            count++;
        }
        firstWord = lastWord + 1; // adjust the start pointer after reading word
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
    