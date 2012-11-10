df1 <- as.data.frame(read.csv(file="../../rawdata/ebwt.stats-book1.txt",head=TRUE,sep=","))

#delete <- c("")

df1 <- df1[df1$Algorithm!="BWT+MTF",]
#df1 <- df1[df1$Algorithm!="BWT+MTF",]
df1 <- df1[df1$Algorithm!="MTF",]
df1 <- df1[df1$Group!="EncodingOnly",]
df1 <- df1[df1$Group!="CompressionOnly",]

library(ggplot2,quiet=TRUE)

pdf(file="uk-new-calgary.pdf",height=5,width=7)

print(ggplot(data=df1, aes(x=Blocksize, y=Size, group=Algorithm, colour=Algorithm)) +
    geom_line() + # Thinner lines
    scale_fill_hue(name="Algorithmus") +      # Set legend title
    xlab("Blockgröße") + ylab("Dateigröße") + # Set axis labels
    ggtitle("Kompression book1 - Calgary Corpus (Fictional text)") +  # Set title
    theme())

dev.off();