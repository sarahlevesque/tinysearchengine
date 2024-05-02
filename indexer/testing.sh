#!/bin/bash

# zero arguments
./indexer/indexer


# invalid arguments
./indexer/indexer NULL indexer/test


./indexer/indexer crawler/letters-0 NULL


# too many arguments
./indexer/indexer crawler/letters-0 indexer/test SARAH CS50


# not a crawled directory
./indexer/indexer crawler/notCrawled indexer/test


# invalid indexFile (does not exist)
./indexer/indexer crawler/letters-3 indexer/CS50


# comparing indexer output with cs50-dev output, should print nothing
./indexer/indexcmp indexer/toscrape-1 /thayerfs/home/f006fz1/cs50-dev/shared/tse/indices/toscrape-1.ndx


# indexing wikipedia_2 from cs50-dev
./indexer/indexer crawler/wikipedia-2 indexer/wikipedia-2


# comparing indexer's output with cs50dev indices
./indexer/indexcmp indexer/wikipedia-2 /thayerfs/home/f006fz1/cs50-dev/shared/tse/indices/wikipedia_2.ndx


# running a test comparison with indexertest
./indexer/indexer crawler/letters-3 indexer/test
./indexer/indexertest indexer/test indexer/testcomp
./indexer/indexcmp indexer/test indexer/testcomp # nothing should be printed following this
