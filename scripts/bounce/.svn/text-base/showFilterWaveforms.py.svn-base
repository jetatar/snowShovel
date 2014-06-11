import os,sys
import ROOT

def showFilterWaveforms(nt):
    
    sel = ROOT.TAMSelector()
    
    bes = ROOT.TSnBasicEvtSelMod("BasicEvtSelMod")
    bes.GetTrgBits().EnableTrig(ROOT.TSnTrgInfo.kThermal)
    bes.SetCheckCRC(True)
    
    fltfcn = ROOT.TF1("fltfcn","gaus(0) + gaus(3)", -1.1, 1.1)
    fltfcn.SetNpx(1000)
    fltfcn.SetParameters(1.0, -0.3, 0.1, 1.0, 0.3, 0.1)
    flt = ROOT.TSnFilterWaveformMod("FilterWaveformMod",
                                    ROOT.TSnSaveCalibDataMod.kFPNSubDatBrNm,
                                    "FilteredFpnSub",
                                    fltfcn)
    # set verbosity really high so that the module will display plots each event
    flt.SetVerbosity(ROOT.TSnFilterWaveformMod.kDebugFilter)
    
    bes.Add(flt)
    
    sel.AddInput(bes)
    
    print "starting ..."
    nt.Process(sel)
    print "process done!"
    
    
    


if __name__=="__main__":
    if (len(sys.argv)<3):
        print "Usage: python showFilterWaveforms.py [input filename (w/ wildcards)] [tree type]"
        print "   tree type = 0 for raw, 1 for FPN sub, 2 for calibrated amp out"

    infn = sys.argv[1]
    
    tt = int(sys.argv[2])
    nt = None
    if (tt==0):
        nt = ROOT.TChain(ROOT.TSnRawTreeMaker.kETname)
    elif (tt==1):
        nt = ROOT.TChain(ROOT.TSnSaveCalibDataMod.kFpnEvtTreeNm)
    elif (tt==2):
        nt = ROOT.TChain(ROOT.TSnSaveCalibDataMod.kFpnEvtTreeNm)
    else:
        print "Unknown tree type [{0}]".format(tt)
        sys.exit()
    
    nt.Add(infn)
    showFilterWaveforms(nt)
    


