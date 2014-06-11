import sys
import ROOT

if (len(sys.argv)<2):
    print 'Need an input file'
    sys.exit(0)

infn = sys.argv[1]

inf = ROOT.TFile.Open(infn)
nt = inf.Get("runtree")
allgood = True
if (nt!=None):
    nevts = nt.GetEntries()
    for e in range(0, nevts):
        nt.GetEntry(e)
        tbits = nt.extra[10]
        if ( (tbits & (1<<7))!=0 ):
            print 'Bad readout tag at entry {0}, EvtNum={1}'\
                .format(e, nt.EvtNum)
            allgood = False
else:
    print "Didn't find runtree in file {0}".format(infn)

if (allgood):
    print 'No events with bad readout tag (all ok).'


    
    
