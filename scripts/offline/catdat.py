import os,sys
#import cPickle
import ROOT

runs=[31,36,45,53,64,68,71]

rt = ROOT.TChain("runtree")
vt = ROOT.TChain("volttree")

outf = ROOT.TFile.Open("nt.SnEvts.r31r36r45r53r64r68r71.root","recreate")
outf.cd()
# run numbers pretty small; this shouldn't waste too much memory
# array indexed like:
#  RunSeqConfs[run num][seq num][conf num]
#  where conf num = 0 => RunInfo, 1 => DAQConfig, 2 => NetAdrSet
runconfs = ROOT.TObjArray(max(runs)+1)

def parseFilename(fn):
    # assumes filename has the form
    #  nt.SnEvtsM0002F7F0C3B6r00045s00100.root
    rst = fn.index("r")
    sst = fn.rindex("s")
    mac = fn[fn.index("M")+1:rst]
    run = int(fn[rst+1:sst])
    seq = int(fn[sst+1:fn.index(".",sst)])
    return mac,run,seq

for r in runs:
    print "Adding run {0}".format(r)
    rpath = "/home/cjreed/work/DATA/2013_InSitu/runtrees/0002F7F0C3B6/"\
        "r{0:05d}".format(r)
    seqconfs = ROOT.TObjArray()
    runconfs.AddAt(seqconfs, r)
    for root, dirs, files in os.walk(rpath):
        for f in files:
            if ( f.startswith("nt.SnEvts")
                 and ("_" not in f) ): # just skip duplicate files for now
                fn = "{0}/{1}".format(root,f)
                # add to chains
                rt.Add(fn)
                vt.Add(fn)
                # open file to get config stuff
                inf = ROOT.TFile.Open(fn)
                cfgs = [ inf.Get("RunInfo"), 
                         inf.Get("DAQConfig"),
                         inf.Get("NetAdrSet") ]
                # put config stuff in the arrays
                mac,run,seq = parseFilename(f)
                ncfgs = len(cfgs)
                configs = ROOT.TObjArray(ncfgs)
                seqconfs.AddAtAndExpand(configs, seq)
                for i in range(0, ncfgs):
                    configs.AddAt(cfgs[i],i)
                inf.Close()

outf.cd()
runconfs.Write("RunSeqConfs", ROOT.TObject.kSingleKey)
runconfs = None
outf.cd()
print "Cloning run tree..."
crt = rt.CloneTree(-1,'fast')
print "Cloning volt tree..."
cvt = vt.CloneTree(-1,'fast')
crt.SetDirectory(outf)
cvt.SetDirectory(outf)
outf.Write()
print "Done."

    
