#!/bin/env Rscript

args<-commandArgs(TRUE)

if (length(args) == 0)
{
  cat("USAGE: draw_hist.R list.txt [numbreaks] [xmin xmax]\n")
} else {
  file <- args[[1]]

  outfile<-paste(file, ".hist.png", sep="")
  cat("Loading from: ", file, "\nWriting to: ", outfile, "\n")

  values<-scan(file)

  numbreaks=1000
  xmin=min(values)
  xmax=max(values)

  if (length(args) > 1) { numbreaks=as.integer(args[[2]]); }
  if (length(args) > 3) { xmin=as.integer(args[[3]]); xmax=as.integer(args[[4]]); }

  cat("breaks:", numbreaks, " xmin:", xmin, " xmax:", xmax, "\n");

  png(outfile)
  hist(values, breaks=numbreaks, xlim=c(xmin, xmax), main=paste("Histogram of", file))
  dev.off()
}
