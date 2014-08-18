import os,sys
import ROOT

def fitAngFromCrossChanDelays(ntn, dbn,
                              infn,
                              geofn, geon,
                              chofffn, choff,
                              outfn,
                              minimizer, algo,
                              filt, envnm, dostpsh, wvdenoise):
    print "tree name = {0}".format(ntn)
    print "data branch = {0}".format(dbn)
    print "input = {0}".format(infn)
    print "geo file = {0}".format(geofn)
    print "geo name = {0}".format(geon)
    print "chan offset file = {0}".format(chofffn)
    print "chan offset name = {0}".format(choff)
    print "output file = {0}".format(outfn)
    print "minimizer = {0}".format(minimizer)
    print "algorithm = {0}".format(algo)
    print "filter = {0}".format(filt)
    print "envelope = {0}".format(envnm)
    print "stop shift = {0}".format(dostpsh)
    print "wavelet denoise = {0}".format(wvdenoise)
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)

    sel = ROOT.TAMSelector()

    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.SetCheckCRC(True)

    sfr = ROOT.TSnStopFindRms("StopFindRms", dbn)
    stopnm = "{0}.".format(sfr.GetStopPosnName().Data())
    sfr.SetStopPosnName(stopnm)
    # keep the stops the same in all channels
    sfr.SetStopWidth(6)
    sfr.SetSearchWidth(6)
    sfr.SetMaxJitter(0)
    
    shiftnm  = dbn
    if (dostpsh):
        shiftnm  = shiftnm.rstrip(".")
        shiftnm += "Shifted."
    shd = ROOT.TSnShiftCalWvDataMod("ShiftDataByStops", dbn, shiftnm)
    shd.SetStopPosnName(stopnm)
    
    denoisenm = shiftnm
    if (wvdenoise):
        denoisenm  = denoisenm.rstrip(".")
        denoisenm += "Denoise."
    wvd = ROOT.TSnWaveletDenoiseMod("WaveletDenoiseMod",
                                    shiftnm, denoisenm)
    wvd.SetWavelet("TSnWaveletDaub20")
    wvd.SetThreshType(wvd.kSoftThresh)
    
    
    # filter out high frequencies
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
    fltfcn.SetParameters(-10, 0.200, 6, 0.350);
    
    fltfcn.SetNpx(1000)
    flt = ROOT.TSnFilterWaveformMod("FilterWaveformMod",
                                    denoisenm,
                                    "FilteredData.",
                                    fltfcn)
    cdnm = flt.GetFltDatName().Data()
    if (filt==False):
        cdnm = denoisenm

    # find envelopes
    menv = None
    if (envnm!=None):
        menv = ROOT.TSnFindWaveformEnvelopeMod("EnvMod", cdnm, envnm)
        
    
    # determine inter-channel correlations
    ctc = ROOT.TSnChanTimeCorlMod("ChanTimeCorl",
                                  cdnm)
    if (envnm!=None):
        ctc.SetEnvelopeName(envnm)
        ctc.SetCorlType("ScanToEnvMax")
    
    # fit to find the best offsets between channels
    #fco = ROOT.TSnFitChanOffsetMod("FitChanOffsets",
    #                               minimizer, algo)
    #fco.SetCorlConstraintSigma(1.)
    
    # load the geometry from a file
    ldgeo = ROOT.TSnLoadFromFileMod("LoadGeoMod",
                                    geofn, geon)
    
    # load the a-priori known channel offsets from a file
    ldoff = ROOT.TSnLoadFromFileMod("LoadOffsetsMod",
                                    chofffn, choff)
    
    # fit the plane wave angle given the channel delays in the tree
    afs = ROOT.TSnAngleFromChanCorlMod("AngleFromChanCorlMod",
                                       minimizer, algo)
    afs.SetChanOffsetsName(choff)
    afs.SetGeometryName(geon)
    
    # save the results - put before process so hopefully it can be used to
    # cache the output tree(s)
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    
    scd = ROOT.TSnSaveCorlDataMod("SaveCorlData",
                                  cdnm,
                                  "CorlFilt",
                                  "channel correlations from filtered data")
    sro = ROOT.TSnSaveRecoResultsMod("SaveDtFitResults",
                                     "FitOffsets",
                                     "fit channel offsets");
    sro.SaveRecoResult(sfr.GetStopPosnName().Data())
    #sro.SaveRecoResult(fco.GetTimeOffsetsName().Data())
    srr = ROOT.TSnSaveRecoResultsMod("SaveAngFitResults",
                                     "FitAngle",
                                     "fit angle from single chan delays");
    srr.SaveRecoResult(afs.GetRecoAngleName().Data())
    srr.SaveRecoResult(afs.GetRecoDTsName().Data())
    
    
    bes.Add(sfr)
    mod = sfr
    if (dostpsh):
        print "Shifting for stops"
        sfr.Add(shd)
        mod = shd
    else:
        print "Not shifting for stops"
    if (wvdenoise):
        print "denoising data"
        mod.Add(wvd)
        mod = wvd
    else:
        print "Not denoising data"
    if (filt):
        print "filtering data"
        mod.Add(flt)
        if (envnm!=None):
            print "finding envelopes"
            flt.Add(menv)
            menv.Add(ctc)
        else:
            flt.Add(ctc)
    else:
        print "not filtering"
        if (envnm!=None):
            print "finding envelopes"
            mod.Add(menv)
            menv.Add(ctc)
        else:
            mod.Add(ctc)
    #ctc.Add(fco)
    ctc.Add(ldgeo)
    
    if (len(chofffn)>0):
        # have channel offsets
        print "using channel offsets"
        ldgeo.Add(ldoff)
        ldoff.Add(afs)
    else:
        # no channel offsets
        print "no channel offsets"
        ldgeo.Add(afs)
    afs.Add(scd)
    afs.Add(sro)
    afs.Add(srr)
    

    sel.SetVerbosity(10)
    bes.SetAllVerbosity(11)
    # uncomment and the module will display plots each event
    #flt.SetVerbosity(ROOT.TSnFilterWaveformMod.kDebugFilter)
    #menv.SetVerbosity(menv.kDebugEnv)
    #ctc.SetVerbosity(ctc.kDebugCorls)
    #fco.SetVerbosity(fco.kDebugCorlFit)
    afs.SetVerbosity(afs.kDrawParSpace)

    print "starting ..."
    sel.AddInput(bes)
    try:
        ROOT.TTree.Process(nt, sel)
        #ROOT.TTree.Process(nt, sel, "", 1)
    except Exception as e:
        print "exception!"
        print e
    print "process done!"
    
    # save the results
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
        if (filt):
            fltfcn.Write()
    

