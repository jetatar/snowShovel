import os,sys
import ROOT

if (len(sys.argv)<2):
    print "Usage: python drawStoplessFitDts.py [file with dts ntuple]"
    sys.exit()

inf = ROOT.TFile(sys.argv[1])

nt = inf.Get("tChanDTsFit")

plots={}
store=[]

cwid = 500
chit = 400

#ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetTitleYOffset(1.2);
ROOT.gStyle.SetPadLeftMargin(0.12);
ROOT.gStyle.SetPadRightMargin(0.04);
ROOT.gStyle.SetPadTopMargin(0.03);
ROOT.gStyle.SetPadBottomMargin(0.12);
ROOT.gSystem.Setenv("TZ","UTC");
ROOT.gStyle.SetTimeOffset(0);

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

dtmin=-3.0
dtmax=3.0
dtbins=int( (dtmax-dtmin)/0.01 )

cmin=-0.005
cmax=0.005
cbins=int ( (cmax-cmin)/0.0001 )

hdts = {}
for ch in range(0, 4):
    for xc in range(0, ch):
        hdts[(ch,xc)] = ROOT.TH1F("hdts_{0}_{1}".format(ch,xc),
                                  ";#Deltat (ch{0}-ch{1})",
                                  dtbins, dtmin, dtmax)
hc20 = ROOT.TH1F("hc20",
                 ";#Deltat_{2,0} - (#Deltat_{2,1}+#Deltat_{1,0})",
                 cbins, cmin, cmax)
hc31 = ROOT.TH1F("hc31",
                 ";#Deltat_{3,1} - (#Deltat_{3,2}+#Deltat_{2,1})",
                 cbins, cmin, cmax)
hc30 = ROOT.TH1F("hc30",
                 ";#Deltat_{3,0} - (#Deltat_{3,2}+#Deltat_{2,1}+#Deltat_{1,0})",
                 cbins, cmin, cmax)
                 
for e in nt:
    hdts[(1,0)].Fill(e.dt10)
    hdts[(2,0)].Fill(e.dt20)
    hdts[(2,1)].Fill(e.dt21)
    hdts[(3,0)].Fill(e.dt30)
    hdts[(3,1)].Fill(e.dt31)
    hdts[(3,2)].Fill(e.dt32)
    hc20.Fill(e.dt20 - e.dt21 - e.dt10)
    hc31.Fill(e.dt31 - e.dt32 - e.dt21)
    hc30.Fill(e.dt30 - e.dt32 - e.dt21 - e.dt10)

c1 = ROOT.TCanvas("c1","c1",int(cwid*1.5),int(chit*2))
c1.Divide(2,3)
i = 0
for ch in range(0, 4):
    for xc in range(0, ch):
        i += 1
        c1.cd(i)
        c1.GetPad(i).SetLogy()
        hdts[(ch,xc)].Draw()
c1.cd()
c1.Update()

c2 = ROOT.TCanvas("c2","c2",int(cwid*1.5),int(chit*1.5))
c2.Divide(2,2)
c2.cd(1)
c2.GetPad(1).SetLogy()
hc20.GetXaxis().SetNdivisions(505)
hc20.Draw()
c2.cd(2)
c2.GetPad(2).SetLogy()
hc30.GetXaxis().SetNdivisions(505)
hc30.Draw()
c2.cd(4)
c2.GetPad(4).SetLogy()
hc31.GetXaxis().SetNdivisions(505)
hc31.Draw()
c2.cd()
c2.Update()

