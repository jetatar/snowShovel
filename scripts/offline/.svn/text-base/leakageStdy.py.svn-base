import os, sys
import ROOT

nchans = 4
nsamps = 128

nadcs = 101
adcmin=-50.5
adcmax= 50.5

if (len(sys.argv)!=4):
    print "Usage: leakageStdy.py [new runtree] [new pedtrack] [old pedtrack]"
    sys.exit(0)

infn = sys.argv[1]
inf = ROOT.TFile.Open(infn)
nt = inf.Get("runtree")
if (nt==None):
    print "runtree not found in file {0}".format(infn)
    sys.exit(0)

pedfn = sys.argv[2]
pedf = ROOT.TFile.Open(pedfn)

opedfn = sys.argv[3]
opedf = ROOT.TFile.Open(opedfn)

newpeds = {}
oldpeds = {}
newrmss = {}
oldrmss = {}
pdiffs = {}
hists = {}
for i in range(0, nchans):
    newpeds[i] = pedf.Get("gExlPed_ch{0}".format(i))
    newrmss[i] = pedf.Get("gExlRms_ch{0}".format(i))
    if (newpeds[i]==None or newrmss[i]==None):
        print "ped or rms ch{0} not found in file {1}".format(i,pedfn)
        sys.exit(0)
    oldpeds[i] = opedf.Get("gExlPed_ch{0}".format(i))
    oldrmss[i] = opedf.Get("gExlRms_ch{0}".format(i))
    if (oldpeds[i]==None or oldrmss[i]==None):
        print "ped or rms ch{0} not found in file {1}".format(i,pedfn)
        sys.exit(0)
    hists[i] = ROOT.TH2F("hPedSubVsSampCh{0}".format(i),
                    "ADC - FPN vs sample (chan {0});sample;ADC-FPN".format(i),
                    nsamps, -0.5, nsamps-0.5,
                    nadcs, adcmin, adcmax)
    nx = newpeds[i].GetN()
    pdiffs[i] = ROOT.TGraphErrors(nx)
    pdiffs[i].SetMarkerStyle(7)
    pdiffs[i].SetName("pdiff_ch{0}".format(i))
    pdiffs[i].SetTitle("new ped - old ped vs sample "
                       "(errors = average of new & old noise)"
                       ";sample;new - old (ADC)")
    for s in range(0, nx):
        pd = (newpeds[i].GetY())[s] - (oldpeds[i].GetY())[s]
        en = (newrmss[i].GetY())[s]
        eo = (oldrmss[i].GetY())[s]
        #pe = ROOT.TMath.Sqrt( (en*en) + (eo*eo) )
        pe = (en+eo)/2.0
        pdiffs[i].SetPoint(s, (newpeds[i].GetX())[s], pd)
        pdiffs[i].SetPointError(s, 0, pe)
        

nevts = nt.GetEntries()

for e in range(0, nevts):
    nt.GetEntry(e)
    
    for s in range(0, nsamps):
        hists[0].Fill( s, nt.data00[s] - ((newpeds[0].GetY())[s]) )
        hists[1].Fill( s, nt.data01[s] - ((newpeds[1].GetY())[s]) )
        hists[2].Fill( s, nt.data02[s] - ((newpeds[2].GetY())[s]) )
        hists[3].Fill( s, nt.data03[s] - ((newpeds[3].GetY())[s]) )

c1 = ROOT.TCanvas("c1","c1",700,600)
c1.Divide(2,2)
for i in range(0, nchans):
    c1.cd(i+1)
    hists[i].Draw("colz")
c1.cd()
c1.Update()

c2 = ROOT.TCanvas("c2","c2",700,600)
c2.Divide(2,2)
for i in range(0, nchans):
    c2.cd(i+1)
    pdiffs[i].Draw("ap")
c2.cd()
c2.Update()

c3 = ROOT.TCanvas("c3","c3",700,600)
c3.Divide(2,2)
for i in range(0, nchans):
    c3.cd(i+1)
    for s in range(0, nsamps):
        oldpeds[i].SetPointError(s, 0, (oldrmss[i].GetY())[s])
        newpeds[i].SetPointError(s, 0, (newrmss[i].GetY())[s])
    oldpeds[i].SetMarkerColor(ROOT.kAzure-2)
    oldpeds[i].SetLineColor(ROOT.kAzure-2)
    oldpeds[i].SetHistogram(0)
    oldpeds[i].SetTitle(" (errs = noise)")
    oldpeds[i].Draw("ap")
    newpeds[i].SetMarkerColor(ROOT.kOrange+7)
    newpeds[i].SetLineColor(ROOT.kOrange+7)
    newpeds[i].SetHistogram(0)
    newpeds[i].SetTitle("{0} (errs = noise)"\
                            .format(newpeds[i].GetTitle()))
    newpeds[i].Draw("p")
    if (i==0):
        leg = ROOT.TLegend(.856,.824,.994,.995)
        leg.SetFillColor(0)
        leg.SetBorderSize(1)
        leg.AddEntry(oldpeds[i],"Old","pl")
        leg.AddEntry(newpeds[i],"New","pl")
        leg.Draw()
    oldpeds[i].SetTitle("{0} (errs = noise)"\
                            .format(oldpeds[i].GetTitle()))
c3.cd()
c3.Update()
