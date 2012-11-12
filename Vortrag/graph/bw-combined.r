df1 <- read.table(header=T, con <- textConnection('
Index Algorithm Type Ratio
  0 LZW Kompression/Größe 210338.542 #(3265536-1246286)/9.6
  1 LZW Dekompression/Größe 388317.308 # (3265536-1246286)/5.2
  2 Deflate Kompression/Größe  52597.3944 #(3265536-1024887)/42.6
  3 Deflate Dekompression/Größe 457275.306 #(3265536-1024887)/4.9
  4 BWT+Huffman Kompression/Größe 47148.7867 #(3265536-856233)/51.1
  5 BWT+Huffman Dekompression/Größe 256308.83 #(3265536-856233)/9.4
  6 comp-2 Kompression/Größe 4006.38428 #(3265536-848885)/603.2
  7 comp-2 Dekompression/Größe 3935.27276 #(3265536-848885)/614.1
'))
close(con)

df1 <- transform(df1, Algorithm = reorder(Algorithm, Index))
library(ggplot2)

pdf(file="bw-3.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Algorithm, y=Ratio, fill=Type)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Typ") +      # Set legend title
    xlab("Algorithmus") + ylab("Größeneinsparung in Bytes pro Sekunde") + # Set axis labels
    ggtitle("Größeneinsparung / Zeit verschiedener Algorithmen") +  # Set title
    theme()
    