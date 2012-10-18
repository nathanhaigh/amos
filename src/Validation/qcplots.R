l<-read.table("qcplots")
t<-t(l)

s<-seq(-20,90,by=1)

#postscript("stats.ps", paper="letter")

par(mfrow=c(3,4), oma=c(0,0,3,0))

for (i in 2:dim(t)[2])
{
  plot(axes=0, frame.plot=1, type="b", cbind(t[,1],t[,i]), xlab=t[1,1], ylab=t[1,i], main=paste(t[1,1], " vs ", t[1,i]))
  abline(v=s,lty="dotted")
  axis(side=2)
  axis(at=s, side=1)
}

mtext("Genome plots", outer=TRUE)

#dev.off()

