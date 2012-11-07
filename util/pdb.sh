#!/bin/sh
# This script downloads 200 random PDB files for the bioinformatics compression corpus
wget -qO- www.rcsb.org/pdb/rest/getCurrent | grep structureId | cut -d\" -f2 > pdbids
for i in $(cat pdbids | sort -R | head -n 200) ; do echo ${i} && wget http://www.rcsb.org/pdb/files/${i}.pdb.gz ; done
tar cvf Bioinf-200pdb *.pdb
