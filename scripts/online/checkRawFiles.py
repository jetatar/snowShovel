import os,sys
#import subprocess
import ROOT

from AriUtils import *


def checkRawFiles(basedir, clkIOvers=1):
    # print out warnings if a file is not ok
    # the output is intended to be piped to a log file
    
    ROOT.gROOT.LoadMacro("checkRawFile.C+")
    
    dupdir = "{0}/duplicates".format(basedir)
    rawdir = "{0}/raw".format(basedir)
    
    for ddir in [dupdir, rawdir]:
        for path,dirs,files in os.walk(ddir):
            for fn in files:
                if (fn.endswith(".dat")):
                    infn = "{0}/{1}".format(path,fn)
                    #print infn

                    isok = False
                    try:
                        isok = ROOT.checkRawFile(infn, clkIOvers)
                    except Exception as e:
                        print e

                    if not isok:
                        print "FILE NOT OK: {0}".format(infn)



if __name__=="__main__":
    
    if (len(sys.argv)<2):
        print "Usage: python checkDuplicates.py [data base directory]"
        sys.exit()
    
    basedir = sys.argv[1]
    
    checkRawFiles(basedir)


