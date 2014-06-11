#!/usr/bin/env python

import os, sys

filepath = "~jamesaw1/work/snowShovel_datafiles/"
filestem = "SnEvtsM0002F7F0D288r00112s00000"
FPNfn    = filepath + "peds.SnEvtsM0002F7F0D288r00111s00019.root"

infile   = filepath + filestem + ".dat"
rootfile = filepath + "nt." + filestem + ".root"
histfile = filepath + "hists." + filestem + ".root"

if len(sys.argv) > 1:
   infile   = sys.argv[1]
if len(sys.argv) > 2:
   FPNfn   = sys.argv[2]
if len(sys.argv) > 3:
   rootfile   = sys.argv[3]
if len(sys.argv) > 4:
   histfile   = sys.argv[4]

os.system('root -l -b -q WriteRootFile.C+\(\\\"'+rootfile+'\\\",\\\"'+infile+'\\\"\)')
os.system('root -l -b -q makeWaves.C+\(\\\"'+rootfile+'\\\",\\\"'+histfile+'\\\",\\\"'+FPNfn+'\\\"\)')
os.system('root -l showEventGui.C+\(\\\"'+histfile+'\\\"\)')
