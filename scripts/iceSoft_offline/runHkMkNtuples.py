#!/usr/bin/env python                                                                                                            

import os, sys

FILES_IN_SEPARATE_RUN_DIRS=False
# if true, expect /runXXXX/hk/hk_0.dat.gz inside 'indirbase'
# if false, expect all event files in a single directory ('indirbase')

hkfprefix="hk_"

#
## switches below
#

#indirbase="/home/cjreed/work/AriannaData/data2010-2011"
#outdir="/home/cjreed/work/dataNtuples/data2010-2011"
#outtag="nt.data2010-11"

#indirbase="/home/cjreed/work/temp/run0001/event/"
#outdir="/home/cjreed/work/temp"
#outtag="nt.temp"

indirbase="/data/2013/icicle2/run1371/hk"
outdir="/home/cjreed/work/hkAnalysis/NTUPLES_Mar14/"
outtag="hknt"

runsubdir="hk" # only if files in separate run directories


files=0
if (FILES_IN_SEPARATE_RUN_DIRS):
   for sdir in os.listdir(indirbase):
      if (sdir.startswith("run")):
         runnum = sdir[3:]
         idr=indirbase+"/"+sdir+"/"+runsubdir
         try:
            for rfile in os.listdir(idr):
               if (rfile.startswith(hkfprefix)):
                  outfn = outtag+"."+rfile
                  hkfn  = indirbase+"/"+sdir+"/"+runsubdir+"/"+rfile
                  files+=1
                  os.system("root.exe -b -q hkMkNtuple.C+\("\
                            +'\\\"'+hkfn+\
                            '\\\",\\\"'+outdir+'\\\",\\\"'+outfn+'\\\"\)')

                  #sys.exit(0)
         except OSError:
            print("OS error for dir {0}".format(idr))
else:
   for rfile in os.listdir(indirbase):
      if (rfile.startswith(hkfprefix)):
         outfn = outtag+"."+rfile
         hkfn  = indirbase+"/"+rfile
         files+=1
         os.system("root.exe -b -q hkMkNtuple.C+\("\
                   +'\\\"'+hkfn+\
                   '\\\",\\\"'+outdir+'\\\",\\\"'+outfn+'\\\"\)')

if (files==0):
   print('no files found in directory: '+indirbase)

