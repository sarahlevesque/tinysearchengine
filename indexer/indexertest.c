/* 
 * indexertest.c - given a file produced by the indexer and a file that the index should be written,
 *      the old file will be loaded into an inverted-index data structure
 *
 * Sarah Levesque
 * Lab 5, CS50 Spring 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "mem.h"
#include "webpage.h"
#include "index.h"
#include "word.h"

/**************** global functions ****************/
void loadNewIndex(char* oldIndexFile, char* newIndexFile);

int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Error: provide an old indexed file and a new file to write into");
        exit(1);
    }
    char* oldIndexFile = argv[1];
    char* newIndexFile = argv[2];
    loadNewIndex(oldIndexFile, newIndexFile);

    return 0;
}
/**************** loadNewIndex() ****************/
/* load the index from the oldIndexFilename into an inverted-index data structure.
*  create a file newIndexFilename and write the index to that file
*/
void loadNewIndex(char* oldIndexFile, char* newIndexFile){
    // try to access provided files, exit if not possible
    FILE* oldFile = fopen(oldIndexFile, "r");
    if(oldFile == NULL){
        fprintf(stderr, "Usage: provide a valid already indexed file\n");
        exit(1);
    }
    FILE* newFile = fopen(newIndexFile, "w");
    if(newFile == NULL){
        fprintf(stderr, "Usage: provide a a valid file\n");
        exit(1);
    }
    hashtable_t* index = hashtable_new(200);
    char* linecontent = malloc(sizeof(char) * FILENAME_MAX);

    while (!feof(oldFile)) {
        // for every line, build a set of counter pairs
        if(fgets(linecontent, FILENAME_MAX, oldFile)!= NULL){
            linecontent[strcspn(linecontent, "\n")] = '\0';
            // use strtok to parse through the line splitting by space
            char* word = strtok(linecontent, " ");
            int wordCount = 1;
            set_t* wordAndCounters;
            counters_t* ctrs = counters_new();
            char* firstWord;

            if(word != NULL && wordCount == 1){
                wordAndCounters = set_new();
                firstWord = word;
                wordCount++;
            }

            while((word = strtok(NULL, " ")) != NULL){
                // start parsing docID and counts
                int docid = atoi(word);
                word = strtok(NULL, " "); // keep getting words
                int count = atoi(word);
                counters_add(ctrs, docid);
                counters_set(ctrs, docid, count);
            }

            set_insert(wordAndCounters, firstWord, ctrs);
            hashtable_insert(index, firstWord, wordAndCounters);
        } else {
            mem_free(linecontent);
        }
    }
    // build new file
    invertedIndexFile(newFile, index);
    fclose(oldFile);
    fclose(newFile);
}