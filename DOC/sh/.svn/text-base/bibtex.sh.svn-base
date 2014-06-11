#!/bin/bash
# $Id: bibtex.sh,v 1.1.1.1 2013-03-05 05:18:32 cjreed Exp $
#

if [ -d "$1" ]; then
   test;
else
   exit 1;
fi

for i in `ls $1/*.aux`; do
   if [ -n "`grep "\citation" $i`" ]; then
      bibtex $i;
   else
      bb=`expr match "$i" '.*\(\.bib\)'`;
      echo $bb;
      touch $bb.bbl;
   fi
done

#if test -z "$1"; then exit 1; fi
#
#if test -n "`grep "\citation" $1.aux`"; then
# bibtex $1
#else
# touch $1.bbl
#fi
