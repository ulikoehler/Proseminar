df1 <- as.data.frame(read.csv(file=commandArgs()[6],head=TRUE,sep=","))

library(ggplot2)

png(file=paste("Snappy-Blocksize-Compression-",commandArgs()[7], ".png", sep=""),height=400, width=800)

ggplot(data=df1, aes(x=Blocksize, y=RelSize)) +
    geom_line() + # Thinner lines
    xlab("Block size") + ylab("Relative filesize (compressed)") + # Set axis labels
    ggtitle(paste("Snappy - Blocksize vs. relative filesize - ", commandArgs()[7], " - Calgary corpus", sep="")) +  # Set title
    theme()
    