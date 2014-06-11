import sys
import fileinput
import datetime
import calendar
import array
import ROOT

from AriUtils import *

ROOT.gSystem.Load("$SNS/lib/libdbdat")
ROOT.gSystem.Load("$SNS/lib/libonline")

CONF_IOVERS_KEY="!CONF_IOVERS"
confIOvers=-1

dptypes = enum( kSD="SDcard", kIrid="Iridium", kAfar="Afar", kUSB="USB" )

pmtypes = enum( kAmpsDatTak="AmpsDatTak",
                kCardDatTak="CardDatTak",
                kIridDatTak="IridDatTak",
                kAfarDatTak="AfarDatTak",
                kAmpsComWin="AmpsComWin",
                kCardComWin="CardComWin",
                kIridComWin="IridComWin",
                kAfarComWin="AfarComWin" )

rmtypes = enum( kSingleSeq='SingleSeq',
                kCountPower='CountPower' )

sdtypes = enum( kAllFiles='AllFiles',
                kTimeout='CloseOnTimeout',
                kDelete='DeleteIfReceived',
                kForceSBD='ForceSBDtoSendFileData',
                kHnShBefSendDat='HandshakeBeforeSendData',
                kDefault='DefaultSendData')

ckeys = enum( kRun='Run',
              kFirstEvt='FirstEvt',
              kFirstSeq='FirstSeq',
              kConfLbl='ConfLabel',
              kDateBuilt='DateBuilt', # number of seconds since epoch in UTC
              kStreamHiLo='StreamHiLoPlas', # True or False
              kWvLoseLSB='WvLoseLSB',
              kWvLoseMSB='WvLoseMSB',
              kWvBaseline='WvBaseline',
              kDatPackList='PackDatFor', # see dptypes
              kAmpOnList='AmpsOn', # ch numbers for which amps should be on "0,1,2,3"
              kDacsList='DACS', # list of dac values for one channel
              kDacEnt='Dac', # list of "chan#, dac#, dacVal". one for each of the 16 DACs
              kPlaList='Plas', # list of PLA strings
              kMajLog='NumCardsMajLog',
              kThmTrgOn='ThermalTrigOn', # 'True' or 'False'
              kForced='ForcedPeriod',
              kHeartbeat='HeartbeatPeriod',
              kThrottle='ThrottlePeriodms',
              kPower='PowerOnFor',
              kBatVoltLP='BatVoltLowPwr', # deprecated
              kBatVoltToLP='BatVoltToLowPwr',
              kBatVoltFromLP='BatVoltFromLowPwr',
              kVoltCheckPeriod='VoltCheckPeriod',
              kCommWinPer='CommWinPeriod',
              kCommWinDur='CommWinDuration',
              kCommWinSendEvents='CommWinSendEvents', # number representing how many events to send back
              kCommWinSendData='CommWinSendData', # bit word representing how to send data back (see sdtypes)
              kCommWinPerLP='CommWinPeriodLowPwr',
              kCommWinDurLP='CommWinDurationLowPwr',
              kCommWinConnectTO='CommWinConnectTOmins',
              kCommWinListenTO='CommWinListenTOmins',
              kWatchdog='WatchdogPeriod',
              kEvtsPerSeq='EvtsPerSeq',
              kRunMode='RunMode',
              kRemoteServer='RemoteServer',
              kRemotePort='RemotePort',
              kStationIP='StationIP',
              kStationMask='StationMask',
              kStationGate='StationGateway' )

def checkConfFilename(fn):
    infspl = fn.rsplit('.',1)
    if (infspl[1]!='txt' and infspl[1]!='TXT'):
        print 'expect a .txt or .TXT file'
        return False, infspl
    else:
        return True, infspl

def getDatFilename(infspl):
    outfn  = infspl[0]
    if (infspl[1].isupper()):
        outfn += '.DAT'
    else:
        outfn += '.dat'
    return outfn

def getRootFilename(infspl):
    routfn = infspl[0]
    routfn += '.root'
    return routfn

