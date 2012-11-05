df1 <- as.data.frame(read.csv(file="../../rawdata/ebwt.statistics.chrIII.fa.txt",head=TRUE,sep=","))

df1 <- df1[df1$Algorithm!="BWT+MTF",]
df1 <- df1[df1$Algorithm!="BWT+MTF",]
df1 <- df1[df1$Algorithm!="MTF",]

library(ggplot2)

pdf(file="uk-new-celegans.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Blocksize, y=Size, group=Algorithm, colour=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Blockgröße") + ylab("Dateigröße") + # Set axis labels
    ggtitle("Kompression des Chromosoms III aus C. Elegans (FASTA)") +  # Set title
    theme()
    