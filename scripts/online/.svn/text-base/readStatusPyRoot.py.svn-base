import sys
import struct

from AriUtils import *

def readStatus(msg):
    status = None
    evt    = None
    power  = None
    ssvers = ROOT.Long()
    erv    = ROOT.Long()
    prv    = ROOT.Long()
    mcode  = ROOT.Long()
    mlen   = ROOT.Long()
    totb   = msg.size()
    br     = 0
    while ( br<totb ):
        br += ROOT.TSnIOHeaderFrame.PyReadFrom(msg, mcode, mlen)
        if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kStatusCode)):
            status = ROOT.TSnStatusUpdate()
            try:
                br += ROOT.TSnIOStatusFrame.BytesReadFrom(msg,
                                                          status, ssvers)
            except:
                printout(vtype.kInfo,"Read without event failed. "
                         "Trying with event.")
                evt = ROOT.TSnEvent()
                br += ROOT.TSnIOStatusFrame.BytesReadFrom(msg,
                                                          status,
                                                          evt,
                                                          ssvers)
        elif (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kEventCode)):
            evt = ROOT.TSnEvent()
            br += ROOT.TSnIOEventFrame.BytesReadFrom(msg,
                                                     evt,
                                                     status.GetWvLoseLSB(),
                                                     status.GetWvLoseMSB(),
                                                     status.GetWvBaseline(),
                                                     erv)
        elif (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kPowerCode)):
            power = ROOT.TSnPowerReading()
            br += ROOT.TSnIOPowerFrame.BytesReadFrom(msg,
                                                     power, prv)
        else:
            raise ValueError("Unhandled block type {0:02x}".format(mcode))
    return br, status, power, evt, ssvers, prv, erv


def main():
    if (len(sys.argv)<2):
        print 'Need filename'
        sys.exit()

    infn = sys.argv[1]
    
    with open(infn,'rb') as inf:
        msg = ROOT.TSnIOBuffer(inf.read())
        br, status, power, evt, ssvers, prv, erv  = readStatus(msg)
        if (status!=None):
            status.Print()
        if (power!=None):
            power.Print()
        if (evt!=None):
            evt.Print()

if __name__=="__main__":
    main()
