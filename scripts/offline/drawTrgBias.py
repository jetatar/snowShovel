import os, sys
import ROOT

if (len(sys.argv)<2):
    print 'No input file specified.'
    sys.exit(0)

if (len(sys.argv)<3):
    print 'No output file specified.'
    sys.exit(0)

NCHANS = 4
NSAMPS = 128
THRM_TRG_BITMASK = 1
FORC_TRG_BITMASK = 2

kIsPedSub = True
if (len(sys.argv)>3):
    kIsPedSub = sys.argv[3] == "True"
    print "kIsPedSub = {0}".format(kIsPedSub)

adcbins = 1200
adcmin  = -600
adcmax  = 600

#zmin = -160
#zmax = 160
zmin = -250
zmax = 250

cbins = 400
cmin = 0
cmax = 400

if not kIsPedSub:
    adcbins = 4096
    adcmin  = -0.5
    adcmax  = 4095.5

    zmin = 1400
    zmax = 1800

    cbins = 400
    cmin  = 1500
    cmax  = 1900

infn = sys.argv[1]
inf  = ROOT.TFile.Open(infn)

outfn = sys.argv[2]
outf  = ROOT.TFile.Open(outfn, "recreate")

plots={}
store=[]

cols = [ROOT.kOrange+10, ROOT.kAzure-7]

def getDumHistForLegend(hist):
   #h = copy(hist)
   #h.SetName("{0}_leg{1}".format(hist.GetName(),gPad.GetName()))
   h = ROOT.TH1F("{0}_leg{1}".format(hist.GetName(),
                                     ROOT.gPad.GetName()),"",1,0,1)
   h.SetFillColor(hist.GetLineColor())
   h.SetLineColor(ROOT.kWhite)
   h.SetMarkerColor(ROOT.kWhite)
   h.SetMarkerStyle(hist.GetMarkerStyle())
   h.SetMarkerSize(1)
   plots[h.GetName()] = h
   return h

nt = inf.Get("nt")

hAdcs = {}
hForced = {}
hMaxAdcs = {}
hMinAdcs = {}
hCumAdcs = {}
hCumMaxAdcs = {}
hCumMinAdcs = {}
hTrgCorr = {}
hTrgCorrMin = {}
hAdcGausDev = {}
hForcedGausDev = {}
hCorrDist = {}

outf.cd()

for ch in range(0,NCHANS):
    hAdcs[ch] = ROOT.TH1F("hAdcs_ch{0}".format(ch),
                          "ADC distribution (therm trg);ADC",
                          adcbins, adcmin, adcmax)
    hForced[ch] = ROOT.TH1F("hForced_ch{0}".format(ch),
                          "min bias ADC distribution (forced trg);ADC",
                          adcbins, adcmin, adcmax)
    hMaxAdcs[ch] = ROOT.TH1F("hMaxAdcs_ch{0}".format(ch),
                             "distribution of max ADC in event (therm trg);ADC",
                             adcbins, adcmin, adcmax)
    hMinAdcs[ch] = ROOT.TH1F("hMinAdcs_ch{0}".format(ch),
                             "distribution of min ADC in event (therm trg);ADC",
                             adcbins, adcmin, adcmax)
    hCumAdcs[ch] = ROOT.TH1F("hCumAdcs_ch{0}".format(ch),
                             "ADC cdf (therm trg);ADC",
                          adcbins, adcmin, adcmax)
    hCumMaxAdcs[ch] = ROOT.TH1F("hCumMaxAdcs_ch{0}".format(ch),
                             "cdf of max ADC in event (therm trg);ADC",
                             adcbins, adcmin, adcmax)
    hCumMinAdcs[ch] = ROOT.TH1F("hCumMinAdcs_ch{0}".format(ch),
                             "cdf of min ADC in event (therm trg);ADC",
                             adcbins, adcmin, adcmax)
    hAdcs[ch].Sumw2()
    hForced[ch].Sumw2()
    hMaxAdcs[ch].Sumw2()
    hMinAdcs[ch].Sumw2()
    for xc in range(0, ch):
        hTrgCorr[(ch,xc)] = ROOT.TH2F("hTrgCorr_ch{0}_ch{1}".format(ch,xc),
                                      "max ADC correlations (ch {0}, ch {1});"
                                      "max ADC ch{0}; max ADC ch{1}".format(
                ch, xc),
                                      cbins, cmin, cmax,
                                      cbins, cmin, cmax)
                          

i=0
n=nt.GetEntries()
for entry in nt:
    if ( (i % 200)==0 ):
        sys.stderr.write("Processing {0}/{1} ...                  \r"
                         .format(i,n))
    if ( (entry.extra10 & FORC_TRG_BITMASK)!=0 ):
        p = [entry.ps00, entry.ps01, entry.ps02, entry.ps03]
        for ch in range(0, NCHANS):
            for s in range(0, NSAMPS):
                hForced[ch].Fill(p[ch][s])
        
    if ( (entry.extra10 & THRM_TRG_BITMASK)!=0 ):
        p = [entry.ps00, entry.ps01, entry.ps02, entry.ps03]
        x = [entry.x0, entry.x1, entry.x2, entry.x3]
        for ch in range(0, NCHANS):
            for s in range(0, NSAMPS):
                hAdcs[ch].Fill(p[ch][s])
                if (x[ch]>-1):
                    hMaxAdcs[ch].Fill(p[ch][x[ch]])
            for xc in range(0, ch):
                hTrgCorr[(ch,xc)].Fill(p[ch][x[ch]], p[xc][x[xc]])
        
    i+=1

