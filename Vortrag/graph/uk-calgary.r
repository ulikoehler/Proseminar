df1 <- read.table(header=T, con <- textConnection('
Index Algorithm Pre Ratio
  0 compress Keine 41.2519463
  1 compress BWT(500) 58.2135642
  2 compress BWT(10k) 49.6874882
  3 gzip Keine 40.7632442
  4 gzip BWT(500) 58.7500049
  5 gzip BWT(10k) 50.6581284
  6 bzip2 Keine 30.2558239
  7 bzip2 BWT(500) 56.0494608
  8 bzip2 BWT(10k) 47.1312003
  9 xz Keine 33.9653811
  10 xz BWT(500) 55.2325725
  11 xz BWT(10k) 45.6869471
  12 "xz -e9" Keine 33.9992013
  13 "xz -e9" BWT(500) 55.251824
  14 "xz -e9" BWT(10k) 45.618266
'))
close(con)

#Original size

df1 <- transform(df1, Algorithm = reorder(Algorithm, Index))
library(ggplot2)

pdf(file="uk-2.pdf",height=5,width=7)

ggplot(data=df1, aes(x=Algorithm, y=Ratio, fill=Pre)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Erste Codierungsstufe") +      # Set legend title
    xlab("Algorithmus") + ylab("Relative Größe (%)") + # Set axis labels
    ggtitle("Kompression des Calgary Corpus, book1 (Fiction book)") +  # Set title
    theme()
    