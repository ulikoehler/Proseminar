createCompressionDiagram <- function(compression, sourceFileName) {
  plot <- ggplot(data=compression, aes(x=Blocksize, y=RelSize, group=Algorithm, color=Algorithm)) +
      geom_line() + # Thinner lines
      scale_fill_hue(name="Algorithm") +      # Set legend title
      xlab("Block size") + ylab("Relative filesize (compressed)") + # Set axis labels
      ggtitle(paste("Snappy - Blocksize vs. relative filesize - ",sourceFileName, sep="")) +  # Set title
      theme();
  return(plot);
}


createCompressionTimeDiagram <- function(comptime, sourceFileName) {
  plot1 <- ggplot(data=comptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. compression time - ",sourceFileName, sep="")) +  # Set title
    theme()
  
  #Results show that the LZO curves clutter the diagram (their relsize is too high)
  #Therefore we remove it to produce another non-lzo diagram here
  #Additionally remove LZ4HC, even if the filename doesn't suggest that
  comptime <- comptime[comptime$Algorithm!="LZO1X11",]
  comptime <- comptime[comptime$Algorithm!="LZO1X999",]
  comptime <- comptime[comptime$Algorithm!="LZ4HC",]

  plot2 <- ggplot(data=comptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. compression time - ", sourceFileName, sep="")) +  # Set title
    theme()
  
  return(list(plot1, plot2));
}

createDecompressionTimeDiagram <- function(decomptime, sourceFileName) {
  plot <- (ggplot(data=decomptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Decompression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. decompression time - ",sourceFileName, sep="")) +  # Set title
    theme());
  
  return(plot);
}