/* 
 * index.h - header file for CS50 'index' module
 * 
 * The module 'index' handles all index related functions, including loading index pages and writing output.
 *
 * Sarah Levesque, CS50 Spring 2024
 */


#include <stdio.h>


/**************** global types ****************/
/*none*/

/**************** functions ****************/

/**************** indexLoad() ****************/
/* Given a crawled file, creates and loads a tse webpage
*
* Caller Provides:
*    a valid crawled file
* We guarantee:
*    if the file is valid and has been crawled, we build and return a webpage for it
*/
webpage_t* indexLoad(FILE* file);

/**************** invertedIndexFile() ****************/
/* Given a constructed hashtable (inverted index), construct an output indexed file
*
* Caller Provides:
*    a valid constructed hashtable with sets containing words seen and counters for them
* We guarantee:
*    if the hashtable has been indexed, we will construct an output file for it
*/
void invertedIndexFile(FILE* fp, hashtable_t* index);



