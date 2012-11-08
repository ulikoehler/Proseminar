compression <- as.data.frame(read.csv(file=paste("../rawdata/snappy.compression.",commandArgs()[6],".txt",sep=""),head=TRUE,sep=","))
comptime <- as.data.frame(read.csv(file=paste("../rawdata/snappy.comptime.",commandArgs()[6],".txt",sep=""),head=TRUE,sep=","))
decomptime <- as.data.frame(read.csv(file=paste("../rawdata/snappy.decomptime.",commandArgs()[6],".txt",sep=""),head=TRUE,sep=","))

combined <- merge(compression, comptime, by=c("Blocksize"))
combined <- merge(combined, decomptime, by=c("decomptime"))

library(ggplot2)

#png(file=paste("Snappy-Blocksize-Compression-",commandArgs()[6], ".png", sep=""),height=400, width=800)
pdf(file=paste("blocksize-compression-diagrams/Snappy-Blocksize-Compression-",commandArgs()[6], ".pdf", sep=""),height=5, width=7)

ggplot(data=df1, aes(x=Blocksize, y=RelSize, group=Algorithm, color=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Block size") + ylab("Relative filesize (compressed)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[6], sep="")) +  # Set title
    theme()
