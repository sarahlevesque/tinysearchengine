/* 
 * word.h - header file for CS50 'word' module
 * 
 * The module 'word' handles checking a word is normalized
 *
 * Sarah Levesque, CS50 Spring 2024
 */

#include <stdio.h>


/**************** global types ****************/
/*none*/

/**************** functions ****************/

/**************** normalizeWord() ****************/
/* Given a char* word, it is normalized
*
* Caller Provides:
*    a valid word
* We guarantee:
*    it is normalized to lowercase
*/
char* normalizeWord(char* word);

/**************** tokenize() ****************/
/* Given a char* query, it is tokenized into an array of strings
*
* Caller Provides:
*    a valid query
* We guarantee:
*    each word is normalized to lowercase, and added to the array
*/
char** tokenize(char* query, const char* delimiter, size_t lengthOfQuery);