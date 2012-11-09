#sourceFileName <- commandArgs()[6]
library(ggplot2,quietly=TRUE)

fileList <- dir("../rawdata",pattern="snappy\\.compression\\..*.txt")

createCombinedSizeDiagrams <- function(compression, comptime, decomptime, sourceFileName) {
	names(decomptime)[names(decomptime)=="ms"] <- "Decomptime"
	names(comptime)[names(comptime)=="ms"] <- "Comptime"

	combined <- merge(compression, comptime)
	combined <- merge(combined, decomptime)

	#Calculate how much size is gained per ms of comp/decomp time
	combined["SizePerCompTime"] <- NA;
	combined["SizePerDecompTime"] <- NA;
	combined$SizePerCompTime <- combined$RelSize / combined$Comptime
	combined$SizePerDecompTime <- combined$RelSize / combined$Decomptime

  pdf(file=paste("combined-diagrams/Snappy-SizePerComptime-",sourceFileName, ".pdf", sep=""),height=5, width=7)

	ggplot(data=combined, aes(x=Blocksize, y=SizePerCompTime, group=Algorithm, color=Algorithm)) +
	    geom_line() + # Thinner lines
	    scale_fill_hue(name="Algorithm") +      # Set legend title
	    xlab("Block size") + ylab("% Size gain per ms compression time") + # Set axis labels
	    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", sourceFileName, sep="")) +  # Set title
	    theme()
	    
	pdf(file=paste("combined-diagrams/Snappy-SizePerDecomptime-",sourceFileName, ".pdf", sep=""),height=5, width=7)

	ggplot(data=combined, aes(x=Blocksize, y=SizePerDecompTime, group=Algorithm, color=Algorithm)) +
	    geom_line() + # Thinner lines
	    scale_fill_hue(name="Algorithm") +      # Set legend title
	    xlab("Block size") + ylab("% Size gain per ms decompression time") + # Set axis labels
	    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", sourceFileName, sep="")) +  # Set title
	    theme()
}

for(i in 1:length(fileList)) {
  sourceFileName <- strsplit(fileList[i],"\\.")[[1]][3];
  print(paste("Processing ",sourceFileName));	#Rename columns
  
  compression <- read.csv(file=paste("../rawdata/snappy.compression.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  comptime <- read.csv(file=paste("../rawdata/snappy.comptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  decomptime <- read.csv(file=paste("../rawdata/snappy.decomptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  
  createDiagrams(compression, comptime, decomptime, sourceFileName); 
}