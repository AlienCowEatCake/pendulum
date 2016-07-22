#!/bin/bash

# generate <input> <output> <size>
function generate
{
	input="$1"
	output="$2"
	size="$3"
	inkscape -z -e "${output}" -w ${size} -h ${size} "${input}"
	optipng -o7 -strip all "${output}"
}

svgfile="ball.svg"
pngdir="ball_icons"
sizes="16 32 48 64 128 256 512 1024"
rm -rf "${pngdir}"
mkdir -p "${pngdir}"
for size in ${sizes} ; do
	generate "${svgfile}" "${pngdir}/icon_${size}.png" ${size}
done

