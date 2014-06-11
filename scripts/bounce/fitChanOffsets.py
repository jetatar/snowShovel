import os,sys
import ROOT

def fitChanOffsets(ntn, dbn, infn, outfn, minimizer, algo, filt):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)
        
    sel = ROOT.TAMSelector()
    
    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.SetCheckCRC(True)

    sfr = ROOT.TSnStopFindRms("StopFindRms", dbn)
    stopnm = "{0}.".format(sfr.GetStopPosnName().Data())
    sfr.SetStopPosnName(stopnm)
    
    shiftnm  = dbn.rstrip(".")
    shiftnm += "Shifted."
    shd = ROOT.TSnShiftCalWvDataMod("ShiftDataByStops", dbn, shiftnm)
    shd.SetStopPosnName(stopnm)
    
    # filter out high frequencies
    fltfcn = ROOT.TF1("fltfcn","gaus(0) + gaus(3)", -1.1, 1.1)
    fltfcn.SetNpx(1000)
    fltfcn.SetParameters(1.0, -0.3, 0.1, 1.0, 0.3, 0.1)
    flt = ROOT.TSnFilterWaveformMod("FilterWaveformMod",
                                    shiftnm,
                                    "FilteredData.",
                                    fltfcn)
    # uncomment and the module will display plots each event
    #flt.SetVerbosity(ROOT.TSnFilterWaveformMod.kDebugFilter)
    
    cdnm = flt.GetFltDatName().Data()
    if (filt==False):
        cdnm = shiftnm
    
    # determine inter-channel correlations
    ctc = ROOT.TSnChanTimeCorlMod("ChanTimeCorl",
                                  cdnm)
    
    # fit to find the best offsets between channels
    fco = ROOT.TSnFitChanOffsetMod("FitChanOffsets",
                                   minimizer, algo)

    # save the results
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    scd = ROOT.TSnSaveCorlDataMod("SaveCorlData",
                                  cdnm,
                                  "CorlFilt",
                                  "channel correlations from filtered data")
    srr = ROOT.TSnSaveRecoResultsMod("SaveFitResults",
                                     "FitOffsets",
                                     "fit channel offsets");
    srr.SaveRecoResult(sfr.GetStopPosnName().Data())
    srr.SaveRecoResult(fco.GetTimeOffsetsName().Data())
    
    
    bes.Add(sfr)
    sfr.Add(shd)
    if (filt):
        print "filtering data"
        shd.Add(flt)
        flt.Add(ctc)
    else:
        print "not filtering"
        shd.Add(ctc)
    ctc.Add(fco)
    fco.Add(scd)
    fco.Add(srr)
    
    print "starting ..."
    sel.AddInput(bes)
    ROOT.TTree.Process(nt, sel)#, "", 10)
    # use Proof Lite .. shouldn't do this with TTrees in the output
    #ROOT.TProof.Open("workers=10")
    #nt.SetProof()
    #ROOT.gProof.AddInput(bes)
    #ROOT.TTree.Process(nt, "TAMSelector")
    print "process done!"
    
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
    
    


if __name__=="__main__":
    if (len(sys.argv)<4):
        print "Usage: python fitChanOffsets.py [input filename (w/ wildcards)] [tree type] [output filename] (minimizer) (algorithm) (apply filter True/False)"
        print "   tree type = 0 for raw, 1 for FPN sub, 2 for calibrated amp out"
        sys.exit()

    infn = sys.argv[1]
    
    tt = int(sys.argv[2])
    ntn = None
    dbn = None
    if (tt==0):
        ntn = ROOT.TSnRawTreeMaker.kETname
        dbn = ROOT.TSnRawTreeMaker.kEWvBrNm
    elif (tt==1):
        ntn = ROOT.TSnSaveCalibDataMod.kFpnEvtTreeNm
        dbn = ROOT.TSnSaveCalibDataMod.kFPNSubDatBrNm
    elif (tt==2):
        ntn = ROOT.TSnSaveCalibDataMod.kCalEvtTreeNm
        dbn = ROOT.TSnSaveCalibDataMod.kAmpOutDatBrNm
    else:
        print "Unknown tree type [{0}]".format(tt)
        sys.exit()
    
    outfn = sys.argv[3]

    minimizer = "Minuit2"
    algo      = "Migrad"
    if (len(sys.argv)>=6):
        minimizer = sys.argv[4]
        algo      = sys.argv[5]

    filt = True
    if (len(sys.argv)>=7):
        if (sys.argv[6]=='False'):
            filt = False
    
    fitChanOffsets(ntn, dbn, infn, outfn, minimizer, algo, filt)
    


