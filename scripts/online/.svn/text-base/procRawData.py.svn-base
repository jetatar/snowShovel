import os,sys

import dataOrganizer


if (len(sys.argv)<5):
    print "Usage: python procRawData.py [dir to move raw files FROM] [dir to move raw files TO] [dir to store runtrees] [dir to store hist files]"

dataOrganizer.binDir=sys.argv[1]
dataOrganizer.storeBaseDir=sys.argv[2]
dataOrganizer.ntupleBaseDir=sys.argv[3]
dataOrganizer.wavesBaseDir=sys.argv[4]
dataOrganizer.dupeBaseDir="/tmp/"

dataOrganizer.appendChartPoints=False
dataOrganizer.doMakeWave=False

dataOrganizer.doRawDataCheck()


