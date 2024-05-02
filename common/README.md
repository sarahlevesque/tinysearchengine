# CS50 Lab 4
## CS50 Spring 2024

### common

Common encapsulates all the knowledge about how to initialize and validate a `pageDirectory`, and how to write and read page files, in one place... anticipating future use by the Indexer and Querier.

### Usage

Common contains functions, defined in `pagedir.h` and implemented in `pagedir.c`, that construct pathnames, open files to write into, and write webpage data into said files.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
```

### Implementation

Given user provided arguments, the functions of `pagedir.c` are used to construct a valid pathname for crawled files and opens them for writing after crawling. 

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `pagedir.h` - the interface
* `pagedir.c` - the implementation

### Compilation

To compile, simply `make`
