#!/usr/bin/env Rscript

#./simulatelayout.pl -readlen 1000  -numreads 500 -covs 1 -cove 20 -erate .20 > layout.20 &
#./simulatelayout.pl -readlen 1000  -numreads 500 -covs 1 -cove 20 -erate .15 > layout.15 &
#./simulatelayout.pl -readlen 1000  -numreads 500 -covs 1 -cove 20 -erate .10 > layout.10 &
#./simulatelayout.pl -readlen 1000  -numreads 500 -covs 1 -cove 20 -erate .05 > layout.05 &


l05<-read.table("layout.05")
l10<-read.table("layout.10")
l15<-read.table("layout.15")
l20<-read.table("layout.20")

pdf("accuracy.pdf", width=7, height=5)

plot(l20[[4]],l20[[14]], typ="l", xlab="coverage", ylab="cns error rate", lwd=2)

abline(h=c(0,5,10,15,20,25,30,35), lty=2, col="grey")
abline(v=c(0,5,10,15,20,25,30,35), lty=2, col="grey")

lines(l20[[4]],l20[[14]], typ="l", col="black", lwd=2)
lines(l15[[4]],l15[[14]], typ="l", col="red", lwd=2)
lines(l10[[4]],l10[[14]], typ="l", col="blue", lwd=2)
lines(l05[[4]],l05[[14]], typ="l", col="green", lwd=2)

legend(15,35, legend=c("20% error", "15% error", "10% error", "5% error"), col=c("black", "red", "blue", "green"), lwd=2, bg="white")

dev.off()



#(for i in `gseq 1 40`; do ./simulatelayout.pl -readlen 1000  -numreads 500 -cov 20 -erate .05 -minovl $i; done ) > minovl.05 &
#(for i in `gseq 1 40`; do ./simulatelayout.pl -readlen 1000  -numreads 500 -cov 20 -erate .10 -minovl $i; done ) > minovl.10 &
#(for i in `gseq 1 40`; do ./simulatelayout.pl -readlen 1000  -numreads 500 -cov 20 -erate .15 -minovl $i; done ) > minovl.15 &
#(for i in `gseq 1 40`; do ./simulatelayout.pl -readlen 1000  -numreads 500 -cov 20 -erate .20 -minovl $i; done ) > minovl.20 &

o05<-read.table("minovl.05")
o10<-read.table("minovl.10")
o15<-read.table("minovl.15")
o20<-read.table("minovl.20")

pdf("overlaprecovery.pdf", width=7, height=5)

plot(o20[[16]],o20[[20]], typ="l", xlab="seed length", ylab="overlap discovery rate", lwd=2)

abline(h=seq(0,100,10), lty=2, col="grey")
abline(v=c(0,5,10,15,20,25,30,35,40), lty=2, col="grey")

lines(o20[[16]],o20[[20]], typ="l", col="black", lwd=2)
lines(o15[[16]],o15[[20]], typ="l", col="red",   lwd=2)
lines(o10[[16]],o10[[20]], typ="l", col="blue",  lwd=2)
lines(o05[[16]],o05[[20]], typ="l", col="green", lwd=2)

legend(30,99,legend=c("20% error", "15% error", "10% error", "5% error"), col=c("black", "red", "blue", "green"), lwd=2)

dev.off()



