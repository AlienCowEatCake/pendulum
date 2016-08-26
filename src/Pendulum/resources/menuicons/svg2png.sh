#!/bin/bash

names="energy_black energy_white displacement_black displacement_white speed_black speed_white"

for name in ${names} ; do
	inkscape -z -e ${name}.png  -w 16 -h 16 ${name}.svg
	optipng -o7 -strip all ${name}.png
done

