import os,sys
import ROOT

outdir = "/data/ONLINE/Calibs"

calsets = {
    '0002F7F175B7': { 0: {  1: (0.969, 0.002,  0.4, 0.3),
                           -1: (0.965, 0.002,  0.3, 0.0) },
                      1: {  1: (0.911, 0.021,  7.5, 3.1),
                           -1: (0.930, 0.021, -2.3, 0.0) },
                      2: {  1: (0.963, 0.007,  2.2, 0.9),
                           -1: (0.973, 0.007,  0.3, 0.3) },
                      3: {  1: (0.954, 0.002,  0.5, 0.3),
                           -1: (0.940, 0.002, -1.3, 0.3) },
                      },
    '0002F7F0C61A': { 0: {  1: (0.919, 0.003,  1.4, 0.4),
                           -1: (0.911, 0.003,  -0.1, 0.0) },
                      1: {  1: (0.918, 0.005,  1.0, 0.7),
                           -1: (0.912, 0.005, -0.3, 0.0) },
                      2: {  1: (0.917, 0.004,  2.1, 0.7),
                           -1: (0.906, 0.003,  -0.4, 0.7) },
                      3: {  1: (1, 0,  0, 0),
                           -1: (1, 0,  0, 0) },
                      },
    '0002F7F0C561': { 0: {  1: (0.920, 0.007,  -0.2, 1.1),
                           -1: (0.896, 0.007,  -2.2, 0.0) },
                      1: {  1: (0.924, 0.012,  -0.3, 1.9),
                           -1: (0.898, 0.012, -1.9, 0.0) },
                      2: {  1: (0.927, 0.015,  -0.4, 2.2),
                           -1: (0.898, 0.012,  -2.0, 1.2) },
                      3: {  1: (0.927, 0.008,  1.3, 1.2),
                           -1: (0.924, 0.008, -0.7, 1.2) },
                      }
}

def main():
    if not os.path.exists(outdir):
        os.makedirs(outdir)
    
    for stn, chcals in calsets.iteritems():
        outfn = "{0}/ReadoutCalibSet.{1}.root".format(outdir, stn)
        outf = ROOT.TFile(outfn,"RECREATE")
        cs = ROOT.TSnDualGainCalibSet("DigitizerCalibSet")
        for ch, vc in chcals.iteritems():
            cs.AddCalibAtChan(ch,
                          vc[1][0], vc[1][1], vc[1][2], vc[1][3],
                          vc[-1][0], vc[-1][1], vc[-1][2], vc[-1][3])
        outf.cd()
        cs.Write()
        print "Wrote [{0}]".format(outf.GetName())
    

if __name__=="__main__":
    main()
