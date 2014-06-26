import os,sys
import subprocess

from AriUtils import *

def moveBadFiles(infn, basedir, baddir="badOrIncomplete"):
    # infn should be a text file obtained by:
    # grep "FILE NOT OK" [checkRawFiles log] > [infn]
    # 
    # this routine loops over that file (each line is FILE NOT OK: filename)
    # and moves it to basedir/baddir, preserving the subdirectory
    # structure


    with open(infn) as inf:
        for line in inf:
            ff = line.split()[-1]
            rl = ff.split(basedir)[-1]
            rl = rl.strip('/') # in case there were double /'s
            rp = rl.split('/')
            rp[0] = baddir
            op = "/".join(rp)
            op = "{0}/{1}".format(basedir,op)
            
            checkDirExists(os.path.split(op)[0])
            
            args = ["mv",ff,op]
            print " ".join(args)
            subprocess.call(args)
            

if __name__=="__main__":
    
    if (len(sys.argv)<3):
        print "Usage: python moveBadFiles.py [file list] [base directory]"
        sys.exit()
    
    infn    = sys.argv[1]
    basedir = sys.argv[2]

    moveBadFiles(infn, basedir)
