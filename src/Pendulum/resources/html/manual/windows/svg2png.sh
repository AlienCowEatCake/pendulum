#!/bin/bash

names="screen1-windows-en screen1-windows-ru screen2-windows-en screen2-windows-ru"

for name in ${names} ; do
	inkscape -z -e ${name}.png ${name}.svg
	optipng -o7 -strip all ${name}.png
done

