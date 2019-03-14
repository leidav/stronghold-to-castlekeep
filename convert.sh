#!/bin/sh

if [ $# -lt 2 ]
then
	echo "Too few arguments!"
	exit 1
fi

stronghold_dir=$1
asset_dir=$2
gm_dir="$stronghold_dir/gm"

if [ -n "$3" ]
then
pack=$3
else
pack="pack"
fi

if [ ! -e bin/sh2ck ]
then
	echo "You have to compile sh2ck first!"
	exit 1
fi

if [ ! -d $asset_dir ]
then
	mkdir $asset_dir 
fi

for i in $gm_dir/*.gm1 
do
	file=`basename "${i}" .gm1`
	echo "Convert: ${file}"
	if [ "${pack}" = "pack" ]; then
		if [ "${file}" = "tile_land_macros" ]; then
			bin/sh2ck --pack "$i" "$asset_dir/" "${file}"
		else
			bin/sh2ck --assemble --sort --pack "$i" "$asset_dir/" "${file}"
		fi
	else
		bin/sh2ck --assemble  "$i" "$asset_dir/$file" "${file}"
	fi
done

