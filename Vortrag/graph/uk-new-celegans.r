  df1 <- as.data.frame(read.csv(file="../../rawdata/ebwt.stats-chrIII.fa.txt",head=TRUE,sep=","))
  
  dfOriginal <- df1
  
  countCharOccurrences <- function(char, s) {
    s2 <- gsub(char,"",s)
    return (nchar(s2) - nchar(s))
  }
  
  df1 <- df1[df1$Algorithm!="BWT+MTF",]
  #df1 <- df1[df1$Algorithm!="BWT+MTF",]
  df1 <- df1[df1$Algorithm!="MTF",]
  df1 <- df1[df1$Algorithm!="MTF+Snappy",]
  df1 <- df1[df1$Algorithm!="MTF+LZO",]
  df1 <- df1[df1$Algorithm!="MTF+LZ4",]
  df1 <- df1[df1$Algorithm!="MTF+Huffman",]
  
  df1 <- df1[df1$Algorithm!="RLE",]
  df1 <- df1[df1$Algorithm!="RLE+Snappy",]
  df1 <- df1[df1$Algorithm!="RLE+LZO",]
  df1 <- df1[df1$Algorithm!="RLE+LZ4",]
  
  df1 <- df1[df1$Algorithm!="RLE+Huffman",]
  
  df1 <- df1[df1$Algorithm!="BWT+RLE+Snappy",]
  df1 <- df1[df1$Algorithm!="BWT+RLE+LZO",]
  df1 <- df1[df1$Algorithm!="BWT+RLE+LZ4",]
  
  df1 <- df1[df1$Algorithm!="BWT+MTF+Snappy",]
  df1 <- df1[df1$Algorithm!="BWT+MTF+LZO",]
  df1 <- df1[df1$Algorithm!="BWT+MTF+LZ4",]
  
  df1 <- df1[df1$Algorithm!="BWT+LZO",]
  df1 <- df1[df1$Algorithm!="BWT+LZ4",]
  df1 <- df1[df1$Algorithm!="BWT+Snappy",]
  
  df1 <- df1[df1$Algorithm!="RLE",]
  df1 <- df1[df1$Group!="EncodingOnly",]
  df1 <- df1[df1$Algorithm!="LZO",]
  df1 <- df1[df1$Algorithm!="LZ4",]
  df1 <- df1[df1$Algorithm!="Huffman",]
  library(ggplot2)
  
  pdf(file="uk-new-celegans.pdf",height=5,width=7)
  
  print(ggplot(data=df1, aes(x=Blocksize, y=Size, group=Algorithm, colour=Algorithm)) +
      geom_line() + # Thinner lines
      scale_fill_hue(name="Algorithmus") +      # Set legend title
      xlab("Blockgröße") + ylab("Dateigröße") + # Set axis labels
      ggtitle("Kompression des Chromosoms III aus C. Elegans (FASTA)") +  # Set title
      theme())
  
  dev.off();