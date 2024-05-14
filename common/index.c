/* 
 * index.c - supports saving and loading index files
 *
 * Sarah Levesque
 * Lab 5, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "pagedir.h"
#include "webpage.h"
#include "mem.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/

/**************** global functions ****************/
/* that is, visible outside this file */
webpage_t* indexLoad(FILE* file);
void invertedIndexFile(FILE* fp, hashtable_t* index);
void loadNewIndexTable(hashtable_t** index, char* oldIndexFile);

/**************** local functions ****************/
/* not visible outside this file */
static void wordandcountprint(void* arg, const char* key, void* item);
static void iterateIndex(void* arg, const char* key, void* item);
static void countsPrint(void* arg, const int key, const int count);
static void deleteindex(void* item);
static void deleteset(void* item);

/**************** indexLoad() ****************/
/* see index.h for more details */
webpage_t* indexLoad(FILE* file){
    char* url = malloc(sizeof(char) * FILENAME_MAX);
    int id;

    int line = 0;
    char* linecontent = NULL;

    if (url == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        fclose(file);
        return NULL;
    }

    // read through lines and extract url and id
    while (!feof(file)) {
        line++;
        linecontent = malloc(sizeof(char) * FILENAME_MAX);
        if (linecontent == NULL) {
            fprintf(stderr, "Error: failed to allocate memory\n");
            fclose(file);
            mem_free(url);
            return NULL;
        }

        if(fgets(linecontent, FILENAME_MAX, file)!= NULL){
            // ensure the line does not contain a new line character, if it does remove it
            linecontent[strcspn(linecontent, "\n")] = '\0';
            
            if(line == 1){  // store url
                strcpy(url, linecontent);
            }
            else if(line == 2){ // store id number
                id = atoi(linecontent);
            } else {
                mem_free(linecontent);
                break;
            }
        } else {
            mem_free(url);
            mem_free(linecontent);
            fclose(file);
            return NULL;
        }
        mem_free(linecontent);
    }

    webpage_t* loadPage = webpage_new(url, id, NULL);
    if(!webpage_fetch(loadPage)){
        webpage_delete(loadPage);
        mem_free(url);
        return NULL;
    }
    return loadPage;
}

/**************** loadNewIndexTable() ****************/
/* see index.h for more details */
void loadNewIndexTable(hashtable_t** index, char* oldIndexFile){
    // try to access provided files, exit if not possible
    FILE* oldFile = fopen(oldIndexFile, "r");
    if(oldFile == NULL){
        fprintf(stderr, "Usage: provide a valid already indexed file\n");
        exit(1);
    }
    *index = hashtable_new(200);
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
            hashtable_insert(*index, firstWord, wordAndCounters);
        } else {
            mem_free(linecontent);
        }
    }
    fclose(oldFile);
}

/**************** invertedIndexFile() ****************/
/* see index.h for more details */
void invertedIndexFile(FILE* fp, hashtable_t* index){
    //iterate through hashtable, call set_print on every set
    hashtable_iterate(index, fp, iterateIndex);
    hashtable_delete(index, deleteindex);
}

/**************** iterateIndex() ****************/
/* Function used to iterate through hashtable and print set */
static void iterateIndex(void* arg, const char* key, void* item){
    // key is docID, item is set
    FILE* fp = arg;
    set_iterate(item, fp, wordandcountprint);
    fprintf(fp, "\n");
}

/**************** wordandcountprint() ****************/
/* print a set */
static void wordandcountprint(void* arg, const char* key, void* item)
  {
    FILE* fp = arg;
    const char* word = key;
    counters_t* counters = item;
    fprintf(fp, "%s ", word);
    counters_iterate(counters, fp, countsPrint);
}

static void countsPrint(void* arg, const int key, const int count){
    FILE* fp = arg;
    fprintf(fp, "%d %d ", key, count);
}

static void deleteindex(void* item)
{
    if(item != NULL){
        set_delete(item, deleteset);
        item = NULL;
    }
}
static void deleteset(void* item)
{
    if(item != NULL){
        counters_delete(item);
        item = NULL;
    }
}