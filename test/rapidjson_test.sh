#!/bin/sh

searchdir=$1

for entry in $searchdir/*

do
        # echo "$entry"

	filename="$entry"
	
	./rapidjson_ex $filename

done
