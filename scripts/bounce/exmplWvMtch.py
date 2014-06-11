import os, sys
import array
import ROOT

wmfn = "/home/cjreed/work/BounceStudies/Stn4/cdbgcl_r15r19_ev1_ch2.root"
evfn = "/home/cjreed/work/BounceStudies/Stn4/cdbgdt_r15r19_ev1.root"

NCHANS = 4
NSAMPS = 128
NSTOPS = 8
SRATE = 1.92 # GHz

useTimeAxis = True

plots={}
store=[]

cwid = 500
chit = 400

cols = [ROOT.kAzure-6,       ROOT.kOrange+7,    ROOT.kRed]
mrks = [ROOT.kFullDotMedium, ROOT.kOpenDiamond, ROOT.kOpenCircle]
linw = [1,                   1,                 1]
msiz = [1.0,                 0.8,               0.65]


save = True
sdir = "/home/cjreed/work/BounceStudies/Stn4/plots"

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetTitleYOffset(1.2);
ROOT.gStyle.SetPadLeftMargin(0.12);
ROOT.gStyle.SetPadRightMargin(0.04);
ROOT.gStyle.SetPadTopMargin(0.03);
ROOT.gStyle.SetPadBottomMargin(0.12);
ROOT.gSystem.Setenv("TZ","UTC");
ROOT.gStyle.SetTimeOffset(0);

def setStyle(h, i):
    h.SetLineWidth(linw[i])
    h.SetLineColor(cols[i])
    h.SetMarkerColor(cols[i])
    h.SetMarkerStyle(mrks[i])
    h.SetMarkerSize(msiz[i])

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

def getHistWithTimeAxis(o):
    if (o.IsA().InheritsFrom("TH1")):
        h = ROOT.TH1F("{0}_tax".format(o.GetName()),
                      o.GetTitle(),
                      NSAMPS, 0,
                      float(NSAMPS)/SRATE)
        h.SetXTitle("time (ns)")
        for i in xrange(1, o.GetNbinsX()+1):
            h.SetBinContent(i, o.GetBinContent(i))
        return h
    elif (o.IsA().InheritsFrom("TGraph")):
        for i in xrange(0, o.GetN()):
            ox = (o.GetX())[i]
            (o.GetX())[i] = ox / SRATE
        return o
    elif (o.IsA().InheritsFrom("TSpline")):
        return o
    else:
        print 'unknown type {0}'.format(o.IsA().GetName())
        sys.exit()

wmf = ROOT.TFile.Open(wmfn)
evf = ROOT.TFile.Open(evfn)

cdbgcl = wmf.Get("cdbgcl")
pad = cdbgcl.GetListOfPrimitives().At(0)
refw = pad.GetListOfPrimitives().At(1)
bncw = pad.GetListOfPrimitives().At(2)

if (useTimeAxis):
    refw = getHistWithTimeAxis(refw)
    bncw = getHistWithTimeAxis(bncw)

xaxtitle = "sample-stop"
if (useTimeAxis):
    xaxtitle = "time (ns)"

refs = ROOT.TSpline3(";{0};ADC-FPN".format(xaxtitle),refw)
bncs = ROOT.TSpline3(";{0};ADC-FPN".format(xaxtitle),bncw)

setStyle(refs, 0)
setStyle(bncs, 1)

refs.SetName("refs")
bncs.SetName("bncs")
refs.SetNpx(500)
bncs.SetNpx(500)

store.append(refs)
store.append(bncs)

pad = cdbgcl.GetListOfPrimitives().At(1)
corf = pad.GetListOfPrimitives().At(1).Clone()
if (useTimeAxis):
    corf = getHistWithTimeAxis(corf)
corf.SetTitle(";shift of far waveform (samples);correlation coef.")
corf.SetName("corf")
corf.SetNpx(500)
corf.GetHistogram().SetTitle(corf.GetTitle())
corf.GetHistogram().SetMaximum(1)
corf.GetHistogram().SetMinimum(-1)
corf.GetHistogram().GetYaxis().SetTitleOffset(1.2)

setStyle(corf, 1)

store.append(corf)

c1 = ROOT.TCanvas("c1","c1",cwid,chit)
c1.cd()
refs.Draw("pc")
bncs.Draw("pc same")
leg = ROOT.TLegend(.161,.716,.360,.916)
leg.SetFillColor(0)
leg.SetBorderSize(1)
leg.SetMargin(0.35)
leg.AddEntry(getDumHistForLegend(refs),"near","f")
leg.AddEntry(getDumHistForLegend(bncs),"far","f")
leg.Draw()
store.append(leg)
c1.cd()
c1.Update()
if (save):
    c1.Print("{0}/canv_filteredWvCmp.root".format(sdir))
    c1.Print("{0}/canv_filteredWvCmp.gif".format(sdir))

c4 = ROOT.TCanvas("c4","c4",cwid,chit)
c4.cd()
refs.Draw("pc")
c4.cd()
c4.Update()
bxmin = 10
bxmax = 70
if (useTimeAxis):
    bxmin /= SRATE
    bxmax /= SRATE
box = ROOT.TBox(bxmin, c4.GetUymin(), bxmax, c4.GetUymax())
box.SetFillColor(ROOT.kGray)
box.Draw()
refs.Draw("pc same")
bncs.Draw("pc same")
leg.Draw()
refs.GetHistogram().Draw("axis same")
c4.cd()
c4.Update()
c4.RedrawAxis()
c4.Update()
if (save):
    c4.Print("{0}/canv_filteredWvCmp_withRegion.root".format(sdir))
    c4.Print("{0}/canv_filteredWvCmp_withRegion.gif".format(sdir))

c2 = ROOT.TCanvas("c2","c2",cwid,chit)
c2.cd()
corf.Draw("pc")
c2.SetGridx()
c2.SetGridy()
c2.cd()
c2.Update()
if (save):
    c2.Print("{0}/canv_filtCorrelation.root".format(sdir))
    c2.Print("{0}/canv_filtCorrelation.gif".format(sdir))


cdbgdt = evf.Get("cdbgdt")
pad = cdbgdt.GetListOfPrimitives().At(2)
mvdw = pad.GetListOfPrimitives().At(5)
if (useTimeAxis):
    mvdw = getHistWithTimeAxis(mvdw)
mvds = ROOT.TSpline3(";{0};ADC-FPN".format(xaxtitle),mvdw)
mvds.SetName("mvds")

setStyle(mvds, 2)

store.append(mvds)

c3 = ROOT.TCanvas("c3","c3",cwid,chit)
c3.cd()
refs.Draw("pc")
mvds.Draw("pc same")
leg = ROOT.TLegend(.155,.725,.429,.925)
leg.SetFillColor(0)
leg.SetBorderSize(1)
leg.SetMargin(0.3)
leg.SetTextSizePixels(18)
leg.AddEntry(getDumHistForLegend(refs),"near","f")
leg.AddEntry(getDumHistForLegend(mvds),"far shifted","f")
leg.Draw()
store.append(leg)
c3.cd()
c3.Update()
if (save):
    c3.Print("{0}/canv_shiftedWvCmp.root".format(sdir))
    c3.Print("{0}/canv_shiftedWvCmp.gif".format(sdir))




