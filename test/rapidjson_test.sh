#!/bin/sh

searchdir=/home/aiadmin/temp/err

for entry in $searchdir/*

do
        # echo "$entry"

	filename="$entry"
	
	./rapidjson_ex $filename

done
