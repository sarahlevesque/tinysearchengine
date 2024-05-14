#!/bin/bash

echo "Testing invalid queries:"

# test cases provided in requirements spec
./querier letters-3-c letters-3 <<< "and"

./querier letters-3-c letters-3 <<< "or"

./querier letters-3-c letters-3 <<< "and earth"

./querier letters-3-c letters-3 <<< "or earth"

./querier letters-3-c letters-3 <<< "planet earth or"

./querier letters-3-c letters-3 <<< "planet earTh and"

./querier letters-3-c letters-3 <<< "planet Earth and or sciEnce"

./querier letters-3-c letters-3 <<< "plAnet eaRth and and scIenCe"

./querier letters-3-c letters-3 <<< "planet earth or and science"

./querier letters-3-c letters-3 <<< "!"

./querier letters-3-c letters-3 <<< "("


echo "Simple tests to demonstrate functionality"

./querier toscrape-1 toscrape-1.ndx <<< "moon and back"
echo "should be 0"

./querier toscrape-1 toscrape-1.ndx <<< "moon"
echo "should be 3 (5, 13, 72)"

./querier toscrape-1 toscrape-1.ndx <<< "page and her"
echo "should be 2 (2, 66)"

./querier toscrape-1 toscrape-1.ndx <<< "page or this and had"
echo "should be 18 docs"


./querier toscrape-1 toscrape-1.ndx <<< "$(./fuzzquery toscrape-1.ndx 1 3)"

./querier toscrape-1 toscrape-1.ndx <<< "$(./fuzzquery toscrape-1.ndx 1 5)"

./querier toscrape-1 toscrape-1.ndx <<< "$(./fuzzquery toscrape-1.ndx 2 3)"


