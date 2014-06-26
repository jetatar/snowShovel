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

    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    # if ... select which trigger
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kForced)
    bes.SetCheckCRC(True)

    fftnm = dbn[:-1]
    fftnm = "{0}FFT.".format(fftnm)
    fft = ROOT.TSnFFTCalcMod("CalcFFTMod", dbn, fftnm)
    
    ctc = ROOT.TSnChanTimeCorlAltEvtsMod("ChanTimeCorlAltEvts",
                                         dbn, dbn)
    
    outf = ROOT.TFile(outfn, "recreate")
    outf.SetCompressionLevel(2)
    outf.cd()

    tb = ROOT.TSnTreeBuilderMod("TreeBuilder", "OutputTree", "TheTree")
    tb.CopyBranchFromTree("TSnEventMetadata")(ROOT.TSnRawTreeMaker.kEMtBrNm)
    tb.CopyBranchFromTree("TSnEventHeader")(ROOT.TSnRawTreeMaker.kEHdBrNm)
    tb.CopyBranchFromTree("TSnCalWvData")(dbn)
    tb.AddBranchForEvtObj("TSnCalFFTData")(fftnm, fftnm)
    tb.AddBranchForEvtObj("TObjArray")(ctc.GetCorrelationsName().Data(), ctc.GetCorrelationsName().Data())

    ree.Add(bes)
    bes.Add(fft)
    fft.Add(ctc)
    ctc.Add(tb)


    print "starting ..."
    sel.AddInput(ree)

    try:
        ROOT.TTree.Process(nt, sel)
    except Exception as exc:
        print exc

    print "process done!"

    output = sel.GetModOutput()
    if (output!=None):
        outf.cd()
        ottb = output.FindOutput("TreeBuilder", "OutputTree")
        ottb.Write(ottb.GetName(), ROOT.TObject.kOverwrite)
        #output.Write()


if __name__=="__main__":

    ntn = ROOT.TSnSaveCalibDataMod.kCalEvtTreeNm
    dbn = ROOT.TSnSaveCalibDataMod.kAmpOutDatBrNm
    
    infn = "/data/euler/arianna_data/2014_InSitu/combinedNoHtBt/CalTree/SingleSidedInvertedCh2Ch3/CalTree.CombNoHtbt.RawTree.SnEvtsM0002F7F0C3B6.root"
    outfn = "/data/euler/arianna_data/scratch/autoCorr.root"

    makeAutoCorrTree(ntn, infn, dbn, outfn)


        
