import os, sys
import array
import ROOT

NCHANS = 4
NSAMPS = 128
NSTOPS = 8

plots={}
store=[]

cwid = 500
chit = 400

rref = 15

save = False
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

cols = { 11: ROOT.kGray+3,
         12: ROOT.kViolet-6,
         13: ROOT.kAzure+2,
         17: ROOT.kSpring-6,
         18: ROOT.kRed+1,
         19: ROOT.kOrange+7 }
agcol = ROOT.kOrange+10

pname = { 11: "A",
          12: "B",
          13: "C",
          17: "D",
          18: "D",
          19: "D" }

arfil = { 11: 3745,
          12: 3006,
          13: 3754,
          17: 3750,
          18: 3750,
          19: 3750 }

allwid = 1
allfil = 3003
selwid = 2
selfil = 3001

rhomax=270

tpmin=1e-3
tpmax=1
phiresmin=-20
phiresmax=20
thetaresmin=-4
thetaresmax=4
arymin=0
arymax=0.045
arxmin=1e-2
arxmax=1

# position = (rho, phi, err rho, err phi)
posns = { 11: [(0./3.28),
               0.,
               (3./3.28),
               360.],
          12: [(218./3.28),
               (270.),
               (7./3.28),
               2.],
          13: [(218./3.28),
               (0.),
               (7./3.28),
               2.],
          17: [(515./3.28),
               (315.),
               (10./3.28),
               5.],
          18: [(515./3.28),
               (315.),
               (10./3.28),
               5.],
          19: [(515./3.28),
               (315.),
               (10./3.28),
               5.]
          }


indir = "/home/cjreed/work/BounceStudies/Stn4"

filens = { 11: "plots.ll035.chi125-22.zip.fitang.migrad.tworb.r15r11.s432.fitdt.migrad.prescan.10-70.root",
           12: "plots.ll035.chi125-22.zip.fitang.migrad.tworb.r15r12s26t41.s432.fitdt.migrad.prescan.10-70.root",
           #13: "plots.ll035.chi0-3.zip.fitang.migrad.tworb.r15r13s27t85.s432.fitdt.migrad.prescan.10-70.root",
           13: "plots.ll025.chi0-3.zip.fitang.migrad.tworb.r15r13s27t85.s432.fitdt.migrad.prescan.10-70.root",
           17: "plots.ll035.chi0-3.zip.fitang.migrad.tworb.r15r17.s432.fitdt.migrad.prescan.10-70.root",
           18: "plots.ll035.chi6-30.zip.fitang.migrad.tworb.r15r18.s432.fitdt.migrad.prescan.10-70.root",
           19: "plots.ll035.chi6-30.zip.fitang.migrad.tworb.r15r19.s432.fitdt.migrad.prescan.10-70.root" }

files = {}
for k,v in filens.iteritems():
    files[k] = ROOT.TFile.Open(indir+"/"+v)

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

# load the plots
for r,f in files.iteritems():
    if r not in plots:
        plots[r] = {}
    for key in f.GetListOfKeys():
        if ( (key.GetClassName() == "TProfile") or
             (key.GetClassName().startswith("TH")) ):
            h = key.ReadObj()
            h.SetLineColor(cols[r])
            h.SetFillColor(cols[r])
            if "All" in h.GetName():
                h.SetLineWidth(allwid)
                h.SetFillStyle(allfil)
            elif "Sel" in h.GetName():
                h.SetLineWidth(selwid)
                h.SetFillStyle(selfil)
            plots[r][key.GetName()] = h
    

