/* 
 * querier.c - a standalone program that reads the index file produced by the TSE Indexer, 
 * and page files produced by the TSE Querier, and answers search queries submitted via stdin.
 *
 * Sarah Levesque
 * Lab 6, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashtable.h"
#include "set.h"
#include "file.h"
#include "counters.h"
#include "pagedir.h"
#include "webpage.h"
#include "mem.h"
#include "index.h"
#include "word.h"
#include "bag.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct {
    int docID;
    int score;
} counterNode_t;

/**************** global types ****************/
/* none */

/**************** local functions ****************/
/* not visible outside this file */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static int compareScore(const void * score1, const void * score2);
static void deleteindex(void* item);
static void deleteindexset(void* item);
static void deleteset(void* item);
static void buildEmptySet(set_t** resultSet, int totalDocs, char* key);
static int score(char* query, hashtable_t* index, int docID);
static void handleErrorInParsingQuery(char* subquery, bag_t** subqueries, char** tokens);

/**************** global functions ****************/
/* that is, visible outside this file */
void parseQuery(bag_t** subqueries, char* query);
void calculateScores(char* pageDir, bag_t* subqueries, hashtable_t* index);
int* sortQueriedDocs(counters_t* counters, int numOfDocs, int* arraySize);
void printResults(set_t* resultSet, char* runningQuery, int totalDocs, char* pageDir);


