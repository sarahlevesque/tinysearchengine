#!/bin/bash

# zero arguments
./crawler

# misused arguments
# NULL URL
./crawler NULL crawled 1

# .com is not a valid URL, can't be normalized
./crawler http://cs50tse.cs.dartmouth.com crawled 1

#invalid url, do nothing
./crawler http://cs50tse.cs.dartmouth.edu/tse/SARAHLEVESQUE crawled 1 

# Not a valid directory
./crawler http://cs50tse.cs.dartmouth.edu/tse/ nullDirectory 1

# depth is out of range
./crawler http://cs50tse.cs.dartmouth.edu/tse/ crawled -10

# depth is out of range
./crawler http://cs50tse.cs.dartmouth.edu/tse/ crawled 1000

# depth is invalid
./crawler http://cs50tse.cs.dartmouth.edu/tse/ crawled 7a


# valid usages
# crawling letters to 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html letters-0 0
count=$(ls -1 "letters-0" | wc -l)
solutioncount=$(ls -1 "/thayerfs/home/f006fz1/cs50-dev/shared/tse/output/letters-0/" | wc -l)

echo "The number of items in the crawled directory is: $count"
echo "The number of items in the cs50/shared/tse/output directory is: $solutioncount"

# crawling letters to 3 
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html letters-3 3
count=$(ls -1 "letters-3" | wc -l)
solutioncount=$(ls -1 "/thayerfs/home/f006fz1/cs50-dev/shared/tse/output/letters-3/" | wc -l)

echo "The number of items in the crawled directory is: $count"
echo "The number of items in the cs50/shared/tse/output directory is: $solutioncount"

# crawling toscrape to 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html toscrape-1 1
count=$(ls -1 "toscrape-1" | wc -l)
solutioncount=$(ls -1 "/thayerfs/home/f006fz1/cs50-dev/shared/tse/output/toscrape-1/" | wc -l)

echo "The number of items in the crawled directory is: $count"
echo "The number of items in the cs50/shared/tse/output directory is: $solutioncount"