clls = {}
crec = {}
cxy = ROOT.TCanvas("cxy","cxy", int(cwid*1.5), int(chit*2))
cxy.Divide(2,3)
ixy=0
hXYSelTot = None
hXYSelTotN12 = None
for r,ps in plots.iteritems():
    ixy+=1
    cxy.cd(ixy)
    cxy.GetPad(ixy).SetLeftMargin(0.15)
    cxy.GetPad(ixy).SetRightMargin(0.12)
    ps["hXYSel"].Draw("colz")
    tx = ROOT.TLatex(50,100,"run {0}".format(r))
    tx.Draw()
    store.append(tx)
    
    if (hXYSelTot==None):
        hXYSelTot = ps["hXYSel"].Clone()
        hXYSelTot.SetName("hXYSelTot")
        hXYSelTot.Scale(1.0/hXYSelTot.GetEntries())
    else:
        hXYSelTot.Add(hXYSelTot, ps["hXYSel"], 1.0, 1.0/ps["hXYSel"].GetEntries())
    if (r!=12):
        if (hXYSelTotN12==None):
            hXYSelTotN12 = ps["hXYSel"].Clone()
            hXYSelTotN12.SetName("hXYSelTotN12")
            hXYSelTotN12.Scale(1.0/hXYSelTotN12.GetEntries())
        else:
            hXYSelTotN12.Add(hXYSelTotN12,
                             ps["hXYSel"], 1.0, 1.0/ps["hXYSel"].GetEntries())
   
    if r not in clls:
        cn = "clls_{0}".format(r)
        clls[r] = ROOT.TCanvas(cn, cn, int(cwid*1.5), int(chit*1.5))
        clls[r].Divide(2,2)
    for ch in range(0, NCHANS):
        clls[r].cd(ch+1)
        clls[r].GetPad(ch+1).SetLogy()
        ha = ps["hllAll_{0}".format(ch)]
        ha.Draw()
        hs = ps["hllSel_{0}".format(ch)]
        hs.Draw("same")
        leg = ROOT.TLegend(0.418,0.763,0.901,0.916)
        leg.SetBorderSize(1)
        leg.SetFillColor(0)
        leg.SetMargin(0.2)
        leg.AddEntry(ha, "r{0}-r{1} ch{2} (all)".format(r,rref,ch), "f")
        leg.AddEntry(hs, "r{0}-r{1} ch{2} (selected)".format(r,rref,ch), "f")
        leg.Draw()
        store.append(leg)
    clls[r].cd()
    clls[r].Update()
    if (save):
        clls[r].Print("{0}/clls_r{1}.root".format(sdir,r))
        clls[r].Print("{0}/clls_r{1}.gif".format(sdir,r))
    
    if r not in crec:
        cn = "crec_{0}".format(r)
        crec[r] = ROOT.TCanvas(cn,cn, int(cwid*1.5), int(chit*1.5))
        crec[r].Divide(2,2)
    crec[r].cd(1)
    crec[r].GetPad(1).SetLogy()
    ps["hChi2All"].Draw()
    ps["hChi2Sel"].Draw("same")
    crec[r].cd(2)
    ps["hRhoAll"].GetXaxis().SetRangeUser(0,rhomax)
    ps["hRhoAll"].Draw()
    ps["hRhoSel"].Draw("same")
    crec[r].cd(3)
    ps["hThetaAll"].Draw()
    ps["hThetaSel"].Draw("same")
    leg = ROOT.TLegend(0.175,0.749,0.658,0.905)
    leg.SetBorderSize(1)
    leg.SetFillColor(0)
    leg.SetMargin(0.2)
    leg.AddEntry(ps["hThetaAll"], "r{0} - r{1} (all)".format(r,rref), "f")
    leg.AddEntry(ps["hThetaSel"], "r{0} - r{1} (selected)".format(r,rref), "f")
    leg.Draw()
    store.append(leg)
    crec[r].cd(4)
    ps["hPhiAll"].Draw()
    ps["hPhiSel"].Draw("same")
    crec[r].cd()
    crec[r].Update()
    if (save):
        crec[r].Print("{0}/crec_r{1}.root".format(sdir,r))
        crec[r].Print("{0}/crec_r{1}.gif".format(sdir,r))
    

cxy.cd()
cxy.Update()
if (save):
    cxy.Print("{0}/cxy_separate.root".format(sdir))
    cxy.Print("{0}/cxy_separate.gif".format(sdir))

ctxy = ROOT.TCanvas("ctxy","ctxy",cwid,chit)
ctxy.cd()
ctxy.SetLeftMargin(0.15)
ctxy.SetRightMargin(0.125)
hXYSelTot.Draw("colz")
for r,psn in posns.iteritems():
    #c = ROOT.TCrown(0,0,psn[0]-(psn[2]/2.0),psn[0]+(psn[2]/2.0),
    #                psn[1]-(psn[3]/2.0), psn[1]+(psn[3]/2.0))
    #c.SetLineWidth(1)
    #c.SetLineColor(cols[r])
    #c.Draw()
    #store.append(c)
    m = ROOT.TMarker(psn[0]*ROOT.TMath.Cos(psn[1]*ROOT.TMath.DegToRad()),
                     psn[0]*ROOT.TMath.Sin(psn[1]*ROOT.TMath.DegToRad()),
                     ROOT.kStar)
    m.SetMarkerColor(ROOT.kAzure+1)
    m.Draw()
    store.append(m)
