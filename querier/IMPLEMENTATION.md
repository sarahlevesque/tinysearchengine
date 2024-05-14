# CS50 TSE Querier
## Implementation Spec
## Sarah Levesque

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data Structures
We use three data structures: a `bag` of char* subqueries that need to be scored, and a `set` for each subquery, which maps to a `counter` of docIDs to scores/occurences for that specific subquery in that doc.

## Control Flow
The querier is implemented in one file 'querier.c', with 5 main functions and several helper/supporting functions.

### main
The `main` function keeps a connection for the user to keep searching with queries through stdin. It calls parseArgs, parseQuery, calculateScores, and ensures the function prints before requesting another query.

### parseArgs
Given arguments from the command line, extract them into the function parameters; return only if successful.
* for `pageDirectory`, try to open the directory and ensure it is crawled
* for `indexFilename`, try to read from it and construct and inverted index data structure as indexer does

### parseQuery
Parse the query into scorable subqueries.
Pseudocode:
    construct a new bag to hold subqueries
    tokenize query
    while the word is not null
        normalize the word
        if the word is and or or
            return (and or or cannot be first)
        while the word is not or (which is what we use to split subqueries)
            skip words less than 3 letters long
            add the word to the subquery if it's valid
            if the word is and
                if the next word is null
                    return(can't end with and)
                if the next word is and or or
                    return (can't end with and or or)
            if the next word is null
                hit the last token, so construct our subquery
            if the next word is or
                if the word after is NULL
                    return (can't end with or)
            if the next word is and
                if the word after is NULL
                    return (can't end with and)
                if the word after is AND or OR
                    return (can't follow and with or or or)
        insert the subquery into the bag if it is valid

### calculateScores
For every subquery, score it and combine to a resulting set and counterset.
Pseudocode:
    while we still have subqueries to score
        make a subquery set and counter
        while we still have files in the crawled directory to open
            score the subquery against that file
            update the counter
        merge the counter with the result counter
        delete the subquery set
    print the results using printResults

### printResults
Given a the result set, construct a sorted list of docIDs from greatest to smallest score, and print urls and scores by the order of said list.
Pseudocode:
    sort the list using the counter
    print matching documents

## Other modules

### word
We create a re-usable module `word.c` to handles the *normalizeWord*  and *tokenize* mentioned in the design (changing the letters to all be lowercase and splitting by delimiter) and as required in the spec.
Pseudocode for tokenize:
    ignore queries less than 3 letters
    construct a tokens array
    loop through the words using pointers to first and last words
    while the last word isn't null
        normalize each word
        if we reach the delimiter, construct a substring
        insert that substring into the tokens array
    update the first word to be the second word

### index
We add a function to the index module which, given an indexed file, is able to construct an inverted index data structure.
Pseudocode:
    open old index file
    build hashtable
    build a set of word to counters for each line of the file
    insert the set into the index

### libcs50

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### querier
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

```c
void parseQuery(bag_t** subqueries, char* query);
void calculateScores(char* pageDir, bag_t* subqueries, hashtable_t* index);
int* sortQueriedDocs(counters_t* counters, int numOfDocs, int* arraySize);
void printResults(set_t* resultSet, char* runningQuery, int totalDocs, char* pageDir);
```

### word
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
char* normalizeWord(char* word);
char** tokenize(char* query, const char* delimiter, size_t lengthOfQuery);
```

## Error handling and recovery
All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a return to requery.

Invalid queries are handles with rigorous if statement checks for invalid input. All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

## Testing plan
Here is an implementation-specific testing plan.

### Unit testing
There are only two units (querier and word).
The querier represents the whole system and is covered below.
The word unit is tiny; it could be tested using a small C 'driver' to invoke its functions with various arguments, but it is likely sufficient to observe its behavior during the system test.

### Integration/system testing

We write a script `testing.sh` that invokes the querier several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Next, runs over all various CS50 websites, querying searches from those sites.
Run that script with `bash -v testing.sh` so the output of crawler is intermixed with the commands used to invoke the crawler.

Verify correct behavior by studying the output and comparing them to the files produced by the indexer. Use `fuzzquery.c` to test random output into the file.