print "\rDone."

for ch in range(0, NCHANS):
    hAdcs[ch].ComputeIntegral()
    hCumAdcs[ch].SetContent(hAdcs[ch].GetIntegral())
    hMaxAdcs[ch].ComputeIntegral()
    hCumMaxAdcs[ch].SetContent(hMaxAdcs[ch].GetIntegral())
    #hAdcs[ch].Scale(1.0/hAdcs[ch].Integral())
    #hMaxAdcs[ch].Scale(1.0/hMaxAdcs[ch].Integral())
    hAdcs[ch].Scale(1.0/hAdcs[ch].GetMaximum())
    hMaxAdcs[ch].Scale(1.0/hMaxAdcs[ch].GetMaximum())

c1 = ROOT.TCanvas("c1","c1",700,600)
c1.Divide(2,2)
c2 = ROOT.TCanvas("c2","c2",700,600)
c2.Divide(2,2)
for ch in range(0, NCHANS):
    pad = c1.GetPad(ch+1)
    pad.cd()
    pad.SetLogy()
    hAdcs[ch].SetLineColor(cols[0])
    hCumAdcs[ch].SetLineColor(cols[1])
    #hCumAdcs[ch].Draw()
    #hAdcs[ch].Draw("same")
    hAdcs[ch].Draw()
    hAdcs[ch].GetXaxis().SetRangeUser(zmin, zmax)
    hAdcs[ch].Fit("gaus")

    pad = c2.GetPad(ch+1)
    pad.cd()
    pad.SetLogy()
    hMaxAdcs[ch].SetLineColor(cols[0])
    hCumMaxAdcs[ch].SetLineColor(cols[1])
    hCumMaxAdcs[ch].Draw()
    hMaxAdcs[ch].Draw("same hist")
    hCumMaxAdcs[ch].GetXaxis().SetRangeUser(0,zmax)

c1.Update()
c2.Update()

def drawSigLine(h, nsig, col):
    fit = h.GetFunction("gaus")
    sig = fit.GetParameter(2)
    line = ROOT.TLine(nsig*sig, ROOT.TMath.Power(10.,pad.GetUymin()),
                      nsig*sig, ROOT.TMath.Power(10.,pad.GetUymax()))
    line.SetLineColor(col)
    line.SetLineStyle(7)
    line.Draw()
    store.append(line)
    

for ch in range(0, NCHANS):
    pad = c1.GetPad(ch+1)
    pad.cd()

    drawSigLine(hAdcs[ch], 5., ROOT.kOrange+7)

    hForced[ch].Fit("gaus","0")
    drawSigLine(hForced[ch], 5., ROOT.kAzure-7)

    
c1.cd()
c1.Update()
c2.cd()
c2.Update()


c3 = ROOT.TCanvas("c3","c3",700,600)
c3.Divide(2,3)
i=0
for ch in range(0, NCHANS):
    for xc in range(0, ch):
        i+=1
        c3.cd(i)
        pad = c3.GetPad(i)
        hTrgCorr[(ch,xc)].Draw("colz")
        pad.SetLogz()
c3.cd()
c3.Update()

c4 = ROOT.TCanvas("c4","c4",700,600)
c4.Divide(2,2)
for ch in range(0, NCHANS):
    pad = c4.GetPad(ch+1)
    pad.cd()
    pad.SetLogy()
    hForced[ch].GetXaxis().SetRangeUser(zmin, zmax)
    hForced[ch].Draw()
    fit = hForced[ch].GetFunction("gaus")
    fit.Draw("same")
c4.cd()
c4.Update()

c5 = ROOT.TCanvas("c5","c5",700,600)
c5.Divide(2,2)
for ch in range(0, NCHANS):
    pad = c5.GetPad(ch+1)
    pad.cd()
    hAdcGausDev[ch] = ROOT.TH1F()
    hAdcs[ch].Copy(hAdcGausDev[ch])
    hAdcGausDev[ch].SetName("hAdcGausDev_ch{0}".format(ch))
    hAdcGausDev[ch].SetTitle("ADC deviation from gaussian (thermal trg);ADC;"
                             "fractional deviation")
    gs = hAdcs[ch].GetFunction("gaus")
    hAdcGausDev[ch].Divide(gs)
    pad.SetLogy()
    pad.SetGridy()
    hAdcGausDev[ch].SetMinimum(1e-1)
    hAdcGausDev[ch].SetMaximum(1e2)
    hAdcGausDev[ch].SetLineWidth(1)
    hAdcGausDev[ch].SetLineColor(ROOT.kSpring-5)
    #hAdcGausDev[ch].SetMarkerColor(ROOT.kSpring-5)
    hAdcGausDev[ch].SetMarkerColor(ROOT.kBlack)
    hAdcGausDev[ch].SetMarkerStyle(6)
    hAdcGausDev[ch].Draw()
    c5.Update()
    drawSigLine(hAdcs[ch], 5., ROOT.kOrange+7)
    drawSigLine(hForced[ch], 5., ROOT.kAzure-7)
