#!/bin/bash

dir=`pwd`

echo "*** Making libEvent.so ***"

tdir=/tmp/`date "+%s"`
mkdir $tdir

cd $tdir

cp -a $ROOTSYS/test/* .

echo "*** Making Event program ***"
make Event  > event.make

echo "*** Generating events ***"

echo "Event1.root..."
time ./Event > event.gen 
mv Event.root $dir/Event1.root

echo "Event2.root..."
time ./Event > event.gen
mv Event.root $dir/Event2.root

mv libEvent.so $dir

cd $dir
rm -rf $tdir
