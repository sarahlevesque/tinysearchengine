# CS50 Lab 4
## CS50 Spring 2024

### crawler

Crawler anticipates receiving user input, specifically a valid `seedURL`, a valid and existing `pageDirectory`, and a `depth` to crawl to between 0-10. The program crawls through the `seedURL` to the `depth` provided, and saves the html of each visited page in the `pageDirectory`.

### Usage

The user provides a valid `seedURL`, a valid and existing `pageDirectory`, and a `depth` to crawl to between 0-10. Crawler guarantees if these parameters are all exisiting and valid, it will output html files into the provided `pageDirectory`.

The _parseArgs_ function ensures the `seedURL` is normalized and internal, and initializes other variables.

The _crawl_ function Implements a page crawler to move through the webpages in a webpage. The caller provides: a valid url, a valid pageDirectory, and a valid maxDepth. We guarantee that: the given page will be scanned; any links will be extracted; the scanner will determine if the URL has been seen before.

The _pageScan_ function scans the current webpage and extracts internal links if they have not yet been seen.

```c
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void deleteitem(void* item);
```

### Implementation

Given user provided arguments, `crawler.c` will crawl the provided webpage and output html files it crawls from that source.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure

### Compilation

To compile, simply `make`
