
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