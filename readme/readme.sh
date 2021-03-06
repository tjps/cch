#!/bin/bash
# Script for generating the README.md from the template.

echo "Generating README.md"

cp readme/README.md.tmpl README.md

build/cch --noBanner --noLineNumbers --input readme/readme.cch --output ./%f

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to parse readme.cch"
    exit 1
fi

sed -i -e '/{{README_CCH}}/{r readme/readme.cch' -e 'd}' README.md
sed -i -e '/{{README_CC}}/{r readme.cch.cc' -e 'd}' README.md
sed -i -e '/{{README_H}}/{r readme.cch.h' -e 'd}' README.md

rm readme.cch.{h,cc}
