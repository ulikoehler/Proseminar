    df1 <- as.data.frame(read.csv(file="../../rawdata/ebwt.stats-chrIII.fa.txt",head=TRUE,sep=","))
    
    dfdfOriginal <- df1
    
    df1 <- df1[df1$Algorithm=="BWT+MTF+Huffman" | df1$Algorithm == "Deflate" | df1$Algorithm == "Snappy",]
    
    library(ggplot2)
    
    pdf(file="uk-time.pdf",height=5,width=7)
    
    print(ggplot(data=df1, aes(x=Blocksize, y=Time, group=Algorithm, colour=Algorithm)) +
        geom_line() + # Thinner lines
        scale_fill_hue(name="Algorithmus") +      # Set legend title
        xlab("Blockgröße") + ylab("Kompressionszeit (ms)") + # Set axis labels
        ggtitle("Kompression des Chromosoms III aus C. Elegans (FASTA)") +  # Set title
        theme())