/**************** main() ****************/
/* parses arguments and initializes other modules
*/
int main(const int argc, char* argv[]){
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    parseArgs(argc, argv, &pageDirectory, &indexFilename);
    hashtable_t* index = NULL;
    loadNewIndexTable(&index, indexFilename);
    if(index == NULL){
        fprintf(stderr, "Usage: failed to initialize index for file %s, provide an indexed file\n", indexFilename);
        return 1;
    }
    while (1) {
        char* query = malloc(sizeof(char) * FILENAME_MAX);
        if (isatty(fileno(stdin))) { // check if input is from stdin
            printf("Query: "); // prompt
        }

        bag_t* subqueries = NULL; 
        if (fgets(query, FILENAME_MAX, stdin) == NULL) { // read query from stdin
            if(query == NULL){
                fprintf(stderr, "Error: allocating memory for query, try again");
            }
            if (feof(stdin)) { // if user uses CTRL D
                mem_free(query);
                break;
            }
        }
        size_t length = strlen(query);
        if (length > 0 && query[length - 1] == '\n') {
            query[length - 1] = '\0'; // remove new line character if in query
        }

        // if we have read all the characters and there is no error in 
        parseQuery(&subqueries, query); // build a bag of subqueries to score 
        if(subqueries == NULL) continue; // check to see if we were able to process the query
        
        // now we score those subqueries (set (key is word) (item is counters of docID to count))
        calculateScores(pageDirectory, subqueries, index);
        bag_delete(subqueries, deleteset);
        mem_free(query);
        query = NULL;
    }
    hashtable_delete(index, deleteindex);
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
    if (access(filePath, F_OK) == -1) { // can we find a crawled file?
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

/**************** parseQuery() ****************/
/* Given the caller provided a query to stdin
*  We guarantee that:
*       the query will be split by or into subqueries, and a bag will be constructed of these subqueries
*/     
void parseQuery(bag_t** subqueries, char* query){
    if(query == NULL) return;
    *subqueries = bag_new();
    char* OR = "or";
    char* AND = "and";
    // split query by spaces into tokens
    char** tokens = tokenize(query, " ", strlen(query));
    
    if(tokens == NULL){
        return;
    }

    int count = 0;
    char* word;
    char* nextWord;
    // while we aren't at the end of the query
    while((word = tokens[count]) != NULL){
        nextWord = tokens[count + 1]; // track the next word

        word = normalizeWord(word); // ensure each word did end up normalized
        if(word == NULL) {
            count ++;
            break;
        }
        if(nextWord != NULL){
            nextWord = normalizeWord(nextWord);
        }

        // check if and or or is first
        if(strcmp(word, AND) == 0 || strcmp(word, OR) == 0){  
            fprintf(stderr, "Error: 'and' and 'or' cannot be first\n");
            bag_delete(*subqueries, deleteset);
            *subqueries = NULL;
            mem_free(tokens);
            return;
        }
        
        char* subquery = malloc(sizeof(char) * FILENAME_MAX);
        if(subquery == NULL){
            fprintf(stderr, "Error: memory allocation failed\n");
            bag_delete(*subqueries, deleteset);
            *subqueries = NULL;
            free(tokens);
            return;
        }
        subquery[0] = '\0';

        while(strcmp(word, OR) != 0){ // while we aren't at or (when we need to make a new subquery)
            if(word == NULL) {
                break;
            }
           
            if(strlen(word) < 3) {
                count +=2;
                word = NULL;
                break; // skip short words
            }

            strcat(subquery, word);
            if(strcmp(word, AND) == 0){  
                // if we hit an "and" check the word after to make sure its not and or or
                if(nextWord == NULL){
                    fprintf(stderr, "Error: cannnot end query with and\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
                if(strcmp(nextWord, AND) == 0){
                    fprintf(stderr, "Error: and cannot be followed with and\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
                if(strcmp(nextWord, OR) == 0){
                    fprintf(stderr, "Error: and cannot be followed with or\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
            }
            // construct our subquery
            if(nextWord == NULL){
                count ++;
                word = NULL;
                break;
            }
            if(strcmp(nextWord, OR) == 0){ // if next word is or, break the subquery
                if(tokens[count +2] == NULL){
                    count ++;
                    word = NULL;
                    fprintf(stderr, "Error: cannot finish query with or\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
                count +=2;
                word = NULL;
                break;
            }
            if(strcmp(nextWord, AND) == 0){ // if next word is and, break the subquery
                if(tokens[count+2] == NULL){
                    count ++;
                    word = NULL;
                    fprintf(stderr, "Error: cannot finish query with and\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
                if(strcmp(tokens[count+2], AND) == 0 || strcmp(tokens[count+2], OR) == 0){
                    count ++;
                    word = NULL;
                    fprintf(stderr, "Error: and cannot be followed with and or or\n");
                    handleErrorInParsingQuery(subquery, subqueries, tokens);
                    return;
                }
            }
            strcat(subquery, " "); 
            
            count += 1;
            word = tokens[count];
            nextWord = tokens[count + 1];
        }
        // now have a constructed subquery, so add it to the bag
        if(subquery[0] != '\0'){ // if subquery isn't empty
            bag_insert(*subqueries, subquery);
        }
    }
    // // now we have a bag of every subquery we need to score
    for (int i = 0; tokens[i] != NULL; i++) {
        mem_free(tokens[i]);
    }
    mem_free(tokens);
}

/* handles parsing errors of query */
static void handleErrorInParsingQuery(char* subquery, bag_t** subqueries, char** tokens){
    mem_free(subquery);
    bag_delete(*(subqueries), deleteset);
    *subqueries = NULL;
     for (int i = 0; tokens[i] != NULL; i++) {
        mem_free(tokens[i]);
    }
    mem_free(tokens);
}

/**************** score() ****************/
/* Given a query and valid index
*  We guarantee that:
*     the subquery will be scored from the index for that specific docID
*/
static int score(char* query, hashtable_t* index, int docID){
    // for every docID, we want to score the query
    // search for the words
    char** tokens = tokenize(query, " ", strlen(query)); // split up the query into an array
    int count = 0;
    int currMinScore = 100000000;
    while(tokens[count] != NULL){
        if(strcmp(tokens[count], "and") == 0){
            count ++;
            continue; // don't need to count and
        }
        int currScore = counters_get(set_find(hashtable_find(index, tokens[count]), tokens[count]), docID);
        
        if(currScore != 0){
            // if we can get the score for this word from the index
            if(currScore < currMinScore) {
                currMinScore = currScore;
            }
            count ++;
            continue;
        } else {
            // word not found in the index for and sequence
            currScore = 0;
            for (int i = 0; tokens[i] != NULL; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return currScore;
        }
    }
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
    return currMinScore;
}

/**************** calculateScores() ****************/
/* Given a valid pageDirectory, bag of subqueries, and index
*  We guarantee that:
*    Each subquery within the bag will be individually scored by score function.
*    For each subquery, make a set, and construct a counter.
*    Use the index to find the scores for each docID.
*    Copy the resulting subquerySet counts into a resultSet, which supports
*    "or" functionality.
*/
void calculateScores(char* pageDir, bag_t* subqueries, hashtable_t* index){
    // for each subquery, loop through all the docIDs in pageDir and then make a new counter for each one, if the score is 0 don't add it
    if(index == NULL || subqueries == NULL || pageDir == NULL) return;
    set_t* resultSet = NULL;

    char* runningQuery = NULL;
    runningQuery = malloc(sizeof(char*) * FILENAME_MAX);
    if(runningQuery == NULL){
        mem_free(pageDir);
        return; // memory allocation failed
    }
    runningQuery[0] = '\0';
    bool firstSubqueryAdd = true;
    char* subquery;

    int totalDocs = 0;
    while((subquery = bag_extract(subqueries)) != NULL){
        if(!firstSubqueryAdd){
            strcat(runningQuery, " "); 
        }
        strcat(runningQuery, subquery); 
        firstSubqueryAdd = false;
        set_t* subquerySet = set_new();
        if(subquerySet == NULL) return;
        counters_t* count = counters_new();
        if(count == NULL) return;

        bool stillFilesToOpen = true;
        char* pathname = malloc(sizeof(char) * FILENAME_MAX);
        if(pathname == NULL){
            return; // failed to allocate memory
        }
        
        int docID = 1;
        // loop through all the crawled files in the directory until we have no more to open
        while(stillFilesToOpen){
            sprintf(pathname, "%s/%d", pageDir, docID); // construct path to crawled file for this docID
            FILE* file = fopen(pathname, "r");
            if(file == NULL){
                stillFilesToOpen = false;
                mem_free(pathname);
                break;// could not find file, must have looped through them all
            }
            totalDocs = docID;
            int subqueryScore = score(subquery, index, docID);
            counters_set(count, docID, subqueryScore);
            docID++;
            fclose(file);
        }

        if(subquerySet == NULL){
            counters_delete(count);
        }
        else if(count != NULL){
            set_insert(subquerySet, subquery, count); // one subquery with counters of every docID and frequency in that docID
        }

        char* result = "result";
        if(resultSet == NULL){
            buildEmptySet(&resultSet, totalDocs, result); // initalize empty resultSet with counters at 0 for every docID
        }

        // loop through new subquerySet and update result counters accordingly for new subquery
        for(int i = 1; i <= totalDocs; i++){
            int currScore = counters_get(set_find(resultSet, result), i);
            int newScore = counters_get(set_find(subquerySet, subquery), i);
            counters_set(set_find(resultSet, result), i, newScore + currScore);
        }
        mem_free(subquery);
        set_delete(subquerySet, deleteindexset);
    }
    printf("%s\n", runningQuery);

    printResults(resultSet, runningQuery, totalDocs, pageDir);
    set_delete(resultSet, deleteindexset);
}

/**************** sortQueriedDocs() ****************/
/* Given counters and the number of documents currently being considered
*  We guarantee that:
*       a sorted list of documentIDs will be returned 
*       sorted by scores in the provided result counter.
*/
int* sortQueriedDocs(counters_t* counters, int numOfDocs, int* arraySize){
    if(counters == NULL || numOfDocs <= 0) return NULL;

    int* docIDs = malloc(sizeof(int) * numOfDocs);
    if(docIDs == NULL) {
        fprintf(stderr, "Error: memory allocation failed for docIDs array\n");
        return NULL;
    }

    counterNode_t* toSort = malloc(sizeof(counterNode_t) * numOfDocs);
    if(toSort == NULL) {
        fprintf(stderr, "Error: memory allocation failed for sorting array\n");
        mem_free(docIDs);
        return NULL;
    }

    for(int i = 0; i < numOfDocs; i++){
        counterNode_t counter = {i+1, counters_get(counters, i+1)};
        toSort[i] = counter;
    }

    qsort(toSort, numOfDocs, sizeof(docIDs), compareScore);
    for(int i =0; i< numOfDocs; i++){
        if(counters_get(counters, i+1) != 0){
            (*arraySize)++;
        }
        docIDs[i] = toSort[i].docID;   
    }
    mem_free(toSort);
    return docIDs;
}

/**************** printResults() ****************/
/* Given a constructed resultSet and valid query
*  We guarantee that:
*   The resultSet of valid docIDs with scores > 0 will be sorted by score.
*   Documents will then be printed to stdout by biggest to smallest score.
*/
void printResults(set_t* resultSet, char* runningQuery, int totalDocs, char* pageDir){
    if(resultSet == NULL) return;

    int size = 0;
    char* key = "result";
    int* sortedDocs = sortQueriedDocs(set_find(resultSet, key), totalDocs, &size);
    if(sortedDocs == NULL){
        printf("Matches 0 documents (ranked):\n");
        mem_free(runningQuery);
        return;
    }
    printf("Matches %d documents (ranked):\n", size);

    for(int i=0; i < size; i++){
        //get URL
        char* pathname = malloc(sizeof(char*) * FILENAME_MAX);
        if(pathname == NULL){
            fprintf(stderr, "Error: memory allocation for pathname failed");
            return;
        }
        sprintf(pathname, "%s/%d", pageDir, sortedDocs[i]); // construct path to crawled file for this docID
        FILE* file = fopen(pathname, "r");
        webpage_t* page = indexLoad(file);
        fclose(file);
        
        printf("score   %d  doc %d: %s\n", counters_get(set_find(resultSet, key), sortedDocs[i]), sortedDocs[i], webpage_getURL(page));
        webpage_delete(page);
        mem_free(pathname);
    }
    printf("\n");
    mem_free(runningQuery);
    mem_free(sortedDocs);
}


/**************** buildEmptySet() ****************/
/* Helper function to populate a resultSet with empty counters
*/
static void buildEmptySet(set_t** resultSet, int totalDocs, char* key){
    *resultSet = set_new();
    counters_t* resultCounters = counters_new();
    for(int i = 1; i <= totalDocs; i++){
        counters_add(resultCounters, i);
        counters_set(resultCounters, i, 0); // ensure every counter for docID is created and initalized at 0
    }
    set_insert(*resultSet, key, resultCounters);
}

/**************** compareScore() ****************/
/* Compare function to compare counters
*/
static int compareScore(const void * score1, const void * score2){
    const counterNode_t* doc1 = score1;
    const counterNode_t* doc2= score2;
    return (doc2-> score) - (doc1 -> score);
}

/* Helper function to delete index 
*/
static void deleteindex(void* item)
{
    if(item != NULL){
        set_delete(item, deleteindexset);
        item = NULL;
    }
}
/* Helper function to delete set with counter
*/
static void deleteindexset(void* item)
{
    if(item != NULL){
        counters_delete(item);
        item = NULL;
    }
}

/* Helper function to delete set (or bag, anything with item type value)
*/
static void deleteset(void* item)
{
    if(item != NULL){
        mem_free(item);
        item = NULL;
    }
} 