import os,sys
import ROOT


def makeAutoCorrTree(ntn, infn, dbn, outfn):
    
    nt = ROOT.TChain(ntn)
    nt.Add(infn)

    sel = ROOT.TAMSelector()
    sel.SetVerbosity(11)
    
    # remove pre-triggered events
    cfl = ROOT.TSnConfigTreeLoader()
    sel.AddLoader(cfl)
    ree = ROOT.TSnRejectEarlyEvtsMod("RejectEarlyEvtsMod")
    
    # basic event selection
    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    # if ... select which trigger
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kForced)
    bes.SetCheckCRC(True)
    
    # put FFTs in the tree
    fftnm = dbn[:-1]
    fftnm = "{0}FFT.".format(fftnm)
    fft = ROOT.TSnFFTCalcMod("CalcFFTMod", dbn, fftnm)
    
    # calculate the auto correlations
    # (dbn is the main and alt event branch name)
    ctc = ROOT.TSnChanTimeCorlAltEvtsMod("ChanTimeCorlAltEvts",
                                         dbn, dbn)
    autoCorlName = "AutoCorrelations."
    ctc.SetCorrelationsName(autoCorlName)
    
    # find the pure thermal events from the auto correlations
    fpt = ROOT.TSnFindPureThermalFromAutoCorrMod("FindPureThermal",
                                                 autoCorlName)
    fpt.SetTagBehavior("AddTagToEvent")
    fpt.SetPureThermalTagName("IsPureThermalFromAutoCorr")

    # open output file before TreeBuilderMod just in case the file
    # needs to be used as a cache for the output tree
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()
    
    # make the output tree
    outtreenm = "AutoCorrTree"
    tb = ROOT.TSnTreeBuilderMod("TreeBuilder", outtreenm, 
                                "Tree with auto correlations and pure thm tags")
    tb.CopyBranchFromTree("TSnEventMetadata")(ROOT.TSnRawTreeMaker.kEMtBrNm)
    tb.CopyBranchFromTree("TSnEventHeader")(ROOT.TSnRawTreeMaker.kEHdBrNm)
    tb.CopyBranchFromTree("TSnCalWvData")(dbn)
    tb.AddBranchForEvtObj("TSnCalFFTData")(fftnm, fftnm)
    tb.AddBranchForEvtObj("TObjArray")(ctc.GetCorrelationsName().Data(),
                                       ctc.GetCorrelationsName().Data())
    tb.AddBranchForEvtObj("TParameter<Bool_t>")(
                              fpt.GetPureThermalTagName().Data(),
                              "{0}.".format(fpt.GetPureThermalTagName().Data()))
    
    # build the module hierarchy
    ree.Add(bes)
    bes.Add(fft)
    fft.Add(ctc)
    ctc.Add(fpt)
    fpt.Add(tb)

    
    print "starting ..."
    sel.AddInput(ree)

    try:
        ROOT.TTree.Process(nt, sel)
        #ROOT.TTree.Process(nt, sel, "", 1000)
    except Exception as exc:
        print exc

    print "process done!"
    
    
    # save the output
    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        ottb = output.FindOutput(tb.GetName(), tb.GetOutTreeName().Data())
        ottb.Write(ottb.GetName(), ROOT.TObject.kOverwrite)
        #output.Write()


if __name__=="__main__":

    #infn = "/data/euler/arianna_data/2014_InSitu/combinedNoHtBt/CalTree/SingleSidedInvertedCh2Ch3/CalTree.CombNoHtbt.RawTree.SnEvtsM0002F7F0C3B6.root"
    #outfn = "/data/euler/arianna_data/scratch/autoCorr.root"
    
    if (len(sys.argv)<3):
        print "Usage: python makeAutoCorrTree.py [input filename] "\
            "[output file name] (tree type)"
        print "  tree type = 0 for raw, 1 for FPN sub, 2 for calibrated amp out"
        print "              default is type 2"
        sys.exit()
        
    infn  = sys.argv[1]
    outfn = sys.argv[2]
    
    tt = 2
    if (len(sys.argv)>=4):
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
    
    
    makeAutoCorrTree(ntn, infn, dbn, outfn)


        
