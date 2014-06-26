import os,sys
import subprocess


def fillFromDups(basedir):
    # assuming bad files have been cleared away in BOTH the raw and
    # duplicates directories (see moveBadFiles.py), check if any gaps
    # in the raw can be filled by something from duplicates
    
    dupdir = "{0}/duplicates".format(basedir)
    rawdir = "{0}/raw".format(basedir)
    
    for path,dirs,files in os.walk(dupdir):
        for fn in files:
            if (fn.endswith(".dat")):
                dupfn = "{0}/{1}".format(path,fn)
                relp = dupfn.split(basedir)[-1].strip('/')
                rel = relp.split('/')
                rel[0] = 'raw'
                rawfn = "/".join(rel)
                rawfn, rawf  = os.path.split(rawfn)
                if ("_" in rawf):
                    rawf  = rawf.split("_")[0]
                    rawf += ".dat"
                rawfn = "{0}/{1}/{2}".format(basedir, rawfn, rawf)
                
                if not os.path.exists(rawfn):
                    args = ["cp","-p",dupfn,rawfn]
                    print " ".join(args)
                    subprocess.call(args)

if __name__=="__main__":
    
    if (len(sys.argv)<2):
        print "Usage: python fillFromDups.py [base directory]"
        sys.exit()
    
    basedir = sys.argv[1]

    fillFromDups(basedir)