if __name__=="__main__":
    
    if (len(sys.argv)<7):
        print "usage: python fitAngFromCrossChanDelays.py [input filename] "\
            "[tree type] [geo filename] [geo name] [chan offset filename] "\
            "[output filename] (minimizer) (algorithm)"\
            "(apply filter True/False)  (use envelopes True/False) "\
            "(shift for stop True/False) (denoise data True/False)"
        print "   tree type = 0 for raw, 1 for FPN sub, 2 for calibrated amp out"
        sys.exit()
        
    
    infn    = sys.argv[1]

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

    geofn   = sys.argv[3]
    geon    = sys.argv[4]
    chofffn = sys.argv[5]
    outfn   = sys.argv[6]
    
    
    minimizer = "Genetic"#"Minuit2"
    algo      = ""#"Migrad"
    if (len(sys.argv)>=9):
        minimizer = sys.argv[7]
        algo      = sys.argv[8]

    filt = True
    if (len(sys.argv)>=10):
        if (sys.argv[9]=='False'):
            filt = False

    envnm = None
    if (len(sys.argv)>=11):
        if (sys.argv[10]=='True'):
            dd = dbn.rstrip(".")
            envnm = "{0}Env".format(dd)
            
    dostpsh = False
    if (len(sys.argv)>=12):
        dostpsh = sys.argv[11] == "True"
    
    wvdenoise = False
    if (len(sys.argv)>=13):
        wvdenoise = sys.argv[12] == "True"

    fitAngFromCrossChanDelays(ntn, dbn,
                              infn,
                              geofn,geon,
                              chofffn, "ChanTimeOffsets",
                              outfn,
                              minimizer, algo,
                              filt, envnm, dostpsh, wvdenoise)

