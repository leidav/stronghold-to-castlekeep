#!/bin/sh

if [ $# -ne 2 ]
then
	echo "Too few arguments!"
	exit 1
fi

stronghold_dir=$1
asset_dir=$2
gm_dir="$stronghold_dir/gm"
gfx_dir="$stronghold_dir/gfx"
gfx8_dir="$stronghold_dir/gfx8"

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
	echo "Convert: `basename $i .gm1`"
	bin/sh2ck $i "$asset_dir/`basename $i .gm1`"
done

for i in $gfx_dir/*.tgx $gfx8_dir/*.tgx 
do
	echo "Convert: `basename $i .tgx`"
	bin/sh2ck -t $i "$asset_dir/`basename $i .tgx`"
done
