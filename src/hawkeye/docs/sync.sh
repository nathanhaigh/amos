#!/bin/sh

rsync --rsh=ssh -av  mcschatz@shell.sourceforge.net:amos/htdocs/hawkeye/ .