def getSendDataBit(val):
    if (val==sdtypes.kDefault):
        return ROOT.TSnCommWinSet.kUseBits
    elif (val==sdtypes.kAllFiles):
        return ROOT.TSnCommWinSet.kAllFiles
    elif (val==sdtypes.kTimeout):
        return ROOT.TSnCommWinSet.kTimeout
    elif (val==sdtypes.kDelete):
        return ROOT.TSnCommWinSet.kDelete
    elif (val==sdtypes.kHnShBefSendDat):
        return ROOT.TSnCommWinSet.kHnShBefSendDat
    elif (val==sdtypes.kForceSBD):
        return ROOT.TSnCommWinSet.kForceSBDdata
    else:
        print 'getSendDataBit: unknown value {0}'.format(val)
        sys.exit()


def setConfVal(key, val, runf, conf, nets, ciov):
    if   (key==ckeys.kRun):
        runf.SetRun(int(val))
    elif (key==ckeys.kFirstEvt):
        if ((ciov<0) or 
            (ciov>castHeaderCode(ROOT.TSnIOConfigFrame.k2012to2013IOVers))):
            print 'Old FirstEvt entry no longer used.'
            print 'Instead, the first seq may be specified with FirstSeq.'
            sys.exit()
        else:
            runf.SetObsFirstEvt(int(val))
    elif (key==ckeys.kFirstSeq):
        if ((ciov<0) or 
            (ciov>castHeaderCode(ROOT.TSnIOConfigFrame.k2012to2013IOVers))):
            runf.SetFirstSeq(int(val))
        else:
            print "Cannot use FirstSeq with config i/o [{0}]. Try FirstEvt."\
                .format(ciov)
            sys.exit()
    elif (key==ckeys.kConfLbl):
        runf.SetConfLabel(val)
        conf.SetLabel(val)
    elif (key==ckeys.kDateBuilt):
        conf.SetDateBuilt(int(val))
    elif (key==ckeys.kStreamHiLo):
        if (val=='True'):
            conf.SetStreamHiLoPlas(True)
        elif (val=='False'):
            conf.SetStreamHiLoPlas(False)
        else:
            print "Cannot set StreamHiLoPlas to value [{0}]".format(val)
    elif (key==ckeys.kWvLoseLSB):
        conf.SetWvLoseLSB(int(val))
    elif (key==ckeys.kWvLoseMSB):
        conf.SetWvLoseMSB(int(val))
    elif (key==ckeys.kWvBaseline):
        conf.SetWvBaseline(int(val))
    elif (key==ckeys.kDatPackList):
        if   (val==dptypes.kSD):
            conf.EnablePackDatFor(ROOT.TSnDAQConfig.kSDcard)
        elif (val==dptypes.kIrid):
            conf.EnablePackDatFor(ROOT.TSnDAQConfig.kIrid)
        elif (val==dptypes.kAfar):
            conf.EnablePackDatFor(ROOT.TSnDAQConfig.kAfar)
        elif (val==dptypes.kUSB):
            conf.EnablePackDatFor(ROOT.TSnDAQConfig.kUSB)
    elif (key==ckeys.kAmpOnList):
        for ch in val.split(','):
            conf.SetAmpOn(int(ch))
    elif (key==ckeys.kDacEnt):
        print 'Old dac entry format no longer used.'
        print 'Use new format:'
        print 'DACS=channel:dac #0, dac #1, dac #2, dac #3'
        print 'for example:'
        print 'DACS=0:3100,2200,2200,3100'
        sys.exit()
        #vdc = val.split(',')
        #if (len(vdc)!=3):
        #    print "Can't set DAC with string [{0}]".format(val)
        #else:
        #    [ch, dc, dv] = vdc
        #    conf.SetDac(int(ch), int(dc), int(dv))
    elif (key==ckeys.kDacsList):
        chan,dcs = val.split(':')
        dacs   = dcs.split(",")
        ch     = int(chan)
        if (len(dacs)==4 and ch<4 and ch>=0):
            for x in range(0,4):
                conf.SetDac(int(ch), x, int(dacs[x]))
        else:
            print 'invalid channel or number of dacs:'
            print val
            sys.exit()
    elif (key==ckeys.kPlaList):
        for pla in val.split(','):
            conf.AddPla(pla.strip())
    elif (key==ckeys.kMajLog):
        conf.SetNumCardsMajLog(int(val))
    elif (key==ckeys.kThmTrgOn):
        if (val=='True'):
            conf.SetThermalTriggering(True)
        elif (val=='False'):
            conf.SetThermalTriggering(False)
        else:
            print "Can't set thermal triggering to [{0}]".format(val)
            sys.exit()
    elif (key==ckeys.kForced):
        if ((ciov<0) or (ciov>3)):
            conf.SetForcedPeriod(float(val))
        else:
            print "Forced period as integer not used in any data taking season."
            sys.exit()
    elif (key==ckeys.kHeartbeat):
        conf.SetHeartbeatPeriod(int(val))
    elif (key==ckeys.kThrottle):
        conf.SetThrottlePeriodms(int(val))
    elif (key==ckeys.kPower):
        if (val==pmtypes.kAmpsDatTak):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kAmpsDatTak)
        elif (val==pmtypes.kCardDatTak):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kCardDatTak)
        elif (val==pmtypes.kIridDatTak):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kIridDatTak)
        elif (val==pmtypes.kAfarDatTak):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kAfarDatTak)
        elif (val==pmtypes.kAmpsComWin):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kAmpsComWin)
        elif (val==pmtypes.kCardComWin):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kCardComWin)
        elif (val==pmtypes.kIridComWin):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kIridComWin)
        elif (val==pmtypes.kAfarComWin):
            conf.EnablePowerModeFor(ROOT.TSnDAQConfig.kAfarComWin)
    elif (key==ckeys.kBatVoltLP):
        if ((ciov<0) or 
            (ciov>castHeaderCode(ROOT.TSnIOConfigFrame.k2012to2013IOVers))):
            print 'Old setting of battery voltage low power switch'
            print 'no longer used. Use the new parameters:'
            print ckeys.kBatVoltToLP
            print ckeys.kBatVoltFromLP
            sys.exit()
        else:
            print "Note: using old style config (iovers {0})".format(ciov)
            conf.SetObsBatVoltToLowPwrShort(int(val))
    elif (key==ckeys.kBatVoltToLP):
        conf.SetBatVoltToLowPwr(int(val))
    elif (key==ckeys.kBatVoltFromLP):
        conf.SetBatVoltFromLowPwr(int(val))
    elif (key==ckeys.kVoltCheckPeriod):
        conf.SetVoltCheckPeriod(int(val))
    elif (key==ckeys.kCommWinPer):
        conf.SetCommWinPeriod(int(val))
    elif (key==ckeys.kCommWinDur):
        conf.SetCommWinDuration(int(val))
    elif (key==ckeys.kCommWinSendEvents):
        conf.SetCommWinSendData(int(val))
    elif (key==ckeys.kCommWinSendData):
        sd = conf.GetCommWinSendData()
        if (sd>=0):
            sd = ROOT.TSnCommWinSet.kUseBits
        nb = getSendDataBit(val)
        if ( (nb==ROOT.TSnCommWinSet.kHnShBefSendDat) and
             (ciov==castHeaderCode(ROOT.TSnIOConfigFrame.k2012to2013IOVers)) ):
            print "Handshake before sending data ({0}) not used in config "\
                "version [{1}]".format(sdtypes.kHnShBefSendDat, ciov)
            sys.exit()
        conf.SetCommWinSendData(sd | nb)
    elif (key==ckeys.kCommWinPerLP):
        conf.SetCommWinPerLowPower(int(val))
    elif (key==ckeys.kCommWinDurLP):
        conf.SetCommWinDurLowPower(int(val))
    elif (key==ckeys.kCommWinConnectTO):
        to = int(val)
        if ( (to>=0) and (to<=255) ):
            conf.SetCommWinConnectTOmin(to)
        else:
            print "Invalid value for timeout in minutes:"
            print to
            sys.exit()
    elif (key==ckeys.kCommWinListenTO):
        to = int(val)
        if ( (to>=0) and (to<=255) ):
            conf.SetCommWinListenTOmin(to)
        else:
            print "Invalid value for timeout in minutes:"
            print to
            sys.exit()
    elif (key==ckeys.kWatchdog):
        conf.SetWatchdogPeriod(int(val))
    elif (key==ckeys.kEvtsPerSeq):
        runf.SetEvtsPerSeq(int(val))
    elif (key==ckeys.kRunMode):
        if (val==rmtypes.kSingleSeq):
            conf.EnableRunModeBit(ROOT.TSnDAQConfig.kSingleSeqBit)
        elif (val==rmtypes.kCountPower):
            conf.EnableRunModeBit(ROOT.TSnDAQConfig.kCountPowerBit)
    elif (key==ckeys.kRemoteServer):
        nets.SetRemoteServer(val)
    elif (key==ckeys.kRemotePort):
        nets.SetRemotePort(int(val))
    elif (key==ckeys.kStationIP):
        nets.SetLocalIP(val)
    elif (key==ckeys.kStationMask):
        nets.SetLocalMask(val)
    elif (key==ckeys.kStationGate):
        nets.SetLocalGateway(val)

