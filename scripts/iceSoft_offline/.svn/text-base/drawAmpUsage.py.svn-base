#!/usr/bin/env python                                                                                                            

import os, sys, pickle, glob, time

import buildRunInfo

from ROOT import *


configs = buildRunInfo.loadConfigs()

gSystem.Setenv("TZ","UTC")
gStyle.SetTimeOffset(0)


cols = [kOrange+7, kAzure-2, kViolet-1, kGreen+2, kRed+1]
mrks = [       24,       25,        27,       28,      7]


def setStyle(h, i):
   h.SetLineColor(cols[i])
   h.SetMarkerColor(cols[i])
   h.SetMarkerStyle(mrks[i])


plots = {}


acvals = list(configs['Acqd'].values())
acvals.sort()
for i in range(0,4):
   g = TGraph()
   n = 'gDac{0}vsT'.format(i)
   g.SetName(n)
   g.SetTitle("DAC of amp {0} vs time;date, time (UTC);DAC (counts)".format(i))
   setStyle(g, i)
   plots[n] = g
   
   j=0
   for acc in acvals:
      g.SetPoint(j, acc.utcdate, acc.__dict__['dac{0}'.format(i)][0])
      j += 1
   
   
hkvals = list(configs['Hkd'].values())
hkvals.sort()
g = TGraph()
n = 'gAmpsVsT'
g.SetName(n)
g.SetTitle("AMP config vs time;date, time (UTC);amps on")
setStyle(g, 4)
plots[n] = g
h = TGraph()
n = 'gNumAmpsVsT'
h.SetName(n)
h.SetTitle("Num amps on vs time;date, time (UTC);num amps on")
setStyle(h, 4)
plots[n] = h
j=0
k=0
for hkc in hkvals:
   on = 0
   if hkc.atwdState > 0:
      for i in range(0,4):
         if hkc.__dict__['amp{0}State'.format(i+1)] > 0:
            on += 1
            g.SetPoint(j, hkc.utcdate, i)
            j += 1
   h.SetPoint(k, hkc.utcdate, on)
   k += 1

c3 = TCanvas("c3","c3 - DACs single", 800, 450)
c1 = TCanvas("c1","c1 - DACs", 1600, 750)
c1.Divide(2,2)
for i in range(0,4):
   c1.cd(i+1)
   n = 'gDac{0}vsT'.format(i)
   g = plots[n]
   g.Draw("ap")
   g.GetHistogram().GetXaxis().SetTimeDisplay(1)
   g.GetHistogram().SetMaximum(4500)
   g.GetHistogram().SetMinimum(0)
   c3.cd()
   if i==0:
      g.Draw("ap")
   else:
      g.Draw("p")
c1.cd()
c1.Update()

c3.cd()
c3.Update()

c2 = TCanvas("c2","c2 - amps", 800, 750)
c2.Divide(1,2)
c2.cd(1)
n = 'gAmpsVsT'
g = plots[n]
g.Draw("ap")
g.GetHistogram().GetXaxis().SetTimeDisplay(1)
g.GetHistogram().SetMaximum(4)
g.GetHistogram().SetMinimum(-1)
c2.cd(2)
n = 'gNumAmpsVsT'
g = plots[n]
g.Draw("ap")
g.GetHistogram().GetXaxis().SetTimeDisplay(1)
g.GetHistogram().SetMaximum(5)
g.GetHistogram().SetMinimum(-1)
c2.cd()
c2.Update()


