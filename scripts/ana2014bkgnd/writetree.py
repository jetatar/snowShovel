import makenoise as mk, numpy as np
import ROOT

n       = 10000
A       = mk.getAfile( "A_0002F7F175B7.txt" )
samps   = mk.readsampledistribution( "deconvolved_samples_0002F7F175B7.root" )
noise   = np.transpose(mk.makenoise(n,A,samps))

# Output file and tree.
rootfile    = ROOT.TFile("stn11.NoiseTree.root","RECREATE")
noisetree   = ROOT.TTree( "CalibTree","tree containing arrays of noise" )
t           = ROOT.TSnCalWvData( "t", "TSnCalWvData" )

noisetree.Branch( "AmpOutData.", t, 262144, 1 )

for i in range(0,n):

    for j in range(0,128):
        t.SetData( 0, j, float(noise[i][j]) )

    noisetree.Fill()

noisetree.Write()
rootfile.Close()

