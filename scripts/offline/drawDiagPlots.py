import os, sys
import ROOT

import dacs

if (len(sys.argv)<2):
    print 'No input file specified.'
    sys.exit(0)

if (len(sys.argv)<3):
    print 'No output tag specified.'
    sys.exit(0)

if (len(sys.argv)<4):
    print 'No station number specified.'
    sys.exit(0)

save=False

NCHANS=4

adcmin = -200
adcmax =  200

#runmin=

cwid = 500
chit = 400

infn = sys.argv[1]
inf  = ROOT.TFile.Open(infn)

otag = sys.argv[2]

stn  = int(sys.argv[3])

plots={}
store=[]

ccols = [ROOT.kOrange+7, ROOT.kAzure-2, ROOT.kViolet-1, ROOT.kGreen+2]
tcols = [ROOT.kViolet+2, ROOT.kRed]
lcol  = ROOT.kTeal+5

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetTitleYOffset(1.2);
ROOT.gStyle.SetPadLeftMargin(0.12);
ROOT.gStyle.SetPadRightMargin(0.04);
ROOT.gStyle.SetPadTopMargin(0.03);
ROOT.gStyle.SetPadBottomMargin(0.12);
ROOT.gSystem.Setenv("TZ","UTC");
ROOT.gStyle.SetTimeOffset(0);
ROOT.gROOT.ForceStyle()

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

def setTimeDisp(h):
    h.GetXaxis().SetTimeFormat("%m/%d")
    h.GetXaxis().SetNdivisions(505)
    h.GetXaxis().SetTimeDisplay(1)


# get all plots
for key in inf.GetListOfKeys():
    cls = ROOT.TClass.GetClass(key.GetClassName())
    isPlot = (cls.InheritsFrom("TH1")) or\
             (cls.InheritsFrom("TGraph"))
    if (isPlot):
        plots[key.GetName()] = key.ReadObj()

def showNorm2D(ch, canvDict, ctag, ctitle, nplottag,
               zaxtitle, plottag, savetag):
    canvDict[ch] = ROOT.TCanvas("{0}{1}".format(ctag,ch),
                                "{0}{1} - {2} ch{1}".format(ctag,ch,ctitle),
                                cwid,chit)
    canvDict[ch].SetRightMargin(0.155)
    canvDict[ch].cd()
    hf = plots["h{0}_ch{1}".format(plottag,ch)]
    hn = plots["h{0}".format(nplottag)]
    for i in range(0, hf.GetNbinsX()):
        n = hn.GetBinContent(i+1)
        if (n>0):
            for j in range(0, hf.GetNbinsY()):
                hf.SetBinContent(i+1,j+1, hf.GetBinContent(i+1,j+1) / n)
    setTimeDisp(hf)
    hf.SetZTitle(zaxtitle)
    hf.Draw("colz")
    canvDict[ch].cd()
#    canvDict[ch].SetLogz( 1 )
    canvDict[ch].Update()
    if (save):
        canvDict[ch].Print("canv.{0}.ch{1}.{2}.root".format(savetag,ch,otag))
        canvDict[ch].Print("canv.{0}.ch{1}.{2}.pdf".format(savetag,ch,otag))
        canvDict[ch].Print("canv.{0}.ch{1}.{2}.gif".format(savetag,ch,otag))


cv={}
ctfrq={}
cffrq={}
ctmfq={}
cfmfq={}
ctlpf={}
cflpf={}

cthvt = ROOT.TCanvas("cthvt","cthvt - threshold vs time",cwid,chit)
fcsvt = ROOT.TCanvas("fcsvt","fcsvt - frc fit sigma vs time",cwid,chit)
tcsvt = ROOT.TCanvas("tcsvt","tcsvt - thm fit sigma vs time",cwid,chit)
svtleg = ROOT.TLegend(.530,.195,.829,.497)
svtleg.SetFillColor(0)
svtleg.SetBorderSize(1)
for ch in range(0,NCHANS):
    cv[ch] = ROOT.TCanvas("c{0}".format(ch),
                          "c{0} - adc, ch{0}, good smp".format(ch),
                          cwid,chit)
    cv[ch].cd()
    hf = plots["hFpnSubFrc_ch{0}".format(ch)]
    ht = plots["hFpnSubThm_ch{0}".format(ch)]
    hf.SetLineColor(tcols[0])
    ht.SetLineColor(tcols[1])
    hf.Scale(1.0/hf.GetMaximum())
    ht.Scale(1.0/ht.GetMaximum())
    hf.Draw()
