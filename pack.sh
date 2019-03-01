#!/bin/sh

if [ $# -ne 2 ]
then
	echo "Too few arguments!"
	exit 1
fi

asset_dir=$1
packed_dir=$2

if [ ! -d "$packed_dir" ]
then
	mkdir "$packed_dir/"
fi

for i in $asset_dir/*
do
	name=`basename $i`
	echo "Pack: ${name}"
	./packer.py "${asset_dir}/${name}/data.json" "$packed_dir/${name}.data" "$packed_dir/${name}.png"
done

