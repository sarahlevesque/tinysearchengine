# CS50 TSE Querier
## Design Spec
## Sarah Levesque

According to the [Querier Requirements Spec](REQUIREMENTS.md), the TSE *Querier* is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin. In this document we cover only the *querier*.

### User interface

The querier's interface with the user includes two arguments on the command line only, and inputted queries from stdin.
```
querier pageDirectory indexFilename
```
For example, if `letters` is a pageDirectory in `../data`,

``` bash
$ indexer ../data/letters ../data/letters.index
```

For the queries, the user can be expected to input queries where the following will cause non-fatal errors and prompt for another input:
- only words under 3 letters
- beginning with `and` or `or`
- ending with `and` or `or`
- having repeating `and`s or `or`s
- special characters

For example, a valid query might be: "dog and cat or fish and rock"

### Inputs and outputs

**Input**: the querier reads files from a directory by constructing file pathnames from the `pageDirectory` parameter followed by a numeric document ID (as described in the Requirements).

The querier reads document files in sequential ID order, beginning at 1, until is unable to open one of those files.

The querier reads an indexed file and builds an index inverted data structure to use from scoring, provided by `indexer.c`.

The querier reads the user's provided query and splits it into managable subqueries, where a score can represent each subquery and then be comvined.

**Output**: We output the user's query and the most relevant URLS into stdout. For example:

```
Query: for or first
first for
Matches 6 documents (ranked):
score   2  doc 3: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
score   2  doc 5: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
score   1  doc 1: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
score   1  doc 2: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
score   1  doc 4: http://cs50tse.cs.dartmouth.edu/tse/letters/E.html
score   1  doc 6: http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
```

### Functional decomposition into modules

We anticipate the following modules or functions:
1. *main*, which parses arguments and initializes other modules, also running a loop to process stdin
2. *parseQuery*, which parses and validates the user's provided query
3. *calculateScores*, which calculates a resulting score for the query, where the score is a counter set of (docIDs, scores).
3. *printResults*, which sorts and prints out documents based on their relevancy score.

And some helper modules that provide data structures:
1. *word*, a module providing a function to normalize a word and tokenize a string of words
2. *index*, a module providing a function to load a new index hashtable from an indexed file

### Pseudocode for logic/algorithm flow

The querier will run as follows:
    parse the command line, validate parameters, initialize other modules
    request stdin query from the user
    call *parseQuery* with the query
    call *calculateScores* with the subqueries
    call *printResults*
where *parseQuery:*
        creates a new `bag` object
        tokenizes the query
        splits the query by `or` and EOF to construct subqueries
        adds the subqueries to the bag
        if successful,
            returns the bag to be scores
where *calculateScores:*
    steps through each docID for each subquery extracted from the bag
    scores how frequently the words of that subquery have been seen in the docID
    merges a subquerySet with a resultSet that is built over time
    finalizes a resultSet with a merged `counter` object that has every subquery's results at every docID visited
where *printResutls*
    sorts the documents by greatest to smallest score
    prints documents with non-zero scores in descending order
    
### Major data structures

A key data structure is the scoring system, where `sets` like the `resultSet` map *words* to *(docID, occurrences)*.
The *counters* is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 

Another key data structure is the subqueries, where a `bag` is constructed to hold the splitting of the query into subqueries.

### Testing Plan

1. A testing.sh file will be implemented to test various invalid arguments.
    2. no arguments
	3. one argument
	4. three or more arguments
	5. invalid `pageDirectory` (non-existent path)
	6. invalid `indexFile` (non-existent path)
	7. invalid queries from stdin as outlined above
8. The file will also test valid queries
9. Run *valgrind* to check for memory leaks or errors
10. use `fuzzquery.c` to test random output into the file

    



