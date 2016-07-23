#!/bin/bash

names="screen1-linux-en screen1-linux-ru screen2-linux-en screen2-linux-ru"

for name in ${names} ; do
	inkscape -z -e ${name}.png ${name}.svg
	optipng -o7 -strip all ${name}.png
done

