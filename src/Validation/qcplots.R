## Run the following steps first:
## % qc-combine */*.qc > qc.combine
## % sed 's/^ *$//' qc.combine | grep -v '^$' | grep -v '^\[' | grep -v '^total' > qc.combine.R
## % vi qc.combine.R  (to fix the headers)

l<-read.table("qc.combine.R")
t<-t(l)

s<-seq(-20,90,by=5)

#postscript("stats.ps", paper="letter")

par(mfrow=c(3,4))

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,17]), xlab=t[1,1], ylab=t[1,17], main="Error Rate vs 2KbScaffolds")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,18]), xlab=t[1,1], ylab=t[1,18], main="Error Rate vs 2KbScaffoldSpan")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,67]), xlab=t[1,1], ylab=t[1,67], main="Error Rate vs DegenReads")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,11]), xlab=t[1,1], ylab=t[1,11], main="Error Rate vs N50Scaffold")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)



plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,77]), xlab=t[1,1], ylab=t[1,77], main="Error Rate vs 2KbContigs")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,78]), xlab=t[1,1], ylab=t[1,78], main="Error Rate vs 2KbContigBases")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,69]), xlab=t[1,1], ylab=t[1,69], main="Error Rate vs SurrogateReads")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,30]), xlab=t[1,1], ylab=t[1,30], main="Error Rate vs N50Contig")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)




plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,75]), xlab=t[1,1], ylab=t[1,75], main="Error Rate vs 2KbUnitigs")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,76]), xlab=t[1,1], ylab=t[1,76], main="Error Rate vs 2KbUnitigBases")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,70]), xlab=t[1,1], ylab=t[1,70], main="Error Rate vs SingletonReads")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)



plot(axes=0, frame.plot=1, type="p", cbind(t[,1],t[,10]), xlab=t[1,1], ylab=t[1,10], main="Error Rate vs MaxScaffold")
abline(v=s,lty="dotted")
axis(side=2)
axis(at=s, side=1)

#dev.off()

