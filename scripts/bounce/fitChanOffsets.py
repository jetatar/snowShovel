import os,sys
import ROOT

def fitChanOffsets(ntn, dbn, infn, outfn, minimizer, algo, filt, envnm, stpshft):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)
        
    sel = ROOT.TAMSelector()
    
    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.SetCheckCRC(True)

    sfr = ROOT.TSnStopFindRms("StopFindRms", dbn)
    stopnm = "{0}.".format(sfr.GetStopPosnName().Data())
    sfr.SetStopPosnName(stopnm)
    
    shiftnm = None
    shd = None
    if (stpshft):
        shiftnm  = dbn.rstrip(".")
        shiftnm += "Shifted."
        shd = ROOT.TSnShiftCalWvDataMod("ShiftDataByStops", dbn, shiftnm)
        shd.SetStopPosnName(stopnm)
    else:
        shiftnm = dbn
    
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

    # find envelopes
    menv = None
    if (envnm!=None):
        menv = ROOT.TSnFindWaveformEnvelopeMod("EnvMod", cdnm, envnm)
        #menv.SetVerbosity(menv.kDebugEnv)
        
    
    # determine inter-channel correlations
    ctc = ROOT.TSnChanTimeCorlMod("ChanTimeCorl",
                                  cdnm)
    if (envnm!=None):
        ctc.SetEnvelopeName(envnm)
    
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
    if (stpshft):
        sfr.Add(shd)
    if (filt):
        print "filtering data"
        if (stpshft):
            shd.Add(flt)
        else:
            sfr.Add(flt)
        if (envnm!=None):
            flt.Add(menv)
            menv.Add(ctc)
        else:
            flt.Add(ctc)
    else:
        print "not filtering"
        if (envnm!=None):
            if (stpshft):
                shd.Add(menv)
            else:
                sfr.Add(menv)
            menv.Add(ctc)
        else:
            if (stpshft):
                shd.Add(ctc)
            else:
                sfr.Add(ctc)
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
        print "Usage: python fitChanOffsets.py [input filename (w/ wildcards)] [tree type] [output filename] (minimizer) (algorithm) (apply filter True/False)  (use envelopes True/False) (shift stops to end True/False)"
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

    envnm = None
    if (len(sys.argv)>=8):
        if (sys.argv[7]=='True'):
            envnm = "{0}Env".format(dbn)
    
    stpshft = True
    if (len(sys.argv)>=9):
        if (sys.argv[8]=='False'):
            stpshft = False
    
    fitChanOffsets(ntn, dbn, infn, outfn, minimizer, algo, filt, envnm, stpshft)
    


