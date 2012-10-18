#!/bin/sh

if [ $# -lt 3 ]
then
  echo "preparescaff.sh prefix olddir newdir"
  exit
fi

prefix=$1
olddir=$2
newdir=$3

mkdir $newdir
ln -s ../$olddir/$prefix.{Arc,contig,ContigIndex,edge,kmerFreq,preArc,preGraphBasic,updated.edge,vertex} $newdir

