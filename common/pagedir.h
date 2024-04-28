/* 
 * pagedir.h - header file for CS50 'pagedir' module
 * 
 * The module 'pagedir' handles initializing and writing pages to the pageDirectory,
 * and marking them as a Crawler-produced pageDirectory.
 *
 * Sarah Levsque, CS50 Spring 2024
 */


#include <stdio.h>

/**************** global types ****************/
typedef struct webpage webpage_t;  // opaque to users of the module

/**************** functions ****************/

/**************** pagedir_init ****************/
/* Constructs pathname for .crawler file and opens it for writing
*
* Caller Provides:
*   a valid pathname for the pageDirectory for the .crawler file to be constructed in
* We guarantee:
*   an invalid pageDirectory is ignored.
*   a pathname is constructed and a file is opened for writing in a valid pageDirectory
*/
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* Constructs pathname for .crawler file and opens it for writing
*
* Caller Provides:
*   a valid pageDirectory for the .crawler file to be constructed in
*   a valid webpage struct containing the pathname to the pagefile
*   a valid docID for the page
* We guarantee:
*   an invalid pageDirectory is ignored; an invalid webpage is ignored; an invalid docID is ignored.
*   a pathname is constructed and a file is opened for writing in a valid pageDirectory
*   the URL, depth, and contents of the webpage are printed
*   the file is closed
*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