ctxy.cd()
ctxy.Update()
if (save):
    ctxy.Print("{0}/ctxy_together.root".format(sdir))
    ctxy.Print("{0}/ctxy_together.gif".format(sdir))


ctxyn12 = ROOT.TCanvas("ctxyn12","ctxyn12",cwid,chit)
ctxyn12.cd()
ctxyn12.SetLeftMargin(0.15)
ctxyn12.SetRightMargin(0.125)
hXYSelTotN12.Draw("colz")
for r,psn in posns.iteritems():
    if (r!=12):
        m = ROOT.TMarker(psn[0]*ROOT.TMath.Cos(psn[1]*ROOT.TMath.DegToRad()),
                         psn[0]*ROOT.TMath.Sin(psn[1]*ROOT.TMath.DegToRad()),
                         ROOT.kStar)
        m.SetMarkerColor(ROOT.kAzure+1)
        m.Draw()
        store.append(m)
ctxyn12.cd()
ctxyn12.Update()
if (save):
    ctxyn12.Print("{0}/ctxyn12_together_norun12.root".format(sdir))
    ctxyn12.Print("{0}/ctxyn12_together_norun12.gif".format(sdir))


ctheta = ROOT.TCanvas("ctheta","ctheta",cwid,chit)
cphi   = ROOT.TCanvas("cphi","cphi",cwid,chit)
cangr  = ROOT.TCanvas("cangr","cangr",cwid,chit)
ctheta.SetLogy()
cphi.SetLogy()
cangr.SetLogx()
hThetaResSelFar = None
hPhiResSelFar = None
hAngResSelFar = None
legth = ROOT.TLegend(0.625,0.615,0.923,0.936)
legth.SetBorderSize(1)
legth.SetFillColor(0)
legth.SetMargin(0.2)
legth.SetTextSizePixels(15)
legph = ROOT.TLegend(0.625,0.615,0.923,0.936)
legph.SetBorderSize(1)
legph.SetFillColor(0)
legph.SetMargin(0.2)
legph.SetTextSizePixels(15)
legar = ROOT.TLegend(0.175,0.604,0.472,0.925)
legar.SetBorderSize(1)
legar.SetFillColor(0)
legar.SetMargin(0.2)
legar.SetTextSizePixels(15)
for r in plots.iterkeys():
    #if (r==12):
    #    continue

    opt = "same"
    if (r==11):
        opt = ""
    
    if (r<17):
        ctheta.cd()
        h = plots[r]["hThetaResSel"].DrawNormalized(opt)
        h.Fit("gaus","ME0Q","goff")
        f = h.GetFunction("gaus")
        rms = f.GetParameter(2)
        rmserr = f.GetParError(2)
        h.SetMinimum(tpmin)
        h.SetMaximum(tpmax)
        h.GetXaxis().SetRangeUser(thetaresmin,thetaresmax)
        h.SetName("hThetaResSelNorm")
        plots[r][h.GetName()] = h
        legth.AddEntry(h,"{0}: #sigma={1:2.2f}#pm{2:2.2f}".format(
                pname[r],rms,rmserr),"f")

        cphi.cd()
        h = plots[r]["hPhiResSel"].DrawNormalized(opt)
        h.Fit("gaus","ME0Q","goff")
        f = h.GetFunction("gaus")
        rms = f.GetParameter(2)
        rmserr = f.GetParError(2)
        h.SetMinimum(tpmin)
        h.SetMaximum(tpmax)
        h.GetXaxis().SetRangeUser(phiresmin,phiresmax)
        h.SetName("hPhiResSelNorm")
        plots[r][h.GetName()] = h
        legph.AddEntry(h,"{0}: #sigma={1:2.2f}#pm{2:2.2f}".format(
                pname[r],rms,rmserr),"f")
        
        cangr.cd()
        plots[r]["hAngResSel"].SetFillStyle(arfil[r])
        h = plots[r]["hAngResSel"].DrawNormalized(opt)
        h.SetMinimum(arymin)
        h.SetMaximum(arymax)
        h.GetXaxis().SetRangeUser(arxmin,arxmax)
        p = array.array('d',[0.5])
        q = array.array('d',[0])
        h.GetQuantiles(1,q,p)
        h.SetName("hAngResSel")
        plots[r][h.GetName()] = h
        legar.AddEntry(h,"{0}: median={1:2.2f}".format(pname[r],q[0]),"f")
    else:
        h = plots[r]["hThetaResSel"]
        if (hThetaResSelFar==None):
            hThetaResSelFar = h.Clone()
            hThetaResSelFar.SetName("hThetaResSelFar")
            hThetaResSelFar.SetLineColor(agcol)
            hThetaResSelFar.SetFillColor(agcol)
            hThetaResSelFar.SetFillStyle(selfil)
            store.append(hThetaResSelFar)
        else:
            hThetaResSelFar.Add(h)
        
        h = plots[r]["hPhiResSel"]
        if (hPhiResSelFar==None):
            hPhiResSelFar = h.Clone()
            hPhiResSelFar.SetName("hPhiResSelFar")
            hPhiResSelFar.SetLineColor(agcol)
            hPhiResSelFar.SetFillColor(agcol)
            hPhiResSelFar.SetFillStyle(selfil)
            store.append(hPhiResSelFar)
        else:
            hPhiResSelFar.Add(h)
        
        h = plots[r]["hAngResSel"]
        if (hAngResSelFar==None):
            hAngResSelFar = h.Clone()
            hAngResSelFar.SetName("hAngResSelFar")
            hAngResSelFar.SetLineColor(agcol)
            hAngResSelFar.SetFillColor(agcol)
            #hAngResSelFar.SetFillStyle(selfil)
            hAngResSelFar.SetFillStyle(arfil[r])
            store.append(hAngResSelFar)
        else:
            hAngResSelFar.Add(h)

