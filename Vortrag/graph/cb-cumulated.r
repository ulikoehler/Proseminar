df1 <- read.table(header=T, con <- textConnection('
Index firstStage secondStage Compression
  0 Keine bzip2 0.00635359116#(8-2.25)/905
  1 Keine PPMd(default) 0.0183950617#(8-2.04)/324
  2 Keine PPMd(large) 0.00116391853#(8-2.00)/5155
  3 Keine PPMd-mx9 0.0003349282#(8-1.98)/17974
  4 BWT bzip2 0.00136237898#(8-2.09)/4338
  5 BWT PPMd(default) 0.00156726052#(8-1.93)/3873
  6 BWT PPMd(large) 0.000702230615#(8-2.05)/8473
  7 BWT PPMd-mx9 0.000292154827#(8-2.09)/20229
  8 BWT-SAP bzip2 0.00160155302#(8-1.40)/4121
  9 BWT-SAP PPMd(default) 0.00175588311#(8-1.21)/3867
  10 BWT-SAP PPMd(large) 0.00101265823#(8-1.28)/6636
  11 BWT-SAP PPMd-mx9 0.000452322738#(8-1.34)/14724
'))
close(con)

#Original size
df1 <- df1[df1$secondStage!="PPMd-mx9",]
df1 <- df1[df1$secondStage!="PPMd(large)",]
df1 <- df1[df1$secondStage!="bzip2",]

df1 <- transform(df1, firstStage = reorder(firstStage, Index))
library(ggplot2)

pdf(file="cb-3.pdf",height=5,width=7)

ggplot(data=df1, aes(x=firstStage, y=Compression, fill=secondStage)) + 
    geom_bar(colour="black", stat="identity",
             position=position_dodge(),
             size=.3) +                        # Thinner lines
    scale_fill_hue(name="Kompressionsstufe") +      # Set legend title
    xlab("Codierungsstufe") + ylab("Reduktion in Bits/Base pro Sekunde") + # Set axis labels
    ggtitle("Kompression / Sekunde nach Cox und Bauer") +  # Set title
    theme()
    