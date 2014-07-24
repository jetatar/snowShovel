import makenoise as mk, numpy as np
import ROOT

n       = 10000
A       = mk.getAfile()
samps   = mk.readsampledistribution()
noise   = np.transpose(mk.makenoise(n,A,samps))

rootfile    = ROOT.TFile("noisetree.root","RECREATE")
noisetree   = ROOT.TTree( "CalibTree","tree containing arrays of noise" )
t           = ROOT.TSnCalWvData( "t", "TSnCalWvData" )

noisetree.Branch( "AmpOutData.", t, 262144, 1 )

for i in range(0,n):

    for j in range(0,128):
        t.SetData( 0, j, float(noise[i][j]) )

    noisetree.Fill()

noisetree.Write()
rootfile.Close()
