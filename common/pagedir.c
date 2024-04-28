/* 
 * pagedir.c - 'pagedir' module
 *
 * see pagedir.h for more information
 *
 * Sarah Levesque
 * Lab 4, CS50 Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "webpage.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
/* none */

/**************** global types ****************/

/**************** global functions ****************/
/* that is, visible outside this file */
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** local functions ****************/
/* not visible outside this file */

/**************** pagedir_init() ****************/
/* see pagedir.h for more details */
bool pagedir_init(const char* pageDirectory){
    char* pathname = malloc(sizeof(char) * FILENAME_MAX);
    if(pathname == NULL){
        mem_free(pathname);
        return false; // failed to allocate memory
    }
    char* crawlMark = ".crawler"; // create ending of file name for use

    sprintf(pathname, "%s/%s", pageDirectory, crawlMark);
    
    // open the file for writing
    if(pathname != NULL){
        FILE* file = fopen(pathname, "w+");
        if(file == NULL) return false;
        fclose(file);
    } 
    mem_free(pathname);
    
    return true;
}

/**************** pagedir_save()) ****************/
/* see pagedir.h for more details */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID){
   
    char* filename = malloc(sizeof(char) * FILENAME_MAX);
    sprintf(filename, "%s/%d", pageDirectory, docID);
    
    FILE* file = fopen(filename, "w");
    if(file == NULL){
        fprintf(stderr, "Error: could not open file %s", filename);
        // free filename also
        return;
    }

    fprintf(file, "%s\n", webpage_getURL(page));
    fprintf(file, "%d\n", webpage_getDepth(page));
    fprintf(file, "%s\n", webpage_getHTML(page));
    fclose(file);
    
    mem_free(filename);
}