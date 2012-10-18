#!/bin/sh

if [ "x"$LOGFILE == "x" ]
then
  LOGFILE=cmd.log
fi

#echo "LOGFILE=$LOGFILE"

echo -n "# " >> $LOGFILE
date >> $LOGFILE
echo "$ $*" >> $LOGFILE

($*) | tee -a $LOGFILE

echo -n "# " >> $LOGFILE
date >> $LOGFILE

