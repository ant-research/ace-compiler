#!/bin/bash

REPO=nn
COMP="core driver onnx2air vector util"

for i in $COMP; do
  echo "$i HEADER"
  echo "====================================="
  cloc include/$REPO/$i
  echo "$i SOURCE"
  echo "====================================="
  cloc $i/src
  echo "$i UNITTEST"
  echo "====================================="
  cloc $i/unittest $i/test $i/example
  echo "$i TOTAL"
  echo "====================================="
  cloc include/$REPO/$i $i/src $i/unittest $i/test $i/example
done

