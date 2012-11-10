library(ggplot2,quietly=TRUE)
require(grid)

source("combined-time-size-diagram.r")
source("simple-diagrams.r")
source("multiplot.r")

fileList <- dir("../rawdata",pattern="snappy\\.compression\\..*.txt")

vp.layout <- function(x, y) viewport(layout.pos.row=x, layout.pos.col=y)
arrange_ggplot2 <- function(..., nrow=NULL, ncol=NULL, as.table=FALSE) {
  dots <- list(...)
  n <- length(dots)
  if(is.null(nrow) & is.null(ncol)) { nrow = floor(n/2) ; ncol = ceiling(n/nrow)}
  if(is.null(nrow)) { nrow = ceiling(n/ncol)}
  if(is.null(ncol)) { ncol = ceiling(n/nrow)}
  ## NOTE see n2mfrow in grDevices for possible alternative
  grid.newpage()
  pushViewport(viewport(layout=grid.layout(nrow,ncol) ) )
  ii.p <- 1
  for(ii.row in seq(1, nrow)){
    ii.table.row <- ii.row	
    if(as.table) {ii.table.row <- nrow - ii.table.row + 1}
    for(ii.col in seq(1, ncol)){
      ii.table <- ii.p
      if(ii.p > n) break
      print(dots[[ii.table]], vp=vp.layout(ii.table.row, ii.col))
      ii.p <- ii.p + 1
    }
  }
}



for(i in 1:length(fileList)) {
  sourceFileName <- strsplit(fileList[i],"\\.")[[1]][3];
  cat("Processing ",sourceFileName,"\n");	#Rename columns
  
  compression <- read.csv(file=paste("../rawdata/snappy.compression.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  comptime <- read.csv(file=paste("../rawdata/snappy.comptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  decomptime <- read.csv(file=paste("../rawdata/snappy.decomptime.",sourceFileName,".txt",sep=""),head=TRUE,sep=",")
  
   #Create the plots, not neccessarily 1 plot per variable
  plot1 <- createCompressionDiagram(compression, sourceFileName);
  compressionTimeDiagrams <- createCompressionTimeDiagram(comptime, sourceFileName)
  plot2 <- compressionTimeDiagrams[1]
  plot3 <- compressionTimeDiagrams[2]
  plot3 <- createDecompressionTimeDiagram(decomptime, sourceFileName)
  sizeTimeDiagrams <- createCombinedSizeDiagrams(compression, comptime, decomptime, sourceFileName)
  plot4 <- sizeTimeDiagrams[1]
  plot5 <- sizeTimeDiagrams[2]
  #print(length(plot))
  #Setup the PDF device
  pdf(file=paste("pdf/Benchmark-",sourceFileName, ".pdf", sep=""), height=5, width=7)
  #vp.setup(2,3);
  #print(typeof(plot1))
  #Write all plots
  print(plot1);
  #grid.newpage();
  print(plot2);
  #grid.newpage();
  print(plot3);
  #grid.newpage();
  print(plot4);
  #grid.newpage();
  print(plot5);
  #arrange_ggplot2(plot1, plot2, plot3, plot4, plot5, ncol=2)
  #multiplot(plot1, plot2, plot3, plot4, plot5, cols=2)
  #Close the device
  dev.off();
}