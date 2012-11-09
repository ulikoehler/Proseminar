library(ggplot2,quietly=TRUE)

source("combined-time-size-diagram.r")
source("simple-diagrams.r")
source("multiplot.r")

fileList <- dir("../rawdata",pattern="snappy\\.compression\\..*.txt")


for(i in 1:length(fileList)) {
  sourceFileName <- strsplit(fileList[i],"\\.")[[1]][3];
  cat("Processing ",sourceFileName,"\n");	#Rename columns
  
  compression <- read.csv(file=paste("../rawdata/snappy.compression.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  comptime <- read.csv(file=paste("../rawdata/snappy.comptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  decomptime <- read.csv(file=paste("../rawdata/snappy.decomptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  
   #Create the plots, not neccessarily 1 plot per variable
  plot1 <- createCompressionDiagram(compression, sourceFileName);
  plot2 <- createCompressionTimeDiagram(comptime, sourceFileName);
  plot3 <- createDecompressionTimeDiagram(decomptime, sourceFileName);
  plot4 <- createCombinedSizeDiagrams(compression, comptime, decomptime, sourceFileName);
  #Setup the PDF device
  pdf(file=paste("pdf/Benchmark-",sourceFileName, ".pdf", sep=""), height=5, width=7)
  #Write all plots
  multiplot(c(plot1, plot2, plot3, plot4), cols=2)
  #Close the device
  dev.off();
}