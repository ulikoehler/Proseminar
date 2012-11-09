
createCompressionDiagram <- function(compression, sourceFileName) {
  pdf(file=paste("blocksize-compression-diagrams/Snappy-Blocksize-Compression-",commandArgs()[6], ".pdf", sep=""),height=5, width=7)
  
  ggplot(data=compression, aes(x=Blocksize, y=RelSize, group=Algorithm, color=Algorithm)) +
      geom_line() + # Thinner lines
      scale_fill_hue(name="Algorithm") +      # Set legend title
      xlab("Block size") + ylab("Relative filesize (compressed)") + # Set axis labels
      ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
      theme()
}


createCompressionTimeDiagram <- function(comptime, sourceFileName) {
  #png(file=paste("Snappy-Blocksize-Compressiontime-",commandArgs()[6], ".png", sep=""),height=400, width=800)
  pdf(file=paste("blocksize-comptime-diagrams/Snappy-Blocksize-Compressiontime-",commandArgs()[6], ".pdf", sep=""),height=5, width=7)
  
  ggplot(data=comptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()
  
  #Results show that the LZO curves clutter the diagram (their relsize is too high)
  #Therefore we remove it to produce another non-lzo diagram here
  #Additionally remove LZ4HC, even if the filename doesn't suggest that
  comptime <- comptime[comptime$Algorithm!="LZO1X11",]
  comptime <- comptime[comptime$Algorithm!="LZO1X999",]
  comptime <- comptime[comptime$Algorithm!="LZ4HC",]
  pdf(file=paste("blocksize-comptime-diagrams/Snappy-Blocksize-Compressiontime-",sourceFileName, "-nolzo.pdf", sep=""),height=5, width=7)
  
  ggplot(data=comptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", sourceFileName, sep="")) +  # Set title
    theme()
}

createDecompressionTimeDiagram <- function(decomptime, sourceFileName) {
  pdf(file=paste("blocksize-decomptime-diagrams/Snappy-Blocksize-Decompressiontime-",sourceFileName, ".pdf", sep=""),height=5, width=7)
  
  ggplot(data=decomptime, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithm") +      # Set legend title
    xlab("Block size") + ylab("Decompression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()
}