/* 
 * indexer.c - a standalone program that reads the document files produced by the TSE crawler, 
 * builds an index, and writes that index to a file
 *
 * Sarah Levesque
 * Lab 5, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "pagedir.h"
#include "webpage.h"
#include "mem.h"
#include "index.h"
#include "word.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/

/**************** global functions ****************/
/* that is, visible outside this file */
void indexBuild(const char* pageDirectory, hashtable_t** index);
void indexPage(int docid, webpage_t* page, hashtable_t* index);

/**************** local functions ****************/
/* not visible outside this file */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void deleteset(void* item);

/**************** main() ****************/
/* parses arguments and initializes other modules
*/
int main(const int argc, char* argv[]){
    char* pageDirectory = NULL;
    char* indexFilename = NULL;
    parseArgs(argc, argv, &pageDirectory, &indexFilename);
    hashtable_t* index = NULL;
    indexBuild(pageDirectory, &index);
    FILE* fp = fopen(indexFilename, "w");
    invertedIndexFile(fp, index); // write the index data structure into the output file
    fclose(fp);
    
    return 0;
}

/**************** parseArgs() ****************/
/* Given the caller provided arguments with the function call
*  We guarantee that:
*     if unsuccessful, do not return. 
*     arguments from the user will be extracted into provided parameters
*/
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename){
    // ensure that all the required arguments have been provided
    if(argc != 3){
        fprintf(stderr, "Usage: provide a valid pageDirectory and indexFilename\n");
        exit(1);
    }

    // initialize pageDirectory
    *pageDirectory = argv[1];
    FILE* fp = fopen(*pageDirectory, "r");
    if(fp == NULL){
        fprintf(stderr, "Usage: failed to initialize and find directory %s\n", *pageDirectory);
        exit(1);
    }
    fclose(fp);
    // check to see if directory has yet been crawled
    char* filePath = malloc(sizeof(char) * FILENAME_MAX);
    char* crawler = ".crawler";
    sprintf(filePath, "%s/%s", *pageDirectory, crawler); // filepath constructor for directory/.crawler file
    if (access(filePath, F_OK) == -1) {
        fprintf(stderr, "Usage: please provide a crawled directory, %s is not yet crawled\n", *pageDirectory);
        exit(1);
    }
    mem_free(filePath);

    // store indexFilename and attempt to open file
    *indexFilename = argv[2];
    FILE* file = fopen(*indexFilename, "r");
    if(file == NULL){
        fprintf(stderr, "Usage: failed to open file %s\n", *indexFilename);
        exit(1);
    }
    fclose(file);
    return;
}

/**************** indexBuild() ****************/
/* Using the provided pageDirectory and an empty hashtable
*  We guarantee that:
*    the provided crawled file will be indexed; each file will be written into a webpage_t object;
*    each webpage object will be used by indexPage(), resulting in a constructed hashtable of sets of words and counters.
*/
void indexBuild(const char* pageDirectory, hashtable_t** index){
    *index = hashtable_new(200);
    if(*index == NULL){
        fprintf(stderr, "Error: failed to create hashtable\n");
        return;
    }
    bool stillFilesToOpen = true;
    char* pathname = malloc(sizeof(char) * FILENAME_MAX);
    if(pathname == NULL){
        hashtable_delete(*index, deleteset);
        return; // failed to allocate memory
    }
    
    int docID = 1;
    // loop through all the crawled files in the directory until we have no more to open
    while(stillFilesToOpen){
        sprintf(pathname, "%s/%d", pageDirectory, docID); // construct path to crawled file for this docID
        FILE* file = fopen(pathname, "r");
        if(file == NULL){
            stillFilesToOpen = false;
            mem_free(pathname);
            break;// could not find file
        }
        webpage_t* loadedPage = indexLoad(file);
        fclose(file);
        if(loadedPage == NULL) {
            mem_free(pathname);
            hashtable_delete(*index, deleteset);
            return;
        }
        // successfully loaded the page, now index it
        indexPage(docID, loadedPage, *index);
        webpage_delete(loadedPage);
        docID++;
    }
}


void indexPage(int docid, webpage_t* page, hashtable_t* index){
    int pos = 0;
    char* word = NULL;
    // while we still have words to check
    while((word = webpage_getNextWord(page, &pos)) != NULL){
        // ignore any html
        if(strlen(word) < 3) {
            mem_free(word);
            continue;
        }
        char* normalizedword = normalizeWord(word);

        // convert int id to string for hashtable storage key
        int idlength = snprintf(NULL, 0, "%d", docid) + 1;
        char* id = malloc(idlength);
        if(id == NULL){
            mem_free(normalizedword); // failed to allocate memory
            return;
        }
        snprintf(id, idlength, "%d", docid);

        // we have not seen a set in this slot
        if(hashtable_find(index, normalizedword) == NULL){
            set_t* wordAndCounter = set_new();
            if (wordAndCounter == NULL) {
                mem_free(id);
                mem_free(normalizedword);
                return; // failed to initalize set
            }
            counters_t* count = counters_new();
            if (count == NULL) {
                mem_free(id);
                mem_free(normalizedword);
                set_delete(wordAndCounter, deleteset);
                return; // failed to initalize counter
            }
            counters_add(count, docid);
            set_insert(wordAndCounter, normalizedword, count);
            hashtable_insert(index, normalizedword, wordAndCounter);
        } else{
            // word has been seen before, increment count for this docID
            counters_t* existingCount = set_find(hashtable_find(index, normalizedword), normalizedword);
            counters_add(existingCount, docid);
            set_insert(hashtable_find(index, normalizedword), normalizedword, existingCount);
        }
        mem_free(normalizedword);
        mem_free(id);
    }
    return;
}

/* Helper function to delete set in the case that some memory cannot be allocated
*/
static void deleteset(void* item)
{
    if(item != NULL){
        mem_free(item);
        item = NULL;
    }
}