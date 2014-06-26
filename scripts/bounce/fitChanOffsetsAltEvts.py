import os,sys
import ROOT

def getSplitSafeBrNm(s):
    x = s
    if ( (len(x)>0) and (x.endswith(".")==False) ):
        x += "."
    return x
        

def fitChanOffsetsAltEvts(ntn, dbn, infn1, infn2, outfn,
                          minimizer, algo, filt, envnm, altevt,
                          shiftStop):
    
    nt = [ROOT.TChain(ntn), ROOT.TChain(ntn)]
    nt[0].Add(infn1)
    nt[1].Add(infn2)
    
    alttag = "AltRun"
    
    sel = ROOT.TAMSelector()

    # remove pre-triggered events
    cfl = ROOT.TSnConfigTreeLoader()
    sel.AddLoader(cfl)
    ree = ROOT.TSnRejectEarlyEvtsMod("RejectEarlyEvtsMod")

    # first run through: apply the event selection
    bes = [ ROOT.TSnBasicEvtSelMod("BasicEvtSelMod"),
            ROOT.TSnBasicEvtSelMod("{0}BasicEvtSelMod".format(alttag)) ]
    bes[0].GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes[0].SetCheckCRC(True)
    bes[1].GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes[1].SetCheckCRC(True)
    bes[1].SetHeaderBranchName("{0}{1}".format(
            alttag, bes[1].GetHeaderBranchName().Data()))
    bes[1].SetEvtWvfmBranchName("{0}{1}".format(
            alttag, bes[1].GetEvtWvfmBranchName().Data()))
    
    # add loader for the alternate run
    altloader = ROOT.TSnAltTreeSingleEntryLoader(alttag, nt[1], altevt)
    #altloader = ROOT.TSnAltTreeRandomLoader(alttag, nt[1], 67392)
    #altloader = ROOT.TSnAltTreeCyclicLoader(alttag, nt[1])
    #altloader = ROOT.TSnAltTreeShuffleLoader(alttag, nt[1])
    sel.AddLoader(altloader)

    altdbn = alttag + dbn
    
    # find the stops
    sfr = [ ROOT.TSnStopFindRms("StopFindRms", dbn),
            ROOT.TSnStopFindRms("StopFindRms{0}".format(alttag), altdbn) ]
    stopnm    = "{0}.".format(sfr[0].GetStopPosnName().Data())
    altstopnm = alttag + stopnm
    sfr[0].SetStopPosnName(stopnm)
    sfr[1].SetStopPosnName(altstopnm)
    # keep the stops the same in all channels
    for ss in sfr:
        ss.SetStopWidth(6)
        ss.SetSearchWidth(6)
        ss.SetMaxJitter(0)
    
    # shift data by stops
    # for the shifted branch name, don't use Shifted as a prefix, or
    # the module will think it's an alias for a branch that's in the tree.
    # appending Shifted will cause it to use FindObjThisEvt
    shiftnm  = dbn
    if (shiftStop):
        shiftnm  = dbn.rstrip(".")
        shiftnm += "Shifted."
    altshiftnm = alttag + shiftnm
    shd = [ ROOT.TSnShiftCalWvDataMod("ShiftDataByStops",
                                      dbn, shiftnm),
            ROOT.TSnShiftCalWvDataMod("ShiftDataByStops{0}".format(alttag),
                                      altdbn, altshiftnm) ]
    shd[0].SetStopPosnName(stopnm)
    shd[1].SetStopPosnName(altstopnm)
    
    # filter out high frequencies
    #fltfcn = ROOT.TF1("fltfcn","gaus(0) + gaus(3)", -1.1, 1.1)
    #fltfcn.SetParameters(1.0, -0.3, 0.1, 1.0, 0.3, 0.1)
    #
    # butterworth filter
    #   par0 = order of filter. bigger = faster cutoff.
    #          positive = low pass, negative = high pass
    #          cutoff slope = 20*par0 dB per decade of freq
    #   par1 = cutoff frequency
    #fltfcn = ROOT.TF1("fltfcn",
    #                  "TMath::Sqrt(1.0/(1.0+TMath::Power(x/[1],2*[0])))",
    #                  1e-5, 1.1)
    #fltfcn.SetParameters(-4, 0.400); # high pass
    #fltfcn.SetParameters(20, 0.180); # low pass
    #fltfcn.SetParameters(20, 0.150); # low pass
    #
    # butterworth bandpass? (i just multiplied a HP and LP together)
    fltfcn = ROOT.TF1("fltfcn",
                      "TMath::Sqrt(1.0/(1.0+TMath::Power(x/[1],2*[0])))"
                      "*TMath::Sqrt(1.0/(1.0+TMath::Power(x/[3],2*[2])))",
                      1e-5, 1.1)
    fltfcn.SetParameters(-10, 0.200, 4, 0.350);
    
    fltfcn.SetNpx(1000)
    flt = [ ROOT.TSnFilterWaveformMod("FilterWaveformMod",
                                     shiftnm,
                                     "FilteredData.",
                                     fltfcn),
           ROOT.TSnFilterWaveformMod("{0}FilterWaveformMod".format(alttag),
                                     altshiftnm,
                                     "{0}FilteredData.".format(alttag),
                                     fltfcn) ]
       
    #flt[0].SetVerbosity(flt[0].kDebugFilter)
    
    #data name (filtered or no)
    cdnm = {}
    if (filt):
        cdnm[0] = flt[0].GetFltDatName().Data()
        cdnm[1] = flt[1].GetFltDatName().Data()
    else:
        cdnm[0] = shiftnm
        cdnm[1] = altshiftnm
    

    fftnm = "{0}FFT".format(shiftnm)
    altfftnm = "{0}FFT".format(altshiftnm)
    fft = [ ROOT.TSnFFTCalcMod("CalcFFTMod", shiftnm, fftnm),
            ROOT.TSnFFTCalcMod("CalcFFTMod", altshiftnm, altfftnm) ]
    
    # fltnm = "FilteredData."
    # altfltnm = "{0}{1}".format(alttag,fltnm)
    # specf = ROOT.TFile("/data/users/cjreed/work/BounceStudy/Stn10/nuTemplate/"
    #                    "graphs.signalTemplates.root")
    # spec = specf.Get("CalFFTData/cfft_E10_H10_c0.00")
    # print spec
    # flt = [ ROOT.TSnApplySpectrumMod("ApplySpecMod", fftnm, fltnm, spec),
    #         ROOT.TSnApplySpectrumMod("ApplySpecMod", altfftnm, altfltnm, spec) ]
    # flt[0].SetVerbosity(flt[0].kDebugSpec)
    # flt[1].SetVerbosity(flt[1].kDebugSpec)
    # cdnm = {}
    # if (filt):
    #     cdnm[0] = fltnm
    #     cdnm[1] = altfltnm
    # else:
    #     cdnm[0] = shiftnm
    #     cdnm[1] = altshiftnm
    
    
    
    # find envelopes
    menv = {}
    if (envnm!=None):
        altenvnm = alttag + envnm
        menv[0] = ROOT.TSnFindWaveformEnvelopeMod("EnvMod",
                                                  cdnm[0], envnm)
        #menv[0].SetVerbosity(menv[0].kDebugEnv)
        menv[1] = ROOT.TSnFindWaveformEnvelopeMod("{0}EnvMod".format(alttag),
                                                  cdnm[1], altenvnm)
        
        
    # determine inter-channel correlations
    ctc = ROOT.TSnChanTimeCorlAltEvtsMod("ChanTimeCorlAltEvts",
                                         cdnm[0], cdnm[1])
    if (envnm!=None):
        ctc.SetCorlType("ScanToEnvMax")
        ctc.SetEnvelopeName(envnm)
        ctc.SetAltEnvelopeName(altenvnm)
    #ctc.SetVerbosity(ctc.kDebugCorls)
    
    # fit to find the best offsets between channels
    fco = ROOT.TSnFitChanOffsetAltEvtsMod(
        "FitChanOffsetsAltEvts",
        ROOT.TSnFitChanOffsetAltEvtsMod.kCorlOnly,
        minimizer, algo)
    fco.SetVerbosity(fco.kMakeScanNtp)
    #fco.SetVerbosity(fco.kDebugFit);
    if (envnm!=None):
        fco.SetEnvelopeName(menv[0].GetEnvDatName().Data())
        fco.SetAltEnvelopeName(menv[1].GetEnvDatName().Data())
        #fco.SetFitType("CorlOnly")
        #fco.SetFitType("CorlAndEnvRiseDist")
        #fco.SetFitType("CorlAndEnvMatch")
        #fco.SetFitType("EnvRiseDistOnly")
    
    # save the results - put before process so hopefully it can be used to
    # cache the output tree(s)
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    scd = ROOT.TSnSaveCorlDataMod("SaveCorlData",
                                  cdnm[0],
                                  "CorlFilt",
                                  "channel correlations from filtered data")
    scd.SetCorrelationsName( fco.GetCorrelationsName().Data() )
    scd.SetAltTreeTag(alttag)
    srr = ROOT.TSnSaveRecoResultsMod("SaveFitResults",
                                     "FitOffsets",
                                     "fit channel offsets");
    srr.SaveRecoResult(sfr[0].GetStopPosnName().Data())
    srr.SaveRecoResult(sfr[1].GetStopPosnName().Data())
    srr.SaveRecoResult(fco.GetTimeOffsetsName().Data())
    
    ree.Add(bes[0])
    bes[0].Add(bes[1])
    bes[1].Add(sfr[0])
    sfr[0].Add(sfr[1])
    if (shiftStop):
        sfr[1].Add(shd[0])
        shd[0].Add(shd[1])
        shd[1].Add(fft[0])
    else:
        sfr[1].Add(fft[0])
    fft[0].Add(fft[1])
    if (filt):
        print "using filter"
        fft[1].Add(flt[0])
        flt[0].Add(flt[1])
        if (envnm!=None):
            flt[1].Add(menv[0])
            menv[0].Add(menv[1])
            menv[1].Add(ctc)
        else:
            flt[1].Add(ctc)
    else:
        print "no filter"
        if (envnm!=None):
            fft[1].Add(menv[0])
            menv[0].Add(menv[1])
            menv[1].Add(ctc)
        else:
            fft[1].Add(ctc)
    ctc.Add(fco)
    fco.Add(scd)
    fco.Add(srr)
    
    print "starting ..."
    sel.AddInput(ree)
    try:
        ROOT.TTree.Process(nt[0], sel)
    except:
        print "exception!"
    # use Proof Lite .. shouldn't do this with TTrees in the output
    #ROOT.TProof.Open("workers=10")
    #nt.SetProof()
    #ROOT.gProof.AddInput(bes)
    #nt.Process("TAMSelector")
    print "process done!"
    
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
    
    


