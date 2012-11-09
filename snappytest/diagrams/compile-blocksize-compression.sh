#!/bin/sh
for i in ../rawdata/snappy.compression.* ; do
	echo "Executing R for "$i
	Rscript blksize-compression.r $(echo $i | cut -d'/' -f3 | cut -d. -f3)
done
