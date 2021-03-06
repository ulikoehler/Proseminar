df1 <- read.table(header=T, con <- textConnection('
Index Algorithm Pre Ratio
  0 compress Keine 28.1499642
  1 compress BWT(500) 28.9788058
  2 compress BWT(10k) 28.7353669
  3 gzip Keine 30.8905185
  4 gzip BWT(500) 32.5494108
  5 gzip BWT(10k) 32.0453275
  6 bzip2 Keine 27.9745582
  7 bzip2 BWT(500) 29.7050468
  8 bzip2 BWT(10k) 29.1685794
  9 xz Keine 26.0320404
  10 xz BWT(500) 28.6216848
  11 xz BWT(10k) 28.1650787
  12 "xz -e9" Keine 25.9844138
  13 "xz -e9" BWT(500) 28.609707
  14 "xz -e9" BWT(10k) 28.1459882
'))
close(con)

#Remove results that clutter the diagram too much
df1 <- df1[df1$Algorithm!="bzip2",]
df1 <- df1[df1$Algorithm!="xz",]
df1 <- df1[df1$Algorithm!="xz -e9",]
df1 <- df1[df1$Algorithm!="compress",]


df1 <- transform(df1, Algorithm = reorder(Algorithm, Index))
library(ggplot2)

pdf(file="uk-1.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Algorithm, y=Ratio, fill=Pre)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Erste Codierungsstufe") +      # Set legend title
    xlab("Algorithmus") + ylab("Relative Größe (%)") + # Set axis labels
    ggtitle("Kompression des C. Elegans Chromosoms III (FASTA)") +  # Set title
    theme()
    