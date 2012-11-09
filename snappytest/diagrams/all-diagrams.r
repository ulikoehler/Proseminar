#sourceFileName <- commandArgs()[6]
library(ggplot2,quietly=TRUE)

source("combined-time-size-diagram.r")

fileList <- dir("../rawdata",pattern="snappy\\.compression\\..*.txt")


for(i in 1:length(fileList)) {
  sourceFileName <- strsplit(fileList[i],"\\.")[[1]][3];
  print(paste("Processing ",sourceFileName));	#Rename columns
  
  compression <- read.csv(file=paste("../rawdata/snappy.compression.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  comptime <- read.csv(file=paste("../rawdata/snappy.comptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  decomptime <- read.csv(file=paste("../rawdata/snappy.decomptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  
  createDiagrams(compression, comptime, decomptime, sourceFileName); 
}