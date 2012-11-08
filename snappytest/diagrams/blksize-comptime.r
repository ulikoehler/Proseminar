df1 <- as.data.frame(read.csv(file=paste("../rawdata/snappy.comptime.",commandArgs()[6],".txt",sep=""),head=TRUE,sep=","))

library(ggplot2)

#png(file=paste("Snappy-Blocksize-Compressiontime-",commandArgs()[6], ".png", sep=""),height=400, width=800)
pdf(file=paste("blocksize-comptime-diagrams/Snappy-Blocksize-Compressiontime-",commandArgs()[6], ".pdf", sep=""),height=5, width=7)

ggplot(data=df1, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()
    
#Results show that the LZO curves clutter the diagram
#Therefore we remove it to produce another non-lzo diagram here
#Additionally remove LZ4HC, even if the filename doesn't suggest that
df1 <- df1[df1$Algorithm!="LZO1X11",]
df1 <- df1[df1$Algorithm!="LZO1X999",]
df1 <- df1[df1$Algorithm!="LZ4HC",]
pdf(file=paste("blocksize-comptime-diagrams/Snappy-Blocksize-Compressiontime-",commandArgs()[6], "-nolzo.pdf", sep=""),height=5, width=7)

ggplot(data=df1, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Block size") + ylab("Compression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()