def getConfObjs():
    return ROOT.TSnDAQConfig(), ROOT.TSnRunInfo(), ROOT.TSnNetAdrSet()

def setConfDate(conf):
    utcnow = datetime.datetime.utcnow()
    conf.SetDateBuilt(calendar.timegm(utcnow.utctimetuple()))

def processConfTextFile(infn,runf,conf,nets):
    cwsd = False
    cwse = False
    confIOvers = -1
    for line in fileinput.input(infn):
        l = line.strip()
        if (len(l)>0 and l[0]!='#'):
           kv = l.split('=')
           if (len(kv)!=2):
               print "Can't parse line [{0}]".format(l)
               sys.exit()
           if (kv[0] == CONF_IOVERS_KEY):
               confIOvers = int(kv[1])
               if (confIOvers < 
                   castHeaderCode(ROOT.TSnIOConfigFrame.k2012to2013IOVers)):
                   print "IO version [{0}] is earlier than used in any "\
                       "data taking season ({1})! "\
                       "Refuse to create such a config."\
                       .format(confIOvers,
                               castHeaderCode(
                               ROOT.TSnIOConfigFrame.k2012to2013IOVers))
           else:
               if (kv[0] == ckeys.kCommWinSendEvents):
                   cwse = True
               if (kv[0] == ckeys.kCommWinSendData):
                   cwsd = True
               if (cwse and cwsd):
                   print "Can't set both SendEvents and SendData"
                   sys.exit()
               setConfVal(kv[0],kv[1],runf,conf,nets, confIOvers)
    return confIOvers

def getConfigFrom(infn):
    conf, runf, nets = getConfObjs()
    setConfDate(conf)
    ciov = processConfTextFile(infn,runf,conf,nets)
    if (getVerbosity()>=vtype.kInfo):
        runf.Print()
        conf.Print()
        nets.Print()
    return conf, runf, nets, ciov

def getBinStr(conf, runf, nets, ciov):
    cp = ROOT.TSnCommConfPack(conf, runf, nets, ciov)
    ba = ROOT.TSnIOBuffer(cp)
    ba.SetPosVal(0)
    return getDataFromBuf(ba)

def writeToDatFile(outfn, conf, runf, nets, ciov):
    print "Writing {0}".format(outfn)
    with open(outfn,"wb") as outf:
        outf.write( getBinStr(conf, runf, nets, ciov) )

def writeToRootFile(outfn, conf, runf, nets):
    routf = ROOT.TFile(routfn,'recreate')
    routf.cd()
    runf.Write()
    conf.Write()
    nets.Write()


def readConfTextAndWriteDat(infn):
    ok, infspl = checkConfFilename(infn)
    if (ok):
        conf, runf, nets, ciov = getConfigFrom(infn)
        odn = getDatFilename(infspl)
        writeToDatFile(odn, conf, runf, nets, ciov)
        return conf, runf, nets, odn, ciov
    return None

### write the config file
if (__name__ == "__main__"):
    if (len(sys.argv)<2):
        print 'No file specified.'
        sys.exit()

    if (len(sys.argv)>2):
        print 'Expect only one file.'
        sys.exit()
        
    obs=None
    if (USE_LOG):
        obs = log.startLogging(sys.stdout)
        
    readConfTextAndWriteDat(sys.argv[1])
        
    if (obs!=None):
        printout(vtype.kDebug,"Stopping logger")
        obs.stop()
