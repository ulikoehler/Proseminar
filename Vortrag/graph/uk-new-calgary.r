df1 <- as.data.frame(read.csv(file="../../rawdata/ebwt.statistics.book1.txt",head=TRUE,sep=","))

#Original size

#df1 <- transform(df1, Algorithm = reorder(BTW+MTF+Huffman, Index))
library(ggplot2)

pdf(file="uk-new-1.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Blocksize, y=Size, group=Algorithm)) +
    #geom_hline(yintercept=4767686) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Blockgröße") + ylab("Größe") + # Set axis labels
    ggtitle("Kompression des C. Elegans Chromosoms III (FASTA)") +  # Set title
    theme()
    