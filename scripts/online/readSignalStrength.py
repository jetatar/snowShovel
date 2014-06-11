import sys

from AriUtils import *

def readSignalStrength(msg):
    sigstr = None
    mcode  = ROOT.Long()
    mlen   = ROOT.Long()
    totb   = msg.size()
    br     = 0
    while ( br<totb ):
        br += ROOT.TSnIOHeaderFrame.PyReadFrom(msg, mcode, mlen)
        if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kSignalStrCode)):
            sigstr = ROOT.TSnSignalStrengthInfo()
            ssvers = ROOT.Long()
            br += ROOT.TSnIOSignalStrengthFrame.BytesReadFrom(msg, 
                                                              sigstr, ssvers)
        else:
            raise ValueError("Unhandled block type {0:02x}".format(mcode))
    return br, sigstr, ssvers

def main():
    if (len(sys.argv)<2):
        print 'Need filename'
        sys.exit()

    infn = sys.argv[1]
    
    with open(infn,'rb') as inf:
        msg = ROOT.TSnIOBuffer(inf.read())
        br, sigstr, ssvers = readSignalStrength(msg)
        if (sigstr!=None):
            sigstr.Print()

if __name__=="__main__":
    main()
