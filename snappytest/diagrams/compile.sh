#!/bin/sh
for i in ../rawdata/* ; do
	Rscript blksize-compression.r $i $(echo $i | cut -d'/' -f3 | cut -d. -f3)
done
