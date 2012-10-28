df1 <- read.table(header=T, con <- textConnection('
Index Algorithm Size
  0 LZW 1246286
  1 Deflate 1024887
  2 BWT+Huffman 856233
  3 comp-2 848885
'))
close(con)
df1 <- transform(df1, Algorithm = reorder(Algorithm, Index))
library(ggplot2)
pdf(file="bw-2.pdf",height=5,width=7)
ggplot(data=df1, aes(x=Algorithm, y=Size,fill=Algorithm)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    xlab("Algorithmus") + ylab("Größe nach Kompression in Bytes (Calgary Corpus)") + # Set axis labels
    ggtitle("") +  # Set title
    theme(  plot.background =   element_rect(colour = NA), legend.position="none") 
