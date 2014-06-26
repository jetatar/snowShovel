import os,sys
import ROOT


if (len(sys.argv)<4):
    print "Usage: python treeBuilderExample.py [raw tree filename] "\
        "[FPN filename] [output filename]"
    sys.exit(-1)

rawtfn    = sys.argv[1]
fpnFilen  = sys.argv[2]
outfilen  = sys.argv[3]

inf = ROOT.TFile(rawtfn)
rawt = inf.Get(ROOT.TSnRawTreeMaker.kETname)
if (rawt!=None):
    sel = ROOT.TAMSelector()
    sel.SetVerbosity(10)
    
    loadfpn = ROOT.TSnLoadFromFileMod("LoadFPN", fpnFilen, "FPNSet")
    fpnsub = ROOT.TSnFPNSubMod("FPNSubMod", "FPNSet")
    tbmod = ROOT.TSnTreeBuilderMod("TreeBuilder","nt","example fpn sub tree")
    # only works for the branch types that have been defined in sigprocLinkdef.h
    tbmod.CopyBranchFromTree('TSnEventMetadata')(ROOT.TSnRawTreeMaker.kEMtBrNm)
    tbmod.CopyBranchFromTree('TSnEventHeader')(ROOT.TSnRawTreeMaker.kEHdBrNm)
    tbmod.AddBranchForEvtObj('TSnCalWvData')( fpnsub.GetCalDatName().Data(),
                                              "MyFpnSubBranch." )
    
    loadfpn.Add(fpnsub)
    fpnsub.Add(tbmod)
    
    sel.AddInput(loadfpn)
    
    ROOT.TTree.Process(rawt, sel)
    
    output = sel.GetModOutput()
    if (output!=None):
        
        outf = ROOT.TFile(outfilen, "recreate")
        outf.cd()
        output.Write()
        
    else:
        print "Could not get output"

else:
    print "Could not get raw tree from [{0}]".format(rawtfn)

