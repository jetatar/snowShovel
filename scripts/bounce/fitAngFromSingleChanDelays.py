import os,sys
import ROOT

def fitAngFromSingleChanDelays(ntn, infn,
                               geofn, geon,
                               chofffn, choff,
                               outfn,
                               minimizer, algo):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)
    
    sel = ROOT.TAMSelector()
    
    # load the geometry from a file
    ldgeo = ROOT.TSnLoadFromFileMod("LoadGeoMod",
                                    geofn, geon)
    
    # load the a-priori known channel offsets from a file
    ldoff = ROOT.TSnLoadFromFileMod("LoadOffsetsMod",
                                    chofffn, choff)
    
    # fit the plane wave angle given the channel delays in the tree
    afs = ROOT.TSnAngleFromSingleChanOffsets("AngleFromChanDelaysMod",
                                             minimizer, algo)
    afs.SetChanOffsetsName(choff)
    afs.SetGeometryName(geon)
    # uncomment to view the fit space
    #afs.SetVerbosity(200)
    
    # save the results - put before process so hopefully it can be used to
    # cache the output tree(s)
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    srr = ROOT.TSnSaveRecoResultsMod("SaveFitResults",
                                     "FitAngle",
                                     "fit angle from single chan delays");
    srr.SaveRecoResult(afs.GetRecoAngleName().Data())
    
    if (len(chofffn)>0):
        ldgeo.Add(ldoff)
        ldoff.Add(afs)
    else:
        ldgeo.Add(afs)
    afs.Add(srr)
    
    print "starting ..."
    sel.AddInput(ldgeo)
    try:
        ROOT.TTree.Process(nt, sel)#, "", 10)
    except:
        print "exception!"
    print "process done!"
    
    # save the results
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
    

if __name__=="__main__":
    
    if (len(sys.argv)<4):
        print "usage: python fitAngFromSingleChanDelays.py [input filename] "\
            "[geo filename] [geo name] [chan offset filename] "\
            "[output filename] (minimizer) (algorithm)"
    
    infn    = sys.argv[1]
    geofn   = sys.argv[2]
    geon    = sys.argv[3]
    chofffn = sys.argv[4]
    outfn   = sys.argv[5]
    
    
    minimizer = "Minuit2"
    algo      = "Migrad"
    if (len(sys.argv)>=8):
        minimizer = sys.argv[6]
        algo      = sys.argv[7]

    fitAngFromSingleChanDelays("FitOffsets",infn,
                               geofn,geon,
                               chofffn, "ChanTimeOffsets",
                               outfn,
                               minimizer, algo)

