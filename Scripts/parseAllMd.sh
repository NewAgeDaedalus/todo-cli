#!/bin/sh

files=$(find -name "*.md")
for file in $files; do 
        md2html $file -o ${file%.md}.html
done;