if __name__=="__main__":
    if (len(sys.argv)<5):
        print "Usage: python fitChanOffsetsAltEvts.py [input filename 1 (w/ wildcards)] [input filename 2] [tree type] [output filename] (minimizer) (algorithm) (apply filter True/False) (use envelopes True/False) (alt entry number) (shift stop to end True/False)"
        print "   tree type = 0 for raw, 1 for FPN sub, 2 for calibrated amp out"
        sys.exit()
    
    infn1 = sys.argv[1]
    infn2 = sys.argv[2]
    
    tt = int(sys.argv[3])
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
    
    outfn = sys.argv[4]

    minimizer = "Minuit2"
    algo      = "Migrad"
    if (len(sys.argv)>=7):
        minimizer = sys.argv[5]
        algo      = sys.argv[6]

    filt = True
    if (len(sys.argv)>=8):
        if (sys.argv[7]=='False'):
            filt = False

    envnm = None
    if (len(sys.argv)>=9):
        if (sys.argv[8]=='True'):
            envnm = "{0}Env".format(dbn)
    
    altevt = 387
    if (len(sys.argv)>=10):
        altevt = int(sys.argv[9])
    
    shiftStop = True
    if (len(sys.argv)>=11):
        shiftStop = sys.argv[10]=='True'
    
    fitChanOffsetsAltEvts(ntn, dbn, infn1, infn2, outfn, 
                          minimizer, algo, filt, envnm,
                          altevt, shiftStop)
    


