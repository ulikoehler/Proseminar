df1 <- as.data.frame(read.csv(file=paste("../rawdata/snappy.decomptime.",commandArgs()[6],".txt",sep=""),head=TRUE,sep=","))

library(ggplot2)

#Debugging
#df1 <- df1[df1$Algorithm!="Snappy",]
#df1 <- df1[df1$Algorithm!="LZO1X11",]

#png(file=paste("Snappy-Blocksize-Decompressiontime-",commandArgs()[6], ".png", sep=""),height=400, width=800)
pdf(file=paste("blocksize-decomptime-diagrams/Snappy-Blocksize-Decompressiontime-",commandArgs()[6], ".pdf", sep=""),height=5, width=7)

ggplot(data=df1, aes(x=Blocksize, y=ms, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Block size") + ylab("Decompression time (ms)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()
    
