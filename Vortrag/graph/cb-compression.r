df1 <- read.table(header=T, con <- textConnection('
Index firstStage secondStage Compression
  0 Keine bzip2 2.25
  1 Keine PPMd(default) 2.04
  2 Keine PPMd(large) 2.00
  3 Keine PPMd-mx9 1.98
  4 BWT bzip2 2.09
  5 BWT PPMd(default) 1.93
  6 BWT PPMd(large) 2.05
  7 BWT PPMd-mx9 2.09
  8 BWT-SAP bzip2 1.40
  9 BWT-SAP PPMd(default) 1.21
  10 BWT-SAP PPMd(large) 1.28
  11 BWT-SAP PPMd-mx9 1.34
'))
close(con)

#Original size

df1 <- transform(df1, firstStage = reorder(firstStage, Index))
library(ggplot2)

pdf(file="cb-1.pdf",height=5,width=7)

ggplot(data=df1, aes(x=firstStage, y=Compression, fill=secondStage)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Kompressionsstufe") +      # Set legend title
    xlab("Codierungsstufe") + ylab("Bits pro Base") + # Set axis labels
    ggtitle("Kompression nach Cox und Bauer") +  # Set title
    theme()
    