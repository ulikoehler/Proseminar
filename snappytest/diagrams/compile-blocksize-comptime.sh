#!/bin/sh
for i in ../rawdata/snappy.comptime.* ; do
	echo "Executing R for "$i
	Rscript blksize-comptime.r $(echo $i | cut -d'/' -f3 | cut -d. -f3)
done
