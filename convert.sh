#!/bin/sh

if [ $# -ne 2 ]
then
	echo "Too few arguments!"
	exit 1
fi

stronghold_dir=$1
asset_dir=$2
gm_dir="$stronghold_dir/gm"

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
	file=`basename $i .gm1`
	echo "Convert: ${file}"
	bin/sh2ck --header $i "$asset_dir/${file}"
done

