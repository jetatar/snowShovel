import os,sys
import ROOT

def fitAngSimCrossChanDelays(treenm, datbrnm, infn,
                             geofn, geon,
                             choffn, choff,
                             outfn,
                             minimizer, algo, 
                             rndtries, thetaScan, phiScan):
    nt = ROOT.TChain(treenm)
    nt.Add(infn)
    
    sel = ROOT.TAMSelector()
    
    # determine inter-channel correlations
    ctc = ROOT.TSnChanTimeCorlMod("ChanTimeCorl", datbrnm)
    ctc.SetAssumeAlwaysTree()
    #ctc.SetVerbosity(ctc.kDebugCorls)
    
    # fit to find the best offsets between channels
    fco = ROOT.TSnFitChanOffsetMod("FitChanOffsets",
                                   minimizer, algo)
    
    # load the geometry from a file
    ldgeo = ROOT.TSnLoadFromFileMod("LoadGeoMod",
                                    geofn, geon)
    
    # load the a-priori known channel offsets from a file
    ldoff = ROOT.TSnLoadFromFileMod("LoadOffsetsMod",
                                    choffn, choff)
    
    # fit the plane wave angle given the channel delays in the tree
    afs = ROOT.TSnAngleFromChanCorlMod("AngleFromChanCorlMod",
                                       minimizer, algo)
    afs.SetChanOffsetsName(choff)
    afs.SetGeometryName(geon)
    # uncomment to view the fit space
    #afs.SetVerbosity(afs.kDrawParSpace)
    afs.SetNumRandomStartValueTries(rndtries) # need ~50 with Migrad or GSLSimAn
                                              # don't need any with Genetic
    afs.SetNumThetaScanPoints(thetaScan)
    afs.SetNumPhiScanPoints(phiScan)
    
    # save the results - put before process so hopefully it can be used to
    # cache the output tree(s)
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    
    scd = ROOT.TSnSaveCorlDataMod("SaveCorlData",
                                  datbrnm,
                                  "CorlFilt",
                                  "channel correlations from filtered data")
    scd.SetAssumeAlwaysTree()
    sro = ROOT.TSnSaveRecoResultsMod("SaveDtFitResults",
                                     "FitOffsets",
                                     "fit channel offsets");
    sro.SaveRecoResult(fco.GetTimeOffsetsName().Data())
    srr = ROOT.TSnSaveRecoResultsMod("SaveAngFitResults",
                                     "FitAngle",
                                     "fit angle from single chan delays");
    srr.SaveRecoResult(afs.GetRecoAngleName().Data())
    
    ctc.Add(fco)
    fco.Add(ldgeo)
    ldgeo.Add(ldoff)
    ldoff.Add(afs)
    afs.Add(scd)
    scd.Add(sro)
    sro.Add(srr)
    
    print "starting ..."
    sel.SetVerbosity(10)
    sel.AddInput(ctc)
    try:
        ROOT.TTree.Process(nt, sel)#, "", 1)
    except Exception as e:
        print "exception!"
        print e
    print "process done!"
    
    # save the results
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()



if __name__=="__main__":
    
    
    infn  = sys.argv[1]
    outfn = sys.argv[2]
    
    treenm  = "SimTemplEvts"
    datbrnm = "NuData."
    
    geofn = "/data/users/cjreed/work/BounceStudy/Stn10/CampSiteGeometry.root"
    geon  = "CampSite2013"
    
    choffn = "/data/users/cjreed/work/BounceStudy/Stn10/ZeroChanOffsets.root"
    choff  = "ChanTimeOffsets"
    
    #minimizer = "Minuit2"
    #algo      = "Migrad"
    #rndtries  = 0
    #thetaScan = 90
    #phiScan   = 360
    minimizer = "GSLSimAn"
    algo      = ""
    rndtries  = 0
    thetaScan = 90
    phiScan   = 120
    #minimizer = "Genetic"
    #algo      = ""
    #rndtries  = 0
    #thetaScan = 0
    #phiScan   = 0
    
    fitAngSimCrossChanDelays(treenm, datbrnm, infn,
                             geofn, geon, choffn, choff,
                             outfn,
                             minimizer, algo, rndtries, thetaScan, phiScan)

    
    
