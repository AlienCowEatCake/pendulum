#!/bin/bash

names="screen1-macosx-en screen1-macosx-ru screen2-macosx-en screen2-macosx-ru"

for name in ${names} ; do
	inkscape -z -e ${name}.png ${name}.svg
	optipng -o7 -strip all ${name}.png
done

