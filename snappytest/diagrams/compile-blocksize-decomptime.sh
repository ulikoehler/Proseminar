#!/bin/sh
for i in ../rawdata/snappy.decomptime.* ; do
	echo "Executing R for "$i
	Rscript blksize-decomptime.r $(echo $i | cut -d'/' -f3 | cut -d. -f3)
done
