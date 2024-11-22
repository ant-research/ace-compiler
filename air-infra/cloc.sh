#!/bin/bash

for i in base core driver opt util; do
  echo "$i HEADER"
  echo "====================================="
  cloc include/air/$i
  echo "$i SOURCE"
  echo "====================================="
  cloc $i/src
  echo "$i UNITTEST"
  echo "====================================="
  cloc $i/unittest $i/test $i/example
  echo "$i TOTAL"
  echo "====================================="
  cloc include/air/$i $i/src $i/unittest $i/test $i/example
done

