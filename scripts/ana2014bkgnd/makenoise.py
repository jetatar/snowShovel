import numpy
import ROOT
from ROOT import TGraph, TCanvas, TH2F, TH1F, TFile, TLegend, TRandom3

#r = TRandom3(0)

def plotsampledistribution(noise,A):

  samp = []
  for i in range(0,128):
    samp.append(TH1F("samp{}".format(i),"samp{}".format(i),400,-30,30))

  for sets in numpy.transpose(noise):
    b = numpy.linalg.solve(A,sets)
    i = 0
    for entry in b:
      samp[i].Fill(entry)
      i += 1

  return samp

#Get the pre-made ones
def readsampledistribution():

  sampfile = TFile.Open("deconvolved_samples.root")
  samp = []
  for i in range(0,128):
    samp.append(sampfile.Get("samp{}".format(i)))

  return samp

# Get waveform data from files.
def getnoisedata(n):

  ntfile = TFile.Open("nt.ps.final.root")
  tree = ntfile.Get("runtree")

  i = 0
  datanoise = []
  for line in tree:
    i += 1
    if (i > n): break
    ch  = tree.dataps00
    temp = []
    for samp in ch:
      temp.append(samp)
    datanoise.append(temp)

  datanoise = numpy.array(datanoise)
  return numpy.transpose(datanoise)

# Find the covariance matrix for minbias data.
def getAdata(datanoise):
  R = numpy.cov(datanoise)
  A = numpy.linalg.cholesky(R)
  return A

# Calc covariance matrix for finite bandwidth simulated noise.
def getAsimnoise(simnoise):
  R = numpy.cov(simnoise)
  A = numpy.linalg.cholesky(R)
  return A

def getAfile():
  f = open("A.txt","r")
  string = f.read()
  exec string
  return A

# 
# n - number of events to generate
# A - lower triangular covariance matrix of minbias data set. 
#
#
#
def makenoise(n,A,hists):
  noise = numpy.zeros((n,128))
  print len(noise)
  for i in range(0,len(noise)):
    for j in range(0,128):
      noise[i][j] = hists[j].GetRandom()
#  print noise
  noise = numpy.inner(A,noise)
  return noise

def makenoiseinfinite(n):
  noise = numpy.random.normal(0,1,(n,128))
  return numpy.transpose(noise)

def aveFFTmag(noise):
  fft = numpy.fft.rfft(noise,None,0)
  fft = numpy.transpose(fft)
  avefft = numpy.zeros(65)
  for x in fft:
    avefft = avefft + numpy.absolute(x)
  avefft =  avefft/float(len(fft))
  return avefft

def aveFFT(noise):
  fft = numpy.fft.rfft(noise,None,0)
  fft = numpy.transpose(fft)
  avefft = numpy.zeros(65)
  for x in fft:
    avefft = avefft + x
  avefft =  numpy.absolute(avefft/float(len(fft)))
  return avefft

def corrcoef(noise1,noise2):

  length = len(noise1[0,:])
  if (len(noise2[0,:]) < len(noise1[0,:])):
    length = len(noise2)

  temp = TH1F("temp","temp",100,0,1)
  for i in range(0,length-1):
    x = noise1[:,i]
    y = noise2[:,i+1]
    # i+1 so that if noise1 and noise2 are the same data then it's 
    # still meaningful
    x = (x - numpy.mean(x))/(numpy.std(x)*len(x))
    y = (y - numpy.mean(y))/numpy.std(y)

    corr = max(numpy.correlate(x,y,"same"))
    temp.Fill(corr)

  return temp

#freq = []
#for i in range(1,66):
#  freq.append(14.769230*i)
#freq = numpy.array(freq)
#
#noise = getnoisedata(100000)e
#A = getAfile()
#samps = readsampledistribution()
#simnoise = makenoise(100000,A,samps)
#infnoise = makenoiseinfinite(100000)
#print noise.shape
#print simnoise.shape
#print infnoise.shape

#simA = getAsimnoise(simnoise)
#simFFT = aveFFT(simnoise)
#print numpy.amax(simA-A)

#FFT = aveFFT(noise)
#simFFT = aveFFT(simnoise)

#noise_self = corrcoef(noise,noise)
#simnoise_self = corrcoef(simnoise,simnoise)
#infnoise_self = corrcoef(infnoise,infnoise)
#simnoise_noise = corrcoef(noise,simnoise)
#infnoise_noise = corrcoef(noise,infnoise)

#c1 = TCanvas("c1","c1",1200,700)
#axes = TH2F("axes","axes",0,0,1000,0,0,150)
#simFFTgraph = TGraph(len(freq),freq,simFFT)
#FFTgraph = TGraph(len(freq),freq,FFT)
#axes.Draw()
#simFFTgraph.SetLineColor(3)
#simFFTgraph.Draw("l")
#FFTgraph.SetLineColor(4)
#FFTgraph.Draw("l")

#c2 = TCanvas("c2","c2",1200,700)
#infnoise_self.SetLineColor(5)
#infnoise_self.Draw()
#noise_self.SetLineColor(3)
#noise_self.Draw("same")
#simnoise_self.SetLineColor(4)
#simnoise_self.Draw("same")
#simnoise_noise.SetLineColor(6)
#simnoise_noise.Draw("same")
#infnoise_noise.SetLineColor(7)
#infnoise_noise.Draw("same")
#leg = TLegend(0.7,0.5,0.95,0.95)
#leg.AddEntry(noise_self,"data-self: mu {}, RMS {}".format(noise_self.GetMean(),#noise_self.GetRMS()),"l")
#leg.AddEntry(simnoise_self,"sim-self: mu {}, RMS {}".format(simnoise_self.GetMe#an(),simnoise_self.GetRMS()),"l")
#leg.AddEntry(infnoise_self,"inf-self: mu {}, RMS {}".format(infnoise_self.GetMe#an(),infnoise_self.GetRMS()),"l")
#leg.AddEntry(infnoise_noise,"inf-data: mu {}, RMS {}".format(infnoise_noise.Get#Mean(),infnoise_noise.GetRMS()),"l")
#leg.AddEntry(simnoise_noise,"sim-data: mu {}, RMS {}".format(simnoise_noise.Get#Mean(),simnoise_noise.GetRMS()),"l")
#leg.Draw()
