df1 <- read.table(header=T, con <- textConnection('
Index firstStage secondStage Time
  0 Keine bzip2 905
  1 Keine PPMd(default) 324
  2 Keine PPMd(large) 5155
  3 Keine PPMd-mx9 17974
  4 BWT bzip2 4338
  5 BWT PPMd(default) 3873
  6 BWT PPMd(large) 8473
  7 BWT PPMd-mx9 20229
  8 BWT-SAP bzip2 4121
  9 BWT-SAP PPMd(default) 3867
  10 BWT-SAP PPMd(large) 6636
  11 BWT-SAP PPMd-mx9 14724
'))
close(con)

#Original size
df1 <- df1[df1$secondStage!="PPMd-mx9",]
df1 <- df1[df1$secondStage!="PPMd(large)",]
df1 <- df1[df1$secondStage!="bzip2",]

df1 <- transform(df1, firstStage = reorder(firstStage, Index))
library(ggplot2)

pdf(file="cb-2.pdf",height=5,width=7)

ggplot(data=df1, aes(x=firstStage, y=Time, fill=secondStage)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Kompressionsstufe") +      # Set legend title
    xlab("Codierungsstufe") + ylab("Kumulative Kompressionsdauer in Sekunden") + # Set axis labels
    ggtitle("Kompressionsdauer nach Cox und Bauer") +  # Set title
    theme()
    