#    hf.GetXaxis().SetRangeUser(adcmin,adcmax)
    hf.Fit("gaus","0")
    ht.Fit("gaus","0")
    ht.Draw("same")
    ffit = hf.GetFunction("gaus")
    ffit.SetLineWidth(2)
    ffit.SetLineColor(lcol)
    tfit = ht.GetFunction("gaus")
    leg = ROOT.TLegend(.691,.668,.929,.930)
    leg.SetBorderSize(1)
    leg.SetFillColor(0)
    leg.AddEntry(getDumHistForLegend(hf),
                  "Minbias","f")
    leg.AddEntry(0,"(#sigma={0:2.1f})".format(
            ffit.GetParameter(2)),"")
    leg.AddEntry(getDumHistForLegend(ht),
                  "Thermal","f")
    leg.AddEntry(0,"(#sigma={0:2.1f})".format(
            tfit.GetParameter(2)),"")
    leg.Draw()
    store.append(leg)
    cv[ch].SetLogy()
    cv[ch].cd()
    cv[ch].Update()
    if (save):
        cv[ch].Print("canv.adcDist.ch{0}.{1}.root".format(ch,otag))
        cv[ch].Print("canv.adcDist.ch{0}.{1}.pdf".format(ch,otag))
        cv[ch].Print("canv.adcDist.ch{0}.{1}.gif".format(ch,otag))
    
    # forced
    a = ROOT.TObjArray()
    hf = plots["hFpnSubVsTFrc_ch{0}".format(ch)]
    hf.FitSlicesY(0,0,-1,0,"QNR",a)
    svt = a.At(2)
    svt.SetName("hFitFrcSigVsT_ch{0}".format(ch))
    plots[svt.GetName()] = svt
    svt.SetTitle("Width of ADC dist vs time (minbias, good samples)"
                 ";date, time (UTC);Gaussian #sigma (ADC)")
    svt.SetLineColor(ccols[ch])
    svt.SetMarkerColor(ccols[ch])
    svt.SetMinimum(20)
    svt.SetMaximum(100)
    setTimeDisp(svt)
    fcsvt.cd()
    fcsvt.SetGridx()
    opt = ""
    if (ch!=0):
        opt = "same"
    svt.Draw(opt)
    svtleg.AddEntry(getDumHistForLegend(svt),
                    "Channel {0}".format(ch),"f")
    if (ch==(NCHANS-1)):
        svtleg.Draw()
    
    # thermal
    a = ROOT.TObjArray()
    ht = plots["hFpnSubVsTThm_ch{0}".format(ch)]
    ht.FitSlicesY(0,0,-1,0,"QNR",a)
    svt = a.At(2)
    svt.SetName("hFitThmSigVsT_ch{0}".format(ch))
    plots[svt.GetName()] = svt
    svt.SetTitle("Width of ADC dist vs time (thermal trig, good samples)"
                 ";date, time (UTC);Gaussian #sigma (ADC)")
    svt.SetLineColor(ccols[ch])
    svt.SetMarkerColor(ccols[ch])
    svt.SetMinimum(20)
    svt.SetMaximum(150)
    setTimeDisp(svt)
    tcsvt.cd()
    tcsvt.SetGridx()
    opt = ""
    if (ch!=0):
        opt = "same"
    svt.Draw(opt)
    if (ch==(NCHANS-1)):
        svtleg.Draw()
    
    # threshold
#    cthvt.cd()
#    ht = plots["hHiTrshVsT_ch{0}".format(ch)]
#    dvt = ROOT.TH1F("hSigHiThrshVsT_ch{0}".format(ch),
#                    "{0};{1};{2}".format(ht.GetTitle(),
#                                         ht.GetXaxis().GetTitle(),
#                                         ht.GetYaxis().GetTitle()),
#                    ht.GetNbinsX(),
#                    ht.GetXaxis().GetXmin(), ht.GetXaxis().GetXmax())
#    plots[dvt.GetName()] = dvt
#    setTimeDisp(dvt)
#    dvt.SetLineColor(ccols[ch])
#    dvt.SetMarkerColor(ccols[ch])
#    dvt.SetMarkerStyle(27)
#    svt = plots["hFitFrcSigVsT_ch{0}".format(ch)]
#    for i in range(0, ht.GetNbinsX()):
#        a = 0.0
#        n = 0.0
#        c = 0.0
#        for j in range(0, ht.GetNbinsY()):
#            c  = ht.GetBinContent(i+1, j+1)
#            a += ht.GetYaxis().GetBinCenter(j+1) * c 
#            n += c
#        if (n>1.0):
#            a /= n
#            vth = dacs.getVth( (dacs.stnMap[stn][ch]), a)
            #dac = dacs.getAdc( (dacs.stnMap[stn][ch]), vth)
            #dac /= svt.GetBinContent(i+1)
#            dvt.Fill(ht.GetXaxis().GetBinCenter(i+1), vth)
#    opt = ""
#    if (ch!=0):
#        opt = "same"
#    dvt.Draw(opt)

    # frequency - forced
    showNorm2D(ch, cffrq, "cffrq", "ave frq frc", "EvtsPerBinFrc",
               "power (a.u.)", "FpnSubFftVsTFrc", "AveFrqFrcVsT")
    
    # frequency - thermal
    showNorm2D(ch, ctfrq, "ctfrq", "ave frq thm", "EvtsPerBinThm",
               "power (a.u.)", "FpnSubFftVsTThm", "AveFrqThmVsT")

    # max frequency - forced
