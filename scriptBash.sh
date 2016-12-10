#! /usr/bin/env bash

make clean
make
rm -f ./out/outTable.txt
rm -f ./out/term.txt
rm -f ./plt/outFig.png

./bin/turbo ./out/outTable.txt #> ./out/term.txt
python3 ./readAndDraw.py
open ./plt/outFig.png
