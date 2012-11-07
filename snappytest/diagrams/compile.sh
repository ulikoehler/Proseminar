#!/bin/sh
for i in ../rawdata/snappy.compression.* ; do
	Rscript blksize-compression.r $(echo $i | cut -d'/' -f3 | cut -d. -f3)
done