c5.cd()
c5.Update()

c6 = ROOT.TCanvas("c6","c6",700,600)
c6.Divide(2,2)
for ch in range(0, NCHANS):
    pad = c6.GetPad(ch+1)
    pad.cd()
    hForcedGausDev[ch] = ROOT.TH1F()
    hForced[ch].Copy(hForcedGausDev[ch])
    hForcedGausDev[ch].SetName("hForcedGausDev_ch{0}".format(ch))
    hForcedGausDev[ch].SetTitle("ADC deviation from gaussian (forced trg);ADC;"
                             "fractional deviation")
    gs = hForced[ch].GetFunction("gaus")
    hForcedGausDev[ch].Divide(gs)
    pad.SetLogy()
    pad.SetGridy()
    hForcedGausDev[ch].SetMinimum(1e-1)
    hForcedGausDev[ch].SetMaximum(1e2)
    hForcedGausDev[ch].SetLineWidth(1)
    hForcedGausDev[ch].SetLineColor(ROOT.kSpring-5)
    #hForcedGausDev[ch].SetMarkerColor(ROOT.kSpring-5)
    hForcedGausDev[ch].SetMarkerColor(ROOT.kBlack)
    hForcedGausDev[ch].SetMarkerStyle(6)
    hForcedGausDev[ch].Draw()
    c6.Update()
    drawSigLine(hAdcs[ch], 5., ROOT.kOrange+7)
    drawSigLine(hForced[ch], 5., ROOT.kAzure-7)
c6.cd()
c6.Update()

def simpleThreshSearch(hist):
    s = hist.FindBin(0)
    e = hist.GetNbinsX()
    for i in range(s,e):
        if (hist.GetBinContent(i)>=5):
            return hist.GetBinCenter(i)
    # didn't find it
    raise Exception("Did not find deviation >=5 in bins [{0},{1}).".format(s,e))

def findSampAbove(data, thr):
    for i in range(0, len(data)):
        if data[i]>thr:
            return i
    raise Exception("Did not find sample above {0}.".format(thr))

c7 = ROOT.TCanvas("c7","c7",700,600)
c7.Divide(2,3)
i=0
thrshs = {}
for ch in range(0, NCHANS):
    if ch not in thrshs:
        try:
            thrshs[ch] = simpleThreshSearch(hAdcGausDev[ch])
            print "Found thresh of chan {0} at {1}".format(ch, thrshs[ch])
        except:
            continue
    for xc in range(0, ch):
        if xc not in thrshs:
            try:
                thrshs[xc] = simpleThreshSearch(hAdcGausDev[xc])
                print "Found thresh of chan {0} at {1}".format(xc, thrshs[xc])
            except:
                continue
        hCorrDist[(ch,xc)] = ROOT.TH1F("hCorrDist_ch{0}_ch{1}".format(ch,xc),
                        "num samples between first highs on chans;"
                        "chan {0} samp ADC>{1} - chan {2} samp ADC>{3}".format(
                ch, thrshs[ch], xc, thrshs[xc]),
                                       #257, -128.5, 128.5)
                                       129, -64.5, 64.5)

i=0
for entry in nt:
    if ( (i % 200)==0 ):
        sys.stderr.write("Processing {0}/{1} ...                  \r"
                         .format(i,n))
    if ( (entry.extra10 & THRM_TRG_BITMASK)!=0 ):
        p = [entry.ps00, entry.ps01, entry.ps02, entry.ps03]
        x = [entry.x0, entry.x1, entry.x2, entry.x3]
        for ch in range(0, NCHANS):
            try:
                if p[ch][x[ch]] > thrshs[ch]:
                    chi = findSampAbove(p[ch], thrshs[ch])
                    for xc in range(0, ch):
                        if p[xc][x[xc]] > thrshs[xc]:
                            xhi = findSampAbove(p[xc], thrshs[xc])
                            dist = chi - xhi
                            if abs(dist)>64:
                                if chi < xhi:
                                    dist = (chi+128) - xhi
                                else:
                                    dist = chi - (xhi+128)
                            hCorrDist[(ch,xc)].Fill(dist)
#                           if ch==3 and xc==1:
#                               print "i={0}, evt={1}, chi={2}, xhi={3}"\
#                                     .format(i, entry.EvtNum, chi, xhi)
            except:
                continue
    i+=1

i=0
for ch in range(0, NCHANS):
    for xc in range(0, ch):
        i+=1
        c7.cd(i)
        pad = c7.GetPad(i)
        pad.SetLogy()
        if (ch,xc) in hCorrDist:
            hCorrDist[(ch,xc)].Draw()

c7.cd()
c7.Update()


outf.Write()
