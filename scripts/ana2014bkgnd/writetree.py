import makenoise as mk, numpy as np
import ROOT

n       = 7000000
outfl   = "/w1/jtatar/Analysis/Stn0/stn11.NoiseTree.root"
A       = mk.getAfile( "A_0002F7F175B7.txt" )
samps   = mk.readsampledistribution( "deconvolved_samples_0002F7F175B7.root" )
noise   = np.transpose(mk.makenoise(n,A,samps))
kNchans = 4


# Output file and tree.
rootfile    = ROOT.TFile( outfl, "RECREATE" )
noisetree   = ROOT.TTree( "CalibTree","tree containing arrays of noise" )
t           = ROOT.TSnCalWvData( "t", "TSnCalWvData" )

noisetree.Branch( "AmpOutData.", t, 262144, 1 )

for i in range(0,n):
    for c in range( 0, kNchans ):
        for j in range(0,128):
            t.SetData( c, j, float(noise[i][j]) )

    noisetree.Fill()

noisetree.Write()
rootfile.Close()

