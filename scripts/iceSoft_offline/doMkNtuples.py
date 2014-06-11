#!/usr/bin/env python                                                                                                            

import os, sys

evfprefix="ev_"
hdfprefix="hd_"

#
## switches below
#

# if true, expect /runXXXX/event/ev_0.dat.gz inside 'indirbase'
# if false, expect all event files in a single directory ('indirbase')
FILES_IN_SEPARATE_RUN_DIRS=False

# if true, loop over files starting with evfprefix
# if false, loop over files starting with hdfprefix
USE_EV_FILES=False

#indirbase="/home/cjreed/work/AriannaData/data2010-2011"
#outdir="/home/cjreed/work/dataNtuples/data2010-2011"
#outtag="nt.data2010-11"

#indirbase="/home/cjreed/work/temp/run0001/event/"
#outdir="/home/cjreed/work/temp"
#outtag="nt.temp"

indirbase="/home/cjreed/work/AriannaData/replica/newMBtest"
outdir="/home/cjreed/work/AriannaData/replica/newMBtest"
outtag="nt"

runsubdir="event" # only if files in separate run directories


prfx = hdfprefix
alfx = evfprefix
if (USE_EV_FILES):
   prfx = evfprefix
   alfx = hdfprefix

files=0
if (FILES_IN_SEPARATE_RUN_DIRS):
   for sdir in os.listdir(indirbase):
      if (sdir.startswith("run")):
         runnum = sdir[3:]
         idr=indirbase+"/"+sdir+"/"+runsubdir
         try:
            for rfile in os.listdir(idr):
               if (rfile.startswith(prfx)):
                  seqnum = rfile[len(prfx):rfile.index(".")]
                  files+=1
                  os.system("root.exe -b -q mkNtuple.C+\("\
                            +runnum+","+seqnum+',\\\"'+outdir+'\\\",\\\"'\
                            +outtag+"\\\",\\\""+indirbase+'\\\"\)')

                  #sys.exit(0)
         except OSError:
            print("OS error for dir {0}".format(idr))
else:
   for rfile in os.listdir(indirbase):
      if (rfile.startswith(prfx)):
         hfn = hdfprefix + rfile[len(prfx):]
         efn = evfprefix + rfile[len(prfx):]
         outfn = outtag+"."+rfile
         fhfn = indirbase+"/"+hfn
         fefn = indirbase+"/"+efn
         files+=1
         os.system("root.exe -b -q mkNtuple.C+\("\
                   +'\\\"'+fhfn+'\\\",\\\"'+fefn+\
                   '\\\",\\\"'+outdir+'\\\",\\\"'+outfn+'\\\"\)')
         #sys.exit(0)

if (files==0):
   print ('no files found in directory: '+indirbase)

