
createCombinedSizeDiagrams <- function(compression, comptime, decomptime, sourceFileName) {
  names(decomptime)[names(decomptime)=="ms"] <- "Decomptime"
  names(comptime)[names(comptime)=="ms"] <- "Comptime"
  
  combined <- merge(compression, comptime)
  combined <- merge(combined, decomptime)
  
  #Calculate how much size is gained per ms of comp/decomp time
  #Initialize the columns
  combined["SizePerCompTime"] <- NA;
  combined["SizePerDecompTime"] <- NA;
  #Calculate the values
  combined$SizePerCompTime <- combined$RelSize / combined$Comptime
  combined$SizePerDecompTime <- combined$RelSize / combined$Decomptime
  
  plot1 <- ggplot(data=combined, aes(x=Blocksize, y=SizePerCompTime, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("% Size gain per ms compression time") + # Set axis labels
    ggtitle(paste("Size gain per compression time - ", sourceFileName, sep="")) +  # Set title
    theme()
  
  plot2 <- (ggplot(data=combined, aes(x=Blocksize, y=SizePerDecompTime, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("% Size gain per ms decompression time") + # Set axis labels
    ggtitle(paste("Size gain per decompression time - ", sourceFileName, sep="")) +  # Set title
    theme())
  
  return(list(plot1, plot2));
}