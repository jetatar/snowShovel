import os,sys
import subprocess
import ROOT

def combineNonHtbt(indir, outfn, htbtOnly=False):
    
    fileAndTrgTimes = []
    outf = ROOT.TFile(outfn,"recreate")
    if not outf.IsOpen():
        print "Could not open output file {0}".format(outfn)
        return
    
    n = 0
    
    for path,dirs,files in os.walk(indir):
        #if n>50:
        #    break

        for fn in files:
            if (fn.startswith("RawTree") and fn.endswith(".root")):
                
                isok = True
                infn = "{0}/{1}".format(path,fn)
                inf = ROOT.TFile(infn)
                if (inf.IsOpen()):
                    ct = inf.Get("ConfigTree")
                    ROOT.SetOwnership(ct, True)
                    trgstart = None
                    # there shouldn't be more than one config, but...
                    for c in ct:
                        # no heartbeat runs
                        ishtbt = (c.DAQConfig.GetHeartBeatPeriod()!=0)
                        if (ishtbt and (htbtOnly==False) ):
                            # reject heartbeats
                            isok = False
                            print "Heartbeat run: {0}".format(infn)
                        elif ((ishtbt==False) and htbtOnly):
                            # only want heartbeats
                            isok = False
                        # no trigger tuning runs or test runs with 1 chan
                        if (c.DAQConfig.GetNumCardsMajLog()<2):
                            isok = False
                            print "Unwanted majority logic {0}: {1}"\
                                .format(c.DAQConfig.GetNumCardsMajLog(),infn)
                        # no runs with the cards off during data taking
                        if not c.DAQConfig.IsPoweredFor(\
                            ROOT.TSnDAQConfig.kCardDatTak):
                            isok = False
                            print "Cards unpowered: {0}".format(infn)

                        if isok:
                            tt = c.TrigStartClock.CalcAbsCurrTime().AsDouble()
                            if (trgstart==None):
                                trgstart = tt
                            else:
                                trgstart = min(trgstart,tt)
                        else:
                            break

                    if (isok):
                        n += 1
                        if ( (n % 100)==0 ):
                            #sys.stdout.write("Added {0} files ...       \r"\
                            #                     .format(n))
                            print "Added {0} files ...".format(n)
                        keys = inf.GetListOfKeys()
                        ROOT.SetOwnership(keys, True)
                        for key in keys:
                            if key.GetClassName() == "TTree":
                                kn = key.GetName()
                                fileAndTrgTimes.append( (trgstart, infn, kn) )

                else:
                    print "Could not open {0}".format(infn)
    
    #sys.stdout.write("\n")
    print "Total {0} files".format(n)

    print "Sorting trigger start times and building chain..."
    fileAndTrgTimes.sort()
    outf.cd()
    chains = {}
    for trgstart,infn,kn in fileAndTrgTimes:
        if kn not in chains:
            chains[kn] = ROOT.TChain(kn)
            chains[kn].SetDirectory(outf)
        chains[kn].Add(infn)

    print "Cloning chains..."
    for nm,ch in chains.iteritems():
        outf.cd()
        t = ch.CloneTree(-1,"fast")
        if (t!=None):
            t.Write()
        else:
            print "No tree for {0}".format(nm)


if __name__=="__main__":
    
    if (len(sys.argv)<3):
        print "Usage: python combineNonHtbt.py [input base dir] "\
            "[output file with path] "\
            "(combine only heartbeats rather than none True/False)"
        sys.exit(-1)

    htbtOnly = False
    if (len(sys.argv)>3):
        htbtOnly = (sys.argv[3]=="True")
        
    combineNonHtbt(sys.argv[1], sys.argv[2], htbtOnly)

