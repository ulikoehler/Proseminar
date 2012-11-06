df1 <- as.data.frame(read.csv(file="../../rawdata/snappy.stats.book1.txt",head=TRUE,sep=","))

library(ggplot2)

pdf(file="snappy-blocksize-compression.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Blocksize, y=RelSize)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Block size") + ylab("Relative filesize (compressed)") + # Set axis labels
    ggtitle("Snappy - Blocksize vs. relative filesize") +  # Set title
    theme()
    