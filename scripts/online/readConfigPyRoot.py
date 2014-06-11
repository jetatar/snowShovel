import sys

from AriUtils import *

import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat.so")
ROOT.gSystem.Load("$SNS/lib/libonline.so")

def main():
    if (len(sys.argv)<2):
        print 'Need filename'
        sys.exit()

    infn = sys.argv[1]
    
    obs=None
    if (USE_LOG):
        obs = log.startLogging(sys.stdout)

    # see readConfigFile in stationIO for the details
    conf, runf, nets, mcode, mlen, ciov = readConfigFile(infn)

    if (obs!=None):
        printout(vtype.kDebug,"Stopping logger")
        obs.stop()

def readConfig(buf):
    mcode = ROOT.Long()
    mlen  = ROOT.Long()
    br = ROOT.TSnIOHeaderFrame.PyReadFrom(buf, mcode, mlen)
    if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kConfigCode)):
        conf = ROOT.TSnDAQConfig()
        runf = ROOT.TSnRunInfo()
        nets = ROOT.TSnNetAdrSet()
        ciov = ROOT.Long()
        #ROOT.TSnIOConfigFrame.ReadFromW(buf, conf, runf, nets)
        ROOT.TSnIOConfigFrame.BytesReadFrom(buf, conf, runf, nets, ciov)
        if (getVerbosity()>=vtype.kInfo):
            printout(vtype.kInfo, "Config Frame IO version = {0}".format(ciov))
            runf.Print();
            conf.Print();
            nets.Print();
        return conf, runf, nets, mcode, mlen, ciov
    else:
        printout(vtype.kError, 'Buffer does not contain config '\
            '(or is an old version with no header).')
    return None, None, None, None, None

def readConfigFile(infname):
    if (infname!=None and infname!=''):
        with open(infname,'rb') as inf:
            fbuf  = inf.read()
            b = ROOT.TSnIOBuffer(fbuf, len(fbuf))
            conf, runf, nets, mcode, mlen, ciov = readConfig(b)
            if (conf!=None):
                printout(vtype.kInfo, 'read config from {0}'.format(infname))
            return conf, runf, nets, mcode, mlen, ciov
    return None, None, None, None, None


if __name__=="__main__":
    main()