ctheta.cd()
h = hThetaResSelFar
rms = h.GetRMS()
rmserr = rms / ROOT.TMath.Sqrt(2.0*(h.GetEntries()-1))
h.SetMinimum(tpmin)
h.SetMaximum(tpmax)
h.GetXaxis().SetRangeUser(phiresmin,phiresmax)
legth.AddEntry(h, "{0}: #sigma={1:2.2f}#pm{2:2.2f}".format(
        pname[17],rms,rmserr),"f")
h.DrawNormalized("same")
legth.Draw()
ctheta.Update()
ctheta.RedrawAxis()
ctheta.Update()
if (save):
    ctheta.Print("{0}/ctheta_res.root".format(sdir))
    ctheta.Print("{0}/ctheta_res.gif".format(sdir))

cphi.cd()
h = hPhiResSelFar
rms = h.GetRMS()
rmserr = rms / ROOT.TMath.Sqrt(2.0*(h.GetEntries()-1))
h.SetMinimum(tpmin)
h.SetMaximum(tpmax)
h.GetXaxis().SetRangeUser(phiresmin,phiresmax)
legph.AddEntry(h, "{0}: #sigma={1:2.2f}#pm{2:2.2f}".format(
        pname[17],rms,rmserr),"f")
h.DrawNormalized("same")
legph.Draw()
cphi.Update()
cphi.RedrawAxis()
cphi.Update()
if (save):
    cphi.Print("{0}/cphi_res.root".format(sdir))
    cphi.Print("{0}/cphi_res.gif".format(sdir))

cangr.cd()
h = hAngResSelFar
p = array.array('d',[0.5])
q = array.array('d',[0])
h.GetQuantiles(1,q,p)
legar.AddEntry(h,"{0}: median={1:2.2f}".format(pname[r],q[0]),"f")
h = h.DrawNormalized("same")
h.SetMinimum(arymin)
h.SetMaximum(arymax)
h.GetXaxis().SetRangeUser(arxmin,arxmax)
h.SetName("hAngResSelFarNorm")
store.append(h)
legar.Draw()
cangr.cd()
cangr.Update()
cangr.RedrawAxis()
cangr.Update()
if (save):
    cangr.Print("{0}/cangr_res.root".format(sdir))
    cangr.Print("{0}/cangr_res.gif".format(sdir))

