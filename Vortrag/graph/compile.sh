#!/bin/sh
for i in *.r ; do echo "Compiling $i..." ; Rscript $i ; done
