import os,sys
import ROOT

def makePeakPlotsSignalTmpl(ntn, dbn, ampunits, infn, outfn,
                            doHiThr, hiThr, doLoThr, loThr, plotSuffix,
                            lscN=3, lscM=2):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)

    sel = ROOT.TAMSelector()
    
    # obtain smooth fits (light low pass filter) to the waveform
    # and also its derivative
    intpnm  = dbn.rstrip(".")
    intpnm += "Smoothed."
    lsc = ROOT.TSnSmoothDataLstSqrCnvMod("SmoothDataMod",
                                         dbn, intpnm,
                                         lscN,lscM,0)
    lsc.SetAssumeAlwaysTree()
    
    dervnm  = dbn.rstrip(".")
    dervnm += "Deriv."
    lsd = ROOT.TSnSmoothDataLstSqrCnvMod("SmoothDataMod",
                                         dbn, dervnm,
                                         lscN,lscM,1)
    lsd.SetAssumeAlwaysTree()
    
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
    
    lsc.Add(lsd)
    lsd.Add(fpm)
    fpm.Add(ppm)
    
    print "starting ..."
    sel.SetVerbosity(10)
    sel.AddInput(lsc)
    ROOT.TTree.Process(nt, sel)
    print "process done!"
    
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        output.Write()
    
    


if __name__=="__main__":
    
    if (len(sys.argv)<7):
        print "Usage: python makePeakPlots.py [input file name(s)] "\
            "[output file name] "\
            "[find high peaks True/False] [high side threshold] "\
            "[find low peaks True/False] [low side threshold] "\
            "(plot name suffix)"
        sys.exit(-1)
    
    infn = sys.argv[1]
    
    ntn = "Templates"
    dbn = "wave."
    ampunits = "mV^{*}"

    outfn = sys.argv[2]
    
    doHiThr = sys.argv[3]=="True"
    hiThr   = float(sys.argv[4])
    
    doLoThr = sys.argv[5]=="True"
    loThr   = float(sys.argv[6])
    
    plotSuffix=None
    if (len(sys.argv)>7):
        plotSuffix = sys.argv[7]
    
    makePeakPlotsSignalTmpl(ntn, dbn, ampunits, infn, outfn,
                            doHiThr, hiThr, doLoThr, loThr, plotSuffix)
