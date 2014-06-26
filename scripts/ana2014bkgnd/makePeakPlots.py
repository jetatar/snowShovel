import os,sys
import ROOT

def makePeakPlots(ntn, dbn, ampunits, infn, outfn,
                  stpshft, doHiThr, hiThr, doLoThr, loThr, plotSuffix,
                  useForced,
                  lscN=3, lscM=2):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)

    sel = ROOT.TAMSelector()
    
    # remove pre-triggered events
    cfl = ROOT.TSnConfigTreeLoader()
    sel.AddLoader(cfl)
    ree = ROOT.TSnRejectEarlyEvtsMod("RejectEarlyEvtsMod")
    
    # apply basic event selection
    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    if (useForced):
        bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kForced)
    else:
        bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.SetCheckCRC(True)
    
    # find the stop
    sfr = ROOT.TSnStopFindRms("StopFindRms", dbn)
    stopnm = "{0}.".format(sfr.GetStopPosnName().Data())
    sfr.SetStopPosnName(stopnm)

    # (maybe) shift by the stop
    shiftnm = None
    shd = None
    if (stpshft):
        shiftnm  = dbn.rstrip(".")
        shiftnm += "Shifted."
        shd = ROOT.TSnShiftCalWvDataMod("ShiftDataByStops", dbn, shiftnm)
        shd.SetStopPosnName(stopnm)
    else:
        shiftnm = dbn
    

    # obtain smooth fits (light low pass filter) to the waveform
    # and also its derivative
    intpnm  = shiftnm.rstrip(".")
    intpnm += "Smoothed."
    lsc = ROOT.TSnSmoothDataLstSqrCnvMod("SmoothDataMod",
                                         shiftnm, intpnm,
                                         lscN,lscM,0)
    dervnm  = shiftnm.rstrip(".")
    dervnm += "Deriv."
    lsd = ROOT.TSnSmoothDataLstSqrCnvMod("SmoothDataMod",
                                         shiftnm, dervnm,
                                         lscN,lscM,1)
    
    # find the peaks in the graph
    fpm = ROOT.TSnFindPeaksPastThreshMod("FindPeaksMod",
                                         intpnm, dervnm)
    fpm.SetFindHighSidePeaks(doHiThr)
    fpm.SetHighThreshold(hiThr)
    fpm.SetFindLowSidePeaks(doLoThr)
    fpm.SetLowThreshold(loThr)
    
    #fpm.SetVerbosity(fpm.kDebugPlots)
    #fpm.SetVerbosity(fpm.kDebugPeakFind)

    # make the plots
    ppm = ROOT.TSnPlotWvfmPeaksMod("PlotPeaksMod",
                                   ampunits, lsc.IsXaxisTime(),
                                   fpm.GetPeaksArrayName().Data(),
                                   plotSuffix)
    ppm.SetMakePeakPropsTree(True)
    
    outf = ROOT.TFile(outfn,"recreate")
    outf.cd()
    
    ree.Add(bes)
    bes.Add(sfr)
    if (stpshft):
        sfr.Add(shd)
        shd.Add(lsc)
    else:
        sfr.Add(lsc)
    lsc.Add(lsd)
    lsd.Add(fpm)
    fpm.Add(ppm)
    
    print "starting ..."
    sel.SetVerbosity(10)
    sel.AddInput(ree)
    ROOT.TTree.Process(nt, sel)
    print "process done!"
    
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
    
    


if __name__=="__main__":
    
    if (len(sys.argv)<9):
        print "Usage: python makePeakPlots.py [input file name(s)] "\
            "[tree type] [output file name] [shift stops True/False] "\
            "[find high peaks True/False] [high side threshold] "\
            "[find low peaks True/False] [low side threshold] "\
            "(plot name suffix) (use forced trg instead of thermal True/False)"
        print "   tree type = 0 for raw, 1 for FPN sub, 2 for calib amp out"
        sys.exit(-1)
    
    infn = sys.argv[1]
    
    tt = int(sys.argv[2])
    ntn = None
    dbn = None
    ampunits = "ADC"
    if (tt==0):
        ntn = ROOT.TSnRawTreeMaker.kETname
        dbn = ROOT.TSnRawTreeMaker.kEWvBrNm
    elif (tt==1):
        ntn = ROOT.TSnSaveCalibDataMod.kFpnEvtTreeNm
        dbn = ROOT.TSnSaveCalibDataMod.kFPNSubDatBrNm
    elif (tt==2):
        ntn = ROOT.TSnSaveCalibDataMod.kCalEvtTreeNm
        dbn = ROOT.TSnSaveCalibDataMod.kAmpOutDatBrNm
        ampunits = "mV"
    else:
        print "Unknown tree type [{0}]".format(tt)
        sys.exit()

    outfn = sys.argv[3]
    
    stpshft = sys.argv[4]=="True"
    
    doHiThr = sys.argv[5]=="True"
    hiThr   = float(sys.argv[6])
    
    doLoThr = sys.argv[7]=="True"
    loThr   = float(sys.argv[8])
    
    plotSuffix=None
    if (len(sys.argv)>9):
        plotSuffix = sys.argv[9]
        
    useForced=False
    if (len(sys.argv)>10):
        useForced = sys.argv[10]=="True"
    
    makePeakPlots(ntn, dbn, ampunits, infn, outfn,
                  stpshft, doHiThr, hiThr, doLoThr, loThr, plotSuffix, 
                  useForced)
