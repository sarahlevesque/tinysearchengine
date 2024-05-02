# CS50 Lab 5
## CS50 Spring 2024

### indexer

Indexer anticipates receiving user input, specifically a valid `pageDirectory` of files written by the `crawler` module, and the `indexFileName` where the output of the index data is written. The program visits every document in the `pageDirectory` and constructs it into a webpage object, and then scans the words on the page to count frequencies of occurence, using `sets` of `counters`.

### Usage
The user provides a valid existing and crawled `pageDirectory` and the name of an index file to write output to. The indexer guarantees that if this directory has been crawled and exists, it will create an inverted index data structure and write words and frequencies by document to the output file.

The _parseArgs_ function ensures the `pageDirectory` is existing and crawled, and initializes other variables.

The _indexBuild_ function creates a new `index` object, loops over documentIDs in the crawled `pageDirectory`, and loads a webpage if it can. It then passes this to the _indexPage_ function.

The _indexPage_ function steps through each word of the webpage, ensures it is a word we want to consider and is not HTML script, and then normalizes it. It then counts the frequency of that word in the index.

```c
void indexBuild(const char* pageDirectory, hashtable_t** index);
void indexPage(int docid, webpage_t* page, hashtable_t* index);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
```

### Implementation

Given valid user provided arguments, `indexer.c` will index the provided crawled documents and output it into the provided indexFile.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `index.c` - a file that provides helper index functions, found in common library
* `indexertest.c` - a test file that stores the output from indexer into a inverted index structure and then outputs it again to a new file, ensuring proper storage
* `indexcmp`- executable program to compare output of indexer with cs50dev provided indices

### Compilation

To compile, simply `make` from the tse directory
