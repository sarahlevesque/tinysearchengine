# CS50 Lab 6
## CS50 Spring 2024
## Sarah Levesque

### querier

Querier anticipates receiving user input, specifically a valid `pageDirectory` of files written by the `crawler` module, and the `indexFileName` where the output of the index data is written. The user inputs a search query into standard input, and the program splits that query into subqueries. The program visits every document in the `pageDirectory` and scores each subquery, and prints a sorted list of documents by highest to lowest scores, relevant to the user's query.

### Usage

The user provides a valid existing and crawled `pageDirectory` and the name of an index file that has been indexed. The indexer guarantees that if this directory has been crawled and exists, it will create an inverted index data structure from the indexed file.

The user also provides a query. If this query is valid, it will not include:
- only words under 3 letters
- beginning with `and` or `or`
- ending with `and` or `or`
- having repeating `and`s or `or`s
- special characters
If the query is valid, the program prints the documents sorted by relevancy. Each document is scores according to how frequently search terms appear within them.

The _parseArgs_ function ensures the `pageDirectory` is existing and crawled, and initializes other variables.

The _parseQuery_ function ensures the user provided a valid query. It then splits the query into subqueries, where a subquery is the portion of the sentence leading up to an `or` or the end of the file. For example, the sentence "dog and cat or fish" holds two subqueries: "dog and cat" and "fish". These subqueries are then individually scored.

The _calculateScores_ function takes a `bag_t*` of subqueries and extracts each one, individually scoring each against the document IDs using the score function. For each subquery, make a set, and construct a counter. Use the index to find the scores for each docID. Copy the resulting subquerySet counts into a resultSet, which supports "or" functionality.

The _sortQueriesDocs_ function takes a result counter and sorts an array of documentIDs by greatest to smallest score.

The _printResults_ function uses the resultSet of valid docIDs with scores > 0 and sorts them by score. Documents will then be printed to stdout by biggest to smallest score.

```c
void parseQuery(bag_t** subqueries, char* query);
void calculateScores(char* pageDir, bag_t* subqueries, hashtable_t* index);
int* sortQueriedDocs(counters_t* counters, int numOfDocs, int* arraySize);
void printResults(set_t* resultSet, char* runningQuery, int totalDocs, char* pageDir);
```

### Implementation

Given valid user provided arguments and a valid query, `querier.c` will print a queried list of scored URLs according to relevancy to the query.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `querier.c` - querier program file
* `DESIGN.md` - design spec
* `IMPLEMENTATION.md` - implementation spec

### Compilation

To compile, simply `make` from the tse directory

### Testing

To generate testing words, use make fuzzquery. This will output random words from fuzz testing.

To generate a random tests from the testing.sh file, use make test.

