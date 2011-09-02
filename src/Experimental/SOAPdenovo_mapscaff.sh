#!/bin/bash

PROC=24

if [ $# -lt 3 ]
then
 echo "mapscaff [all|map|scaff|index|stats] config prefix [procs=$PROC]"
 exit
fi

MODE=$1
CONFIG=$2
PREFIX=$3

if [ $# -gt 3 ]
then
  PROC=$4
fi

for readfile in `grep '^q[12]*=' $CONFIG | cut -f2 -d'='`
do
  if [ ! -r $readfile ]
  then
    echo "ERROR: Can't read $readfile"
    exit
  fi
done

if [ $MODE == "all" ] || [  $MODE == "map" ]
then
  echo "soap map"
  rec SOAPdenovo63mer map -s $CONFIG -g $PREFIX -p $PROC
fi

if [ $MODE == "all" ] || [ $MODE == "map" ] || [ $MODE == "scaff" ]
then
  echo "soap scaff"
  rec SOAPdenovo63mer scaff -g $PREFIX -F -p $PROC
fi


if [ $MODE == "all" ] || [ $MODE == "map" ] || [ $MODE == "scaff" ] || [ $MODE == "index" ]
then
  echo "bwa index"
  rec bwa index $PREFIX.scafSeq
fi


if [ $MODE == "all" ] || [ $MODE == "map" ] || [ $MODE == "scaff" ] || [ $MODE == "index" ] || [ $MODE == "stats" ]
then
  echo "stats"

  if [ ! -r $PREFIX.scafSeq.ctg.fa ]
  then
    echo " splitscaff"
    splitscafffa $PREFIX.scafSeq > $PREFIX.scafSeq.ctg.fa
  fi

  echo " scanning"

  echo -n "Scaffolds "                                              >  $PREFIX.scafSeq.stats
  getlengths $PREFIX.scafSeq | stats -f 2 -big 100,500,1000         >> $PREFIX.scafSeq.stats
  echo -n "Contigs "                                                >> $PREFIX.scafSeq.stats
  getlengths $PREFIX.scafSeq.ctg.fa | stats -f 2 -big 100,500,1000  >> $PREFIX.scafSeq.stats
  echo -n "Unitigs "                                                >> $PREFIX.scafSeq.stats
  getlengths $PREFIX.contig | stats -f 2 -big 100,500,1000          >> $PREFIX.scafSeq.stats

  cat $PREFIX.scafSeq.stats
fi
