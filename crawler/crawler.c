/* 
 * crawler.c - program that crawls the web and retrieves webpages starting from a "seed" URL. 
 * It parses the seed webpage, extracts any embedded URLs, then retrieves each of those pages, 
 * recursively, but limiting its exploration to a given "depth"
 *
 * Sarah Levesque
 * Lab 4, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashtable.h"
#include "bag.h"
#include "pagedir.h"
#include "webpage.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/

/**************** global functions ****************/
/* that is, visible outside this file */

/**************** local functions ****************/
/* not visible outside this file */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void deleteitem(void* item);

/**************** main() ****************/
/* parses arguments and initializes other modules
*/
int main(const int argc, char* argv[]){
    // parse provided arguments and assign to local variables
    char* seed = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;
    parseArgs(argc, argv, &seed, &pageDirectory, &maxDepth);
    crawl(seed, pageDirectory, maxDepth);
    return 0; // successfully exit
}

/**************** parseArgs() ****************/
/* Given the caller provided arguments with the function call
*  We guarantee that:
*     if unsuccessful, do not return. 
*     arguments from the user will be extracted into provided parameters
*/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth){
    // ensure that all the required arguments have been provided
    if(argc != 4){
        fprintf(stderr, "Usage: provide a valid URL, seedDirectory, and depth of crawl\n");
        exit(1);
    }
    // parse url
    *seedURL = argv[1];
    char* internalURLreq = "http://cs50tse.cs.dartmouth.edu/tse/";
    *seedURL = normalizeURL(*seedURL);
    if(*seedURL == NULL) {
        fprintf(stderr, "Usage: failed to normalizeURL, please provide a valid internal URL\n");
        exit(1);
    }
    if(strncmp(*seedURL, internalURLreq, strlen(internalURLreq)) != 0) {// check to see if normalized seedURL is an internal URL
        fprintf(stderr, "Usage: seedURL is not an internal URL\n");
        exit(1);
    }

    // initialize seedDirectory
    *pageDirectory = argv[2];
    if(!pagedir_init(*pageDirectory)){
        fprintf(stderr, "Usage: failed to initialize and find directory %s\n", *pageDirectory);
        exit(1);
    }

    // initialize acceptable crawl range
    int depth = 0;
    // check if it is even an int
    char extra; // should be empty
    if (sscanf(argv[3], "%d%c", &depth, &extra) != 1) { // check that we have an integer with no extra characters
        fprintf(stderr, "Usage: please enter only an integer.\n");
        exit(1);
    }
    if(depth < 0 || depth > 10){
        fprintf(stderr, "Usage: maxDepth must be an integer from 0-10\n");
        exit(1);
    }
    *maxDepth = depth;
    return; // successfully exit
}

/**************** crawl() ****************/
/* Implements a page crawler to move through the webpages in a webpage.
*  Caller provides:
*       a valid url, a valid pageDirectory, and a valid maxDepth
*  We guarantee that:
*       the given page will be scanned; any links will be extracted
*       the scanner will determine if the URL has been seen before.
*/
void crawl(char* seedURL, char* pageDirectory, const int maxDepth){
    if(seedURL == NULL || pageDirectory == NULL || maxDepth < 0) {
        mem_free(seedURL);
        mem_free(pageDirectory);
        return;
    }

    // given we have valid parameters, start the crawl
    hashtable_t* visitedURLS = hashtable_new(200);      // hashtable to hold URLS seen
    bag_t* pagesToVisit = bag_new();                    // bag to function as a stack
    int currDepth = 0;                                  // where we are
    int docID = 1;

    // set up the first source page
    webpage_t* source = webpage_new(seedURL, currDepth, NULL);

    bag_insert(pagesToVisit, source);

    // while we still have pages to visit and the depth is in range
    while(pagesToVisit != NULL && currDepth <= maxDepth){
        webpage_t* curr = bag_extract(pagesToVisit);
        sleep(1); // pause for one second

        currDepth = webpage_getDepth(curr);
        if(currDepth > maxDepth){
            webpage_delete(curr);
            continue; // skip this one, but continue checking the bag
        }

        if(curr == NULL) {
            webpage_delete(curr);
            curr = NULL;
            break; 
        }

        if(curr != NULL && currDepth <= maxDepth){
            if(webpage_fetch(curr)){
                pagedir_save(curr, pageDirectory, docID); // save because we successfully fetched
                docID++;
                if(currDepth < maxDepth){         // scan because the depth for internal URLs is in range
                    pageScan(curr, pagesToVisit, visitedURLS);
                }
                webpage_delete(curr); // successfully finished with page so delete
                curr = NULL;
                continue;
            } else {
                webpage_delete(curr);  // failed to fetch webpage
                curr = NULL;
                continue;   // continue checking the bag
            }
        } else{
            webpage_delete(curr); 
            curr = NULL;
            break;
        }
    }
    bag_delete(pagesToVisit, deleteitem);
    hashtable_delete(visitedURLS, deleteitem);
}

static void deleteitem(void* item)
{
    ; // do nothing because we are using urls as items which we already free
}

/**************** pageScan() ****************/
/* Implements a pagescanner.
*  Caller provides:
*       a valid webpage
*  We guarantee that:
*       the given page will be scanned; any links will be extracted
*       the scanner will determine if the URL has been seen before.
*/
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen){
    if(page == NULL || pagesToCrawl== NULL || pagesSeen == NULL) return; // error

    hashtable_insert(pagesSeen, webpage_getURL(page), webpage_getURL(page));
    char* url;
    int pos = 0;

    // now we want to crawl the page and gather all the links
    while((url = webpage_getNextURL(page, &pos)) != NULL){
        if(hashtable_find(pagesSeen, url) == NULL && isInternalURL(url)){
            webpage_t* newpage = webpage_new(url, webpage_getDepth(page) + 1, NULL);
            if(newpage != NULL){
                bag_insert(pagesToCrawl, newpage);
                pos++;  // increment number of links seen on this source page
                hashtable_insert(pagesSeen, url, url);

            } else {
                webpage_delete(newpage);
                }
        }
    }
    mem_free(url);
}
