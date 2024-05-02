# CS50 TSE Indexer - Sarah Levesque Lab 5
## Implementation Spec

In this document we reference [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 
We use three data structures: a `counter` to track the frequencies for how often specific words have been seen on specific pages, counting docID to frequency count. We use a `set` to store the word visited and the `counters` for that word. We then use an `index` or `hashtable` to store all of these sets. All start empty.
The size of the hashtable (slots) is impossible to determine in advance, so we use 200.

# Control flow
The indexer is implemented in the `indexer.c` file, with 4 main functions. It also depends on the implementation of index functions in the `index.c` file of the common library.

### main
The `main` function simply calls `parseArgs`, `indexBuild`, `invertedIndexFile`, then exits zero.

### parseArgs
Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, ensure it is a directory that can be opened and ensure it is crawled
* for `indexFilename`, ensure it is a file that can be opened and written to
* if any trouble is found, print an error to stderr and exit non-zero.

### indexBuild

Given a valid directory and an empty hashtable, read each file and construct an inverted index data structure.
Pseudocode:

	initialize the hashtable 
    initialize a count of documents
    while there are still files to visit in the pageDirectory
		build the pathname for the document we are visiting
        try to open the document
        load a webpage object for that document
        index the page
		delete the webpage
        increment the count of documents

### indexPage

Given a valid webpage and index data structure, the function steps through each word of the webpage, ensures it is a word we want to consider and is not HTML script, and then normalizes it. It then counts the frequency of that word in the index.
Pseudocode:

	while we still have words to get on the page
        ignore words < 3 letters long 
        normalize all words
        if we have not seen the word in the index
            create a set for it
            create a counter for it
            insert into the index
        else
            find the existing count for the word in the index and increment
    

## Other modules

### index

We create a re-usable module `index.c` to handles all index related functions, including loading index pages and writing output (as required in the spec).
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to normalize a word in one place... anticipating future use by the Querier.
Pseudocode for indexLoad:
    while we have lines of a crawled file left to read
        strip the line of its new line character
        if it's line one, copy that to the url
        if it's line two, copy that to the docID
        if it's not, break out of the loop
    construct a webpage using the url and docID
    fetch the HTML
    return the loaded page

Pseudocode for invertedIndexFile:

    iterate through the hashtable using a helper function
    iterate through a set and print using a helper function
    iterate through a counter and print using a helper function

### word

We create a re-usable module `word.c` to handles the *normalizeWord*  mentioned in the design (changing the letters to all be lowercase) and as required in the spec.
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to normalize a word in one place... anticipating future use by the Querier.
Pseudocode:
    
    loop through each character of the word until the end
        use the _toLower()_ function to make it lowercase

### libcs50

We leverage the modules of libcs50, most notably `set`, `counter`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.
Indeed, `webpage_fetch` enforces the 1-second delay for each fetch, so our indexer need not implement that part of the spec.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
void indexBuild(const char* pageDirectory, hashtable_t** index);
void indexPage(int docid, webpage_t* page, hashtable_t* index);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void deleteset(void* item);
```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
webpage_t* indexLoad(FILE* file);
void invertedIndexFile(FILE* fp, hashtable_t* index);
static void wordandcountprint(void* arg, const char* key, void* item);
static void iterateIndex(void* arg, const char* key, void* item);
static void countsPrint(void* arg, const int key, const int count);
static void deleteindex(void* item);
static void deleteset(void* item);
```

### word
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
char* normalizeWord(char* word);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters. Certain errors are caught and handled internally, like in parseArgs for example.

## Testing plan

Here is an implementation-specific testing plan.

### Integration/system testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Second, a run with valgrind over a moderate-sized test case (such as `toscrape` at depth 1). We do not include this in testing.sh as bash and valgrind do not work together well.
Third, we build indices from cs50dev/tse shared output files, and we compare them to the output of cs50dev indice files.

### indexertest.c
Lastly, we test the construction of our index by using `indexertest.c`.
This program reconstructs an inverted index data structure from the output indexed file, and then outputs it again to another index file. It then can be compared for any potential differences, which ensures the correct implementation of the inverse index data structure.




