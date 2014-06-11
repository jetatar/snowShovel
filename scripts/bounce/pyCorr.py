import os, sys
import scipy, numpy, array
import scipy.stats

import ROOT
ROOT.gSystem.Load("$SNS/lib/libutil")

NCHANS = 4
NSAMPS = 128

rtfn = "/home/cjreed/work/DATA/2013_InSitu/runtrees/0002F7F0C41C"\
    "/r00015/s00000/nt*root"
FPNfn = "~/work/PEDS/peds.M0002F7F0C41Cr00035.root"
outfn = "pycorr.r15.root"

nt = ROOT.TChain("runtree")
nt.Add(rtfn)

nents = nt.GetEntries()

fpnf = ROOT.TFile.Open(FPNfn)
gPed = []
gNoise = []
for ch in range(0, NCHANS):
    gPed.append( fpnf.Get("gExlPed_ch{0}".format(ch)) )
    gNoise.append( fpnf.Get("gExlNoise_ch{0}".format(ch)) )

outf = ROOT.TFile.Open(outfn,"recreate")

ev=0
for e in nt:
    ps = [[],[],[],[]]
    ps[0] = array.array('d',e.data00)
    ps[1] = array.array('d',e.data01)
    ps[2] = array.array('d',e.data02)
    ps[3] = array.array('d',e.data03)
    for ch in range(0, NCHANS):
        if (ch==0):
            print "["
        for i in range(0,NSAMPS):
            ps[ch][i] -= gPed[ch].GetY()[i]
            if (ch==0):
                sys.stdout.write("{0}, ".format(ps[ch][i]))
        if (ch==0):
            print "]"
    ev+=1
    if (ev>1):
        break
    
        
psp = scipy.stats.pearsonr(ps[0],ps[1])
pst = ROOT.TSnSpectral.PearsonSmpCorrCoef(ps[0],ps[1],NSAMPS)

print "psp={0}, pst={1}".format(psp,pst)

g1 = ROOT.TSnSpectral.NewScanPrsCorrCoefGraph(ps[0],ps[1],NSAMPS)
g2 = ROOT.TSnSpectral.NewScanPrsCorrCoefGraph(ps[0],ps[1],NSAMPS, 20,59, True)
g3 = ROOT.TSnSpectral.NewScanPrsCorrCoefGraph(ps[0],ps[1],NSAMPS, 20,59, False)
g4 = ROOT.TSnSpectral.NewScanPrsCorrCoefGraph(ps[0][20:],ps[1][20:],40)

c1 = ROOT.TCanvas("c1","c1",800,700)
c1.Divide(2,2)
c1.cd(1)
g1.Draw("apl")
c1.cd(2)
g2.Draw("apl")
c1.cd(3)
g3.Draw("apl")
c1.cd(4)
g4.Draw("apl")
c1.cd()
c1.Update()
