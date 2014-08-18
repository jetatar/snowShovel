import os,sys
import ROOT


if (len(sys.argv)<2):
    print "Usage: python -i drawNHM.py [input filename]"
    sys.exit(1)

infn = sys.argv[1]

inf = ROOT.TFile(infn)

tags = ["Forced", "PureThm", "NonThm"]
lbls = ["Unbiased", "Pure Thermal", "Non-Thermal"]
cols = [ROOT.kGray+1, ROOT.kBlack, ROOT.kGray+2]
lsty = [7, 3, 1]

ymin = 1e-5
ymax = 1

xmin = 0
xmax = 30

plots = {}
store = []

# change the directory if necessary
ROOT.gROOT.LoadMacro("~/work/plotDefaults.C")

ROOT.smallHistDefaults()
ROOT.gROOT.ForceStyle() # if histograms are loaded from a file, we need this

c1 = ROOT.TCanvas("c1","c1 - min NHM comparison", ROOT.cwid, ROOT.chit)
c1.cd()
c1.SetLogy()

leg = ROOT.TLegend(.6,.62,.93,.92)
leg.SetFillColor(0)
leg.SetBorderSize(1)
leg.SetTextFont(ROOT.titleFont)
leg.SetTextSize(ROOT.titleSize)
store.append(leg) # so it isn't garbage collected

first = True
for tag,col,lst,lbl in zip(tags,cols,lsty,lbls):
    h = inf.Get("{0}fhMinNHM".format(tag))
    plots[h.GetName()] = h # so it isn't garbage collected
    h.SetLineColor(col)
    h.SetLineStyle(lst)
    h.Scale( 1.0 / (h.Integral()*h.GetBinWidth(1)) )
    h.SetYTitle("prob. density")
    h.SetMinimum(ymin)
    h.SetMaximum(ymax)
    h.GetXaxis().SetRangeUser(xmin, xmax)
    opt = "same"
    if (first):
        opt = ""
        first = False
    h.Draw(opt)

    leg.AddEntry(h, lbl, "l")

leg.Draw()

c1.cd()
c1.Update()