#    showNorm2D(ch, cfmfq, "cfmfq", "max frq frc", "NFrc",
#               "ave power (a.u.)", "MaxFrqFrc", "MaxFrqFrcVsT")

    # max frequency - thermal
#    showNorm2D(ch, ctmfq, "ctmfq", "max frq thm", "NThm",
#               "ave power (a.u.)", "MaxFrqThm", "MaxFrqThmVsT")

    # low power fraction - forced
#    showNorm2D(ch, cflpf, "cflpf", "low pwr frac frc", "NFrc",
#               "", "LowPwrFrc", "LowPwrFrcVsT")

    # low power fraction - thermal
#    showNorm2D(ch, ctlpf, "ctlpf", "low pwr frac thm", "NThm",
#               "", "LowPwrThm", "LowPwrThmVsT")

fcsvt.cd()
fcsvt.Update()
if (save):
    fcsvt.Print("canv.FrcSigVsT.{0}.root".format(otag))
    fcsvt.Print("canv.FrcSigVsT.{0}.pdf".format(otag))
    fcsvt.Print("canv.FrcSigVsT.{0}.gif".format(otag))
tcsvt.cd()
tcsvt.Update()
if (save):
    tcsvt.Print("canv.ThmSigVsT.{0}.root".format(otag))
    tcsvt.Print("canv.ThmSigVsT.{0}.pdf".format(otag))
    tcsvt.Print("canv.ThmSigVsT.{0}.gif".format(otag))

cthvt.cd()
cthvt.Update()
if (save):
    cthvt.Print("canv.ThreshVsT.ch{0}.{1}.root".format(ch,otag))
    cthvt.Print("canv.ThreshVsT.ch{0}.{1}.pdf".format(ch,otag))
    cthvt.Print("canv.ThreshVsT.ch{0}.{1}.gif".format(ch,otag))

#cruvt = ROOT.TCanvas("cruvt","cruvt - run vs time",cwid,chit)
#cruvt.SetRightMargin(0.155)
#hr = plots["hRunVsT"]
#setTimeDisp(hr)
#hr.SetZTitle("N evts")
#hr.Draw("colz")
#if (save):
#    cruvt.Print("canv.RunVsT.{0}.root".format(otag))
#    cruvt.Print("canv.RunVsT.{0}.pdf".format(otag))
#    cruvt.Print("canv.RunVsT.{0}.gif".format(otag))

<<<<<<< .mine
#cbat = ROOT.TCanvas("cbat","cbat - battery vs time",int(cwid*1.5),chit)
#cbat.cd()
#hbat = [ plots["hBatOffVsT"],
#         plots["hBatDepVsT"],
#         plots["hBatChgVsT"],
#         plots["hBatFulVsT"] ]
#hbcols = [ ROOT.kBlack, ROOT.kOrange+7, ROOT.kAzure-7, ROOT.kGreen+1 ]
#hbfils = [ 1, 3007, 3006, 3001 ]
#hbatst = ROOT.THStack("hBatStack","Battery usage;"
#                      "date, time (UTC);fraction of time")
#for h,c,f in zip(hbat,hbcols,hbfils):
#    h.SetFillStyle(f)
#    h.SetFillColor(c)
#    h.SetLineColor(c)
#    hbatst.Add(h)
#hbatst.Draw()
#cbat.cd()
#cbat.Update()
#setTimeDisp(hbatst)
#cbat.Update()
=======
cbat = ROOT.TCanvas("cbat","cbat - battery vs time",int(cwid*1.5),chit)
cbat.cd()
hbat = [ plots["hBatOffVsT"],
         plots["hBatDepVsT"],
         plots["hBatChgVsT"],
         plots["hBatFulVsT"] ]
hbcols = [ ROOT.kBlack, ROOT.kOrange+7, ROOT.kAzure-7, ROOT.kGreen+1 ]
hbfils = [ 1, 3007, 3006, 3001 ]
hbatst = ROOT.THStack("hBatStack","Battery usage;"
                      "date, time (UTC);fraction of time")
for h,c,f in zip(hbat,hbcols,hbfils):
    h.SetFillStyle(f)
    h.SetFillColor(c)
    h.SetLineColor(c)
    hbatst.Add(h)
hbatst.Draw()
cbat.cd()
cbat.Update()
setTimeDisp(hbatst)
cbat.Update()
if (save):
    cbat.Print("canv.Battery.{0}.root".format(otag))
    cbat.Print("canv.Battery.{0}.pdf".format(otag))
    cbat.Print("canv.Battery.{0}.gif".format(otag))
>>>>>>> .r1117
