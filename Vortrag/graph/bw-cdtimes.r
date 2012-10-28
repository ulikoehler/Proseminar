df1 <- read.table(header=T, con <- textConnection('
Index Algorithm Type Time
  0 LZW Kompression 9.6 
  1 LZW Dekompression 5.2
  2 Deflate Kompression 42.6 
  3 Deflate Dekompression 4.9
  4 BWT+Huffman Kompression 51.1
  5 BWT+Huffman Dekompression 9.4
  6 comp-2 Kompression 603.2
  7 comp-2 Dekompression 614.1
'))
close(con)

df1 <- transform(df1, Algorithm = reorder(Algorithm, Index))
library(ggplot2)

pdf(file="bw-1.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Algorithm, y=Time, fill=Type)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Typ") +      # Set legend title
    xlab("Algorithmus") + ylab("Zeit(s)") + # Set axis labels
    ggtitle("Zeitaufwand verschiedener Algorithmen") +  # Set title
    theme()
    