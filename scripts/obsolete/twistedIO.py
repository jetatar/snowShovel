import sys, os
import datetime
import array
import struct
import time

from twisted.internet import protocol, defer, threads

from AriUtils import *
import AriFileTransferOpts

from readConfigPyRoot import readConfigFile

import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat")
ROOT.gSystem.Load("$SNS/lib/libevdat")
ROOT.gSystem.Load("$SNS/lib/libonline")

HEADER_LEN = ROOT.TSnIOHeaderFrame.SizeOf()
MAXMESGLEN = 1300 # above this, we know it's an error
DEL_CONF_MAGIC_STR = "fj2io32FIJ(#jd;;.O@++/]ewavk2[49ojv"
MAX_EVT_RATE = 11
FROM_ADDRESS = "arianna.icicle@gmail.com"
TMP_MAIL_FILE = "/tmp/temp.ari.msg"
DEFCONF_DAT_NM = "DEFCONF.DAT"

ftype=enum(kConfig=0, kMbedBin=10, kNone=100)

# switch: fraction of events or power readings received
#         out of total sent by station, above which is
#         considered an acceptable partial receipt
partialOkFrc = 0.90
# switch: directory in which to put raw status update files
rstatDir="rawStatus"
# switch: directory in which to put binary data files
binDir="binFiles"
# switch: name of file to save bytes tranfered timing ntuple
#         this is only used if verb>=kFileTimer
filetimerNToutfn = "FileTimerNtuple.txt"
# switch: update num events received every...
updEvtCount=25
# switch: read in config file and make sure it's a config
doCheckConf=False
# switch: thermals off config to send
thmoffConfn="thermsOff.dat"
# switch: who to email when things to wrong
emergContacts="cjreed@uci.edu, jtatar@uci.edu, barwick@HEP.ps.uci.edu"
# switch: verbosity level (see vtype enum)
setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kFileTimer)
#setVerbosity(vtype.kDebug)
#setVerbosity(vtype.kDebugData)


def uponFail(msg, loc):
    printout(vtype.kError,'FAIL <{0}>: {1}'.format(loc, msg))
    return msg

class stationProtocol(protocol.Protocol):
    def GetInConfName(self):
        # should be overridden
        # return the name of the config file to send to the station
        return ''    

    def IsTalkingTo(self, mac):
        # override if only want to talk to certain stations
        return False

    def IsDEFCONF(self):
        # override if want to be able to send a new DEFCONF.DAT to
        # store on the MBED
        return False
    
    def setup(self):
        self.peer            = None
        # for buffering and reading incoming data
        self.msgtype         = None
        self.needBytes       = HEADER_LEN # this is vital -- we wait for a header first
        self.buff            = []
        # for file transfer timing
        self.start   = None
        self.fstart  = None
        self.tbytes  = 0
        self.fbytes  = 0
        if (getVerbosity()>=vtype.kFileTimer):
            self.ntof = open(filetimerNToutfn,"wt")
        # for sending
        self.doSendConf = False
        self.doSendBinf = False
        self.sendFtype  = ftype.kNone
        self.turnOffThermals = False
        self.turnOffThmHook  = False
        # for an incoming event
        self.inWvPack   = None
        # for an incoming file name
        self.inFilen    = None
        AriFileTransferOpts.fto.setStage(AriFileTransferOpts.stage.kInput,
                                         withPrompt=False)
        self.filesStarted = False
        # the output binary file
        self.binFile    = None
        # for saving a raw status update
        self.outStatFn   = None
        self.statFile    = None
        self.whenGotStatusEvt = None
        self.whenGotStatusPow = None
        # for checking if enough (file) data arrived
        self.numInEvts   = 0
        self.numInPows   = 0
        self.numPowsSent = 0
        self.numEvtsSent = 0

    #
    # utilities
    #
    def sendTrgEmail(self, inStatus):
        msg = "To: {0}\r\n"\
              "From: {1}\r\n"\
              "Subject: HIGH RATES on station {2}\r\n"\
              "\r\n\r\n"\
              "Rates on station {2} were reportedly {3} Hz thermal triggering "\
              "and {4} Hz event saving as of {5}, during run {6}, seq {7}. SD "\
              "card usage at {8} kB.\r\n"\
              "\r\nThermal triggers have been disabled.\r\n\r\n".format(
            emergContacts, FROM_ADDRESS, 
            inStatus.GetMacAdrAsStr(),
            inStatus.GetThmRate(),
            inStatus.GetEvtRate(),
            inStatus.GetStatusTime().AsString("s"),
            inStatus.GetRunNum(),
            inStatus.GetSeqNum(),
            inStatus.GetTotKB()
            )
        with open(TMP_MAIL_FILE,"w") as tf:
            tf.write(msg)

        def dosendtrgemail():
            os.system("ssmtp -t < {0}".format(TMP_MAIL_FILE))
        def ignoreRes(x):
            pass
        d = threads.deferToThread(dosendtrgemail)
        d.addBoth(ignoreRes)
        
    #
    # Binary file output
    #

    def getBinOutf(self, fn, tag=''):
        if (self.binFile!=None):
            return self.binFile
        else:
            if (fn!=None):
                oftag = '{0}/{1}{2}'.format(
                    binDir, splitFilenameExt(fn)[0], tag)
                ofn = "{0}.dat".format(oftag)
                # prevent an overwrite
                seq = 1
                while os.path.exists(ofn):
                    ofn  = "{0}_{1}.dat".format(oftag, seq)
                    seq += 1
                self.binFile = open(ofn, 'wb', 0)
                return self.binFile
            else:
                printout(vtype.kError, 'getBinOutf: no fn')
                return None
    
    def getStatOutf(self, fn):
        if (self.statFile!=None):
            return self.statFile
        else:
            if (fn!=None):
                oftag = '{0}/{1}'.format(rstatDir, splitFilenameExt(fn)[0])
                ofext = splitFilenameExt(fn)[1]
                ofn = '{0}.{1}'.format(oftag, ofext)
                # prevent an overwrite
                seq = 1
                while os.path.exists(ofn):
                    ofn  = "{0}_{1}.{2}".format(oftag,seq,ofext)
                    seq += 1
                self.statFile = open(ofn, 'wb', 0)
                return self.statFile
            else:
                printout(vtype.kError, 'getStatOutf: no fn')
                return None
    
    def writeBinary(self, binf, data, mcode, mlen):
        # writes the header plus the contents of data
        # to the specified binary file
        # only the length of data specified in the current header
        # is written
        if (len(data)>=mlen):
            om = array.array('c', '0'*HEADER_LEN)
            ROOT.TSnIOHeaderFrame.WriteTo(om, mcode, mlen)
            x = b''.join(y for y in om)
            if (binf!=None):
                printout(vtype.kDebug, 'writing to binf')
                if (mcode!=castHeaderCode(
                        ROOT.TSnIOHeaderFrame.kFileHeadrCode)):
                    binf.write(x) # the header for this block
                binf.write(data[0:mlen])
            else:
                printout(vtype.kError, 'no bin file to write to')
        else:
            printout(vtype.kError,'len(data)={0} but expected mlen={1}'.\
                         format(len(data), mlen))

    def writeToBinf(self, data, mcode, mlen):
        # writes the header plus the contents of data
        # to self.inFilen
        # only the length of data specified in the current header
        # is written
        self.writeBinary(self.getBinOutf(self.inFilen), data, mcode, mlen)

    def writeToStatf(self, data, mcode, mlen):
        # write the header plus the contents of data
        # to self.inStatFn
        # only the length of data specified in the current header
        # is written
        self.writeBinary(self.getStatOutf(self.outStatFn), data, mcode, mlen)

    def writeTupToStatf(self, dtupl):
        # to be called by a deferred (whenGotStatusEvt, whenGotStatusPow)
        return self.writeToStatf(dtupl[0], dtupl[1], dtupl[2])

    #
    # Input handlers
    #

    def handleStatusUpd(self, msg, mcode, mlen):
        inStatus = ROOT.TSnStatusUpdate()
        inEvent  = ROOT.TSnEvent()
        # as of V2, the status doesn't actually contain the event
        # but ReadFrom wants an event anyway, in case we receive
        # a status V1
        br = ROOT.TSnIOStatusFrame.BytesReadFrom(
            msg,inStatus,inEvent,len(msg))
        if (br==mlen):
            # message reading successful
            printout(vtype.kDebug,
                     'status complete: msg len {0}'.format(len(msg)))
            # store the wave pack settings in case an event comes next
            self.inWvPack = { 'LSB':inStatus.GetWvLoseLSB(),
                              'MSB':inStatus.GetWvLoseMSB(),
                              'base':inStatus.GetWvBaseline() }
            # save the raw status update
            self.outStatFn = 'status.{0}.{1:08d}-{2:06d}.dat'.format(
                inStatus.GetMacAdrAsStr(),
                inStatus.GetStatusTime().GetDate(),
                inStatus.GetStatusTime().GetTime())
            self.statFile = None
            # TODO: make a TSnIOStatusFile class and use it to write this
            # status file (with a normal data file header)
            self.writeToStatf(msg,mcode,mlen)
            # check the version number of the status
            # only v1 has an event
            evtInStatus = getUChar(msg)==1
            if (evtInStatus):
                inEvent.CalcCRC()
            else:
                if (self.whenGotStatusEvt==None):
                    self.whenGotStatusEvt = defer.Deferred()
                    self.whenGotStatusEvt.addCallback(self.writeTupToStatf)
                    self.whenGotStatusEvt.addErrback(uponFail, 
                                                     'whenGotStatusEvt')
            # put the voltage in the status file when it comes
            if (self.whenGotStatusPow==None):
                self.whenGotStatusPow = defer.Deferred()
                self.whenGotStatusPow.addCallback(self.writeTupToStatf)
                self.whenGotStatusPow.addErrback(uponFail,'whenGotStatusPow')
            if (getVerbosity()>=vtype.kInfo):
                inStatus.Print()
                if ( evtInStatus ): # check i/o version of status
                    inEvent.Print()
            # send the config
            printout(vtype.kDebug,"status rate {0}, max rate {1}".format(
                    inStatus.GetEvtRate(), MAX_EVT_RATE))
            if ( inStatus.GetEvtRate() > MAX_EVT_RATE ):
                printout(vtype.kWarn,"Big rate!! Sending email.")
                self.doSendConf = True
                self.turnOffThermals = True
                self.turnOffThmHook = True # hook for SBD
                self.sendTrgEmail(inStatus)
            if ( self.sendFtype==ftype.kConfig or self.sendFtype==ftype.kNone ):
                if not self.IsTalkingTo(inStatus.GetMacAdrAsStr()):
                    self.sendFtype=ftype.kNone
                self.doSendConf = True
            elif ( (self.sendFtype==ftype.kMbedBin)
                   and self.IsTalkingTo(inStatus.GetMacAdrAsStr()) ):
                self.doSendBinf = True
    
    def handleConfig(self, msg, mcode, mlen):
        inConf = ROOT.TSnDAQConfig()
        inRunf = ROOT.TSnRunInfo()
        inNets = ROOT.TSnNetAdrSet()
        br = ROOT.TSnIOConfigFrame.BytesReadFrom(
            msg, inConf, inRunf, inNets)
        if (br==mlen):
            # store wave pack settings for incoming events
            self.inWvPack = { 'LSB':inConf.GetWvLoseLSB(),
                              'MSB':inConf.GetWvLoseMSB(),
                              'base':inConf.GetWvBaseline() }
            self.writeToBinf(msg, mcode, mlen)

    def handleFileHeader(self, msg, mcode, mlen):
        printout(vtype.kDebug, 'handleFileHeader:')
        printout(vtype.kDebug, 'len(msg)={0}, mlen={1}'.\
                     format(len(msg),mlen))
        printHex(vtype.kDebugData, bytearray(msg))

        inFileRunInfo = ROOT.TSnRunInfo()
        inFileMbed    = ROOT.TSnCompMbed()
        #self.inFileSeq     = array.array("H",[0]) # doesn't work with 5.34.01
        inFileSeq     = ROOT.Long()
        inFilePower   = ROOT.TSnPowerReading()
        br = ROOT.TSnIODataFile.PyReadFileHeader(msg,
                                                 inFileRunInfo,
                                                 inFileMbed,
                                                 inFileSeq,
                                                 inFilePower)
        if (br==mlen):
            self.writeToBinf(msg, mcode, mlen)
            printout(vtype.kDebug, 'read {0} bytes'.format(br))
            printout(vtype.kInfo, 'Receiving run {r}, seq {s} from MBED {m}'.\
                         format(r=inFileRunInfo.GetRun(),
                                s=inFileSeq,
                                m=inFileMbed.GetMAC()))
            printout(vtype.kInfo, str(datetime.datetime.now()))

    def handlePower(self, msg, mcode, mlen):
        self.numInPows += 1
        if (self.whenGotStatusPow!=None):
            # we've been waiting for the status update power reading.
            printout(vtype.kDebug, "callback for whenGotStatusPow")
            d, self.whenGotStatusPow = self.whenGotStatusPow, None
            d.callback( (msg, mcode, mlen) )
            # print the power
            if (getVerbosity()>=vtype.kInfo):
                power = ROOT.TSnPowerReading()
                br = ROOT.TSnIOPowerFrame.BytesReadFrom(msg, power)
                power.Print()
                power = None
        else:
            printout(vtype.kDebug, 'saving power reading to event binf')
            self.writeToBinf(msg, mcode, mlen)

    def handleEvent(self, msg, mcode, mlen):
        self.numInEvts += 1
        if (self.whenGotStatusEvt!=None):
            # we've been waiting for the status update event.
            printout(vtype.kDebug, "callback for whenGotStatusEvt")
            d, self.whenGotStatusEvt = self.whenGotStatusEvt, None
            d.callback( (msg, mcode, mlen) )
        else:
            printout(vtype.kDebug, 'saving to event binf')
            self.writeToBinf(msg, mcode, mlen)
        if ( (getVerbosity()>=vtype.kInfo) and
             ((self.numInEvts % updEvtCount)==0) ):
            #sys.stderr.write("got {0} events           \r"
            #                 .format(self.numInEvts))
            log.msg("got {0} events".format(self.numInEvts))
        if (self.doSendConf):
            self.sendConf(msg, mcode, mlen)
        elif (self.doSendBinf):
            self.sendBinf(msg, mcode, mlen)

    def handleFilename(self, msg, mcode, mlen):
        self.inFilen = str(msg[:mlen])
        printout(vtype.kInfo, 'got filename [{0}]'.format(self.inFilen))
        # get ready to receive file
        self.numInEvts = 0
        self.numInPows = 0
        self.fbytes    = 0
        self.fstart    = time.time()
        if (self.start == None):
            self.start = time.time()
        self.binFile   = None
        binf = self.getBinOutf(self.inFilen)
        if (self.filesStarted==False):
            AriFileTransferOpts.fto.setStage(AriFileTransferOpts.stage.kInput)
        self.filesStarted=True

    def handleString(self, msg, mcode, mlen):
        printout(vtype.kUrgent, 'received string:')
        printout(vtype.kUrgent, msg[:mlen])
    
    def handleHeartbeat(self, msg, mcode, mlen):
        self.writeToBinf(msg, mcode, mlen)

    def handleReadFail(self, msg, mcode, mlen):
        printout(vtype.kWarn, 'station read file failed!')

    def handleNevtsSent(self, msg, mcode, mlen):
        try:
            self.numEvtsSent = getUInt(msg)
            printout(vtype.kInfo, 'Received {0}/{1} events'.\
                         format(self.numInEvts, self.numEvtsSent))
        except:
            self.numEvtsSent = None
            printout(vtype.kError, 'failed to read UInt from:')
            printHex(vtype.kError, bytearray(msg))

    def handleNpwrsSent(self, msg, mcode, mlen):
        try:
            self.numPowsSent = getUInt(msg)
            printout(vtype.kInfo, 'Received {0}/{1} powers'.\
                         format(self.numInPows, self.numPowsSent))
            if (getVerbosity()>=vtype.kFileTimer):
                dt  = time.time() - self.start
                dft = time.time() - self.fstart
                if (dt>0 and dft>0):
                    tr  = self.tbytes/dt
                    fr  = self.fbytes/dft
                    printout(vtype.kFileTimer, 
                             "average total rate = {0} Bps ({1} bytes / {2} secs)".
                             format(tr, self.tbytes, dt))
                    printout(vtype.kFileTimer, 
                             "this file ave rate = {0} Bps ({1} bytes / {2} secs)".
                             format(fr, self.fbytes, dft))
                    self.ntof.write("{0} {1} {2} {3} {4} {5}\r\n"
                                    .format(tr, self.tbytes, dt, 
                                            fr, self.fbytes, dft) )
                    self.ntof.flush()
        except:
            self.numPowsSent = None
            printout(vtype.kError, 'failed to read UInt from:')
            printHex(vtype.kError, bytearray(msg))

        # Npwrs is the last thing sent
        # so the handshake is sent from here
        self.sendDataRcvResponse()

    #
    # Output senders
    #

    def sendMsg(self, message):
        if (self.transport!=None):
            self.transport.write(message)
        else:
            printout(vtype.kError,"No transport to send message:")
            printHex(vtype.kError, bytearray(message))

    def getHandshake(self, hnShCode, hnShLen=0):
        om = array.array('c', '0'*HEADER_LEN)
        ROOT.TSnIOHeaderFrame.WriteTo(om, hnShCode, hnShLen)
        x = b''.join(y for y in om)
        return x
    
    def sendHandshake(self, hnShCode, hnShLen=0):
        x = self.getHandshake(hnShCode, hnShLen)
        printout(vtype.kInfo, 'sending handshake: {0}'.format(hnShCode))
        printHex(vtype.kDebug, bytearray(x))
        printout(vtype.kInfo, str(datetime.datetime.now()))
        self.sendMsg(x)

    def sendDataRcvResponse(self):
        notSent = True
        # first see if user wants a special handshake
        printout(vtype.kDebug,"[][][][][] fto={0}, hasinput={1}".format(
            AriFileTransferOpts.fto, AriFileTransferOpts.fto.hasInput()))
        if ( (AriFileTransferOpts.fto!=None) and
             (AriFileTransferOpts.fto.hasInput()) ):
            printout(vtype.kDebug,"()()()()() have user input!")
            # send the handshake the user wants no matter what
            opt, run = AriFileTransferOpts.fto.popInput()
            printout(vtype.kDebug,"()()()()() have user input! opt={0}, run={1}".\
                format(opt, run))
            if (opt==AriFileTransferOpts.hkeys.kStopTransfer):
                printout(vtype.kDebug, 'send kHnShOkStopCode')
                self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShOkStopCode)
                notSent = False
            elif (opt==AriFileTransferOpts.hkeys.kDelRun):
                try:
                    printout(vtype.kDebug, 'send kHnShOkDelRnCode')
                    x = self.getHandshake(ROOT.TSnIOHeaderFrame.kHnShOkDelRnCode,
                                          run)
                    m = b''.join( [x, b''.join(i for i in DEL_CONF_MAGIC_STR)] )
                    self.sendMsg(m)
                    notSent = False
                except:
                    printout(vtype.kError,
                             'Error sending delete run handshake with run={0}'\
                                 .format(run))
            elif (opt==AriFileTransferOpts.hkeys.kDelAll):
                printout(vtype.kDebug, 'send kHnShOkDelAlCode')
                x = self.getHandshake(ROOT.TSnIOHeaderFrame.kHnShOkDelAlCode)
                m = b''.join( [x, b''.join(i for i in DEL_CONF_MAGIC_STR)] )
                self.sendMsg(m)
                notSent = False
            else:
                printout(vtype.kError,'Unknown transfer option [{0}]. Ignoring.'.\
                             format(opt))
        # if not, determine proper handshake based on receipt of file
        if ( notSent ):
            if ( (self.numPowsSent==self.numInPows) and
                 (self.numEvtsSent==self.numInEvts) ):
                printout(vtype.kDebug, 'send kHnShOkCompleCode')
                self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShOkComplCode)
            elif ( (self.numPowsSent<self.numInPows) or
                   (self.numEvtsSent<self.numInEvts) ):
                printout(vtype.kDebug, 'got MORE pow/evts than station sent??')
                printout(vtype.kDebug, 'send kHnShFailPrtCode')
                self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShFailPrtCode)
            else:
                rcvPowsFr = 1
                rcvEvtsFr = 1
                if (self.numPowsSent>0):
                    rcvPowsFr = float(self.numInPows)/float(self.numPowsSent)
                if (self.numEvtsSent>0):
                    rcvEvtsFr = float(self.numInEvts)/float(self.numEvtsSent)
                if ( (rcvPowsFr>partialOkFrc) and
                     (rcvEvtsFr>partialOkFrc) ):
                    printout(vtype.kDebug, 'send kHnShOkPartlCode')
                    self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShOkPartlCode)
                elif ( (self.numInPows>0) and (self.numInEvts>0) ):
                    printout(vtype.kDebug, 'send kHnShFailPrtCode')
                    self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShFailPrtCode)
                else:
                    printout(vtype.kDebug, 'send kHnShFailNonCode')
                    self.sendHandshake(ROOT.TSnIOHeaderFrame.kHnShFailNonCode)
        # reset for the next file
        self.numInPows = 0
        self.numInEvts = 0
        self.numPowsSent = 0
        self.numEvtsSent = 0
        if (self.binFile!=None):
            self.binFile.close()
            self.binFile = None

    def sendBinf(self, msg, mcode, mlen):
        if (self.doSendBinf and (self.sendFtype==ftype.kMbedBin) ):
            try:
                printout(vtype.kWarn,"SENDING NEW BIN FILE!")
                # TODO - change GetInConfName to reflect input file types
                lfname = self.GetInConfName()
                bfname = None
                inext  = splitFilenameExt(lfname)[1]
                if ( ((inext=='dat') or (inext=='DAT'))
                     and self.IsDEFCONF() ):
                    bfname = DEFCONF_DAT_NM
                else:
                    bfname = lfname[lfname.rfind('/')+1:]
                    if (len(bfname.split('.')[0])>8):
                        printout(vtype.kWarn,"Making filename conform to 8.3!")
                        bfname = ''.join( [ (bfname.split('.')[0])[0:8],
                                            '.',
                                             bfname.split('.')[1] ] )
                printout(vtype.kInfo,"Local file={0}".format(lfname))
                printout(vtype.kInfo,"Filename={0}".format(bfname))
                if (bfname==None):
                    printout(vtype.kError, "Could not make 8.3 filename"\
                                 "for [{0}]".format(lfname))
                    self.doSendBinf = False
                    return
                # send the filename
                hdar = array.array('c','0'*HEADER_LEN)
                ROOT.TSnIOHeaderFrame.WriteTo(hdar, 
                   ROOT.TSnIOHeaderFrame.kMbedFilenameCode, len(bfname))
                x = b''.join(hdar)
                fnmMsg = b''.join( [ x, b''.join(z for z in bfname) ] )
                printout(vtype.kDebug,"filename header")
                printHex(vtype.kDebugData,bytearray(x))
                #self.sendMsg(x)
                # now send the file (and calc the CRC)
                with open(lfname,"rb") as bf:
                    # just load the whole thing into memory
                    # hopefully it's not too big!
                    crc = ROOT.Long()
                    ab = bf.read()
                    for b in ab:
                        crc = ROOT.NSnCRCUtils.update_crc32_xfer(crc, b)
                    printout(vtype.kDebug,"crc = {0}".format(crc))
                    ROOT.TSnIOHeaderFrame.WriteTo(hdar,
                       ROOT.TSnIOHeaderFrame.kMbedFileCode, len(ab))
                    printout(vtype.kDebug,"bytes in file = {0}".format(len(ab)))
                    x = b''.join(hdar)
                    printout(vtype.kDebug,"file bytes...")
                    printHex(vtype.kDebugData,bytearray(x))
                    printHex(vtype.kDebugData,bytearray(ab))
                    fbyMsg = b''.join( [x, b''.join(b for b in ab)] )
                    #self.sendMsg(x)
                    #self.sendMsg(ab)
                    #printout(vtype.kDebug,"sent file")
                    # now send the CRC
                    ROOT.TSnIOHeaderFrame.WriteTo(hdar,
                       ROOT.TSnIOHeaderFrame.kMbedFileChksCode, crc)
                    printout(vtype.kDebug,"send checksum {0}".format(crc))
                    printHex(vtype.kDebugData,bytearray(hdar))
                    #self.sendMsg( b''.join(hdar) )
                    crcMsg = b''.join(hdar)
                    totMsg = b''.join( [fnmMsg, fbyMsg, crcMsg] )
                    printout(vtype.kDebug,"sending binary file message")
                    printHex(vtype.kDebugData,bytearray(totMsg))
                    self.sendMsg(totMsg)
            except IOError as e:
                printout(vtype.kDebug, e)
                printout(vtype.kInfo, 'Not sending any MBED binary file.')
            
            self.doSendBinf = False
        else:
            printout(vtype.kDebug,
                     'not sending binf: doSendBinf={0}, sendFtype={1}'.format(
                    self.doSendBinf, self.sendFtype))

    def sendNoConfSignal(self):
        printout(vtype.kInfo, 'Not sending any new config.')
        hd = array.array('c', '0'*HEADER_LEN)
        ROOT.TSnIOHeaderFrame.WriteTo(hd,
                                      ROOT.TSnIOHeaderFrame.kNoConfigCode, 0)
        x = b''.join(hd)
        printout(vtype.kDebugData, 'sending no config signal:')
        printHex(vtype.kDebugData, bytearray(x))
        self.sendMsg(x)
        self.doSendConf = False

    def sendConf(self, msg, mcode, mlen):
        if ( (self.doSendConf and (self.sendFtype==ftype.kConfig))
             or self.turnOffThermals ):
            try:
                cname = self.GetInConfName()
                if (self.turnOffThermals):
                    cname = thmoffConfn
                if (doCheckConf):
                    outConf, outRunf, outNets, mcode, mlen \
                        = readConfigFile(cname)
                    if ( (outConf != None) and 
                         (outRunf != None) and
                         (outNets != None) and
                         (mcode   != None) and
                         (mlen    != None) ):
                        hdar = array.array('c', '0'*HEADER_LEN)
                        ROOT.TSnIOHeaderFrame.WriteTo(hdar, mcode, mlen)
                        cfar = array.array('c', '0'*ROOT.TSnIOConfigFrame.SizeOf(
                                ROOT.TSnIOConfigFrame.kIOvers, outConf))
                        ROOT.TSnIOConfigFrame.WriteTo(cfar,
                                                      outConf,
                                                      outRunf,
                                                      outNets)
                        # put header in buffer 'x'
                        x = b''.join(y for y in hdar)
                        # put config in buffer 'x'
                        x = b''.join( [ x, b''.join(z for z in cfar) ] )
                        printout(vtype.kDebug,'sending config: {0}'.format(len(x)))
                        printHex(vtype.kDebugData, bytearray(x))
                        # send the buffer
                        self.sendMsg(x)
                        # print the config we sent
                        if (getVerbosity()>=vtype.kDebug):
                            outConf.Print()
                            outRunf.Print()
                            outNets.Print()
                    else:
                        self.sendNoConfSignal()
                else:
                    with open(cname,'rb') as inf:
                        fbuf = inf.read()
                        self.sendMsg(fbuf)
                printout(vtype.kDebug,'sent config')
                self.doSendConf = False
                self.turnOffThermals = False
            except IOError as e:
                printout(vtype.kDebug, e)
                self.sendNoConfSignal()
        elif (self.doSendConf and (self.sendFtype==ftype.kNone) ):
            self.sendNoConfSignal()
        else:
            printout(vtype.kDebug,
                     'not sending conf: doSendConf={0}, sendFtype={1}'.format(
                    self.doSendConf, self.sendFtype))

    def connectionMade(self):
        self.setup()
        # save the connection made
        printout(vtype.kDebug,"connectionMade")
        try:
            self.peer = self.transport.getPeer()
            printout(vtype.kInfo, "Connected from {0}:{1} ({2})"\
                         .format(self.peer.host, 
                                 self.peer.port, self.peer.type))
        except:
            pass
        # TODO: get file to send from the config queue
        inext = splitFilenameExt(self.GetInConfName())[1]
        printout(vtype.kDebug, "GetInConfName={0}, inext={1}".format(
                self.GetInConfName(), inext))
        if ( (inext=='dat') or (inext=='DAT') ):
            self.sendFtype = ftype.kConfig
            if (self.IsDEFCONF()):
                self.sendFtype = ftype.kMbedBin
        elif ( (inext=='bin') or (inext=='BIN') ):
            self.sendFtype = ftype.kMbedBin
        else:
            self.sendFtype = ftype.kNone

        if (self.sendFtype==ftype.kNone):
            printout(vtype.kWarn, 'Will not send any file.')
        elif (self.sendFtype==ftype.kConfig):
            printout(vtype.kInfo, "Need to send config {0}".format(
                    self.GetInConfName()))
        elif (self.sendFtype==ftype.kMbedBin):
            printout(vtype.kInfo, "Need to send MBED binary {0}".format(
                    self.GetInConfName()))

        if (getVerbosity()>=vtype.kDebugData):
            bd = b''.join(self.buff)
            printout(vtype.kDebugData, 'full buffer now:')
            printHex(vtype.kDebugData, bytearray(bd))
        

    def connectionLost(self, reason):
        printout(vtype.kInfo, "connectionLost")
        try:
            printout(vtype.kInfo, "Disconnected from {0}:{1} ({2})"\
                         .format(self.peer.host, self.peer.port, reason.value))
        except:
            pass
        self.buff = []
        AriFileTransferOpts.fto.setStage(AriFileTransferOpts.stage.kInput,
                                         withPrompt=False)
        if (getVerbosity()>=vtype.kDebugData):
            bd = b''.join(self.buff)
            printout(vtype.kDebugData, 'full buffer now:')
            printHex(vtype.kDebugData, bytearray(bd))
        
        
    def getFromBuff(self, nbytes):
        br = b''.join(self.buff)
        m = br[:nbytes]
        self.buff = [ br[nbytes:] ]
        if (getVerbosity()>=vtype.kFileTimer):
            lm = len(m)
            self.tbytes += lm
            self.fbytes += lm
        return m

    def uponMsgFail(self, msg, loc):
        # reset to keep waiting
        return uponFail(msg, loc)

    def getHandlerFor(self, mcode, mlen):
        if (mcode == castHeaderCode(ROOT.TSnIOHeaderFrame.kStatusCode)):
            printout(vtype.kDebug, 'status!')
            return self.handleStatusUpd
        elif (mcode == \
                  castHeaderCode(ROOT.TSnIOHeaderFrame.kFileHeadrCode)):
            printout(vtype.kDebug, 'file header!')
            return self.handleFileHeader
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFilenameCode)):
            printout(vtype.kDebug, 'filename!')
            return self.handleFilename
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kConfigCode)):
            printout(vtype.kDebug, 'config!')
            return self.handleConfig
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kEventCode)):
            printout(vtype.kDebug, 'event!')
            return self.handleEvent
            printout(vtype.kDebug,"doSendConf={0}, doSendBinf={1}".format(
                    self.doSendConf, self.doSendBinf))
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kPowerCode)):
            printout(vtype.kDebug, 'power!')
            return self.handlePower
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kHeartbeatCode)):
            printout(vtype.kDebug, 'heartbeat!')
            return self.handleHeartbeat
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileNevtsCode)):
            printout(vtype.kDebug, 'nevts!')
            return self.handleNevtsSent
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileNpwrsCode)):
            printout(vtype.kDebug, 'npwrs!')
            return self.handleNpwrsSent
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileReadFailCode)):
            printout(vtype.kDebug, 'read fail!')
            return self.handleReadFail
        elif (mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kStringCode)):
            printout(vtype.kDebug, 'string!')
            return self.handleString
        else:
            printout(vtype.kError,"Unknown header code {0:02x}"
                     .format(mcode))
        if (mlen>MAXMESGLEN):
            printout(vtype.kError,"Message length {0} too long.".format(mlen))
        return None

    def dataReceived(self, data):
        printout(vtype.kDebugData, 'dataReceived:')
        printHex(vtype.kDebugData, bytearray(data))
        
        self.buff.append(data)
        #bd = b''.join(self.buff)
        #self.buff = [ bd ]
        bds = b''.join(self.buff)
        bd = ROOT.TSnIOBuffer(bds)
        self.buff = bds
        
        printout(vtype.kDebugData, 'full buffer now:')
        printHex(vtype.kDebugData, bytearray(bd))
        
        while ( len(bd) >= HEADER_LEN ):
            
            printout(vtype.kDebug, 'get header from:')
            printHex(vtype.kDebug, bytearray(bd[0:HEADER_LEN]))
            
            self.handleHeader(bd)
            if ( len(bd) < HEADER_LEN + self.needBytes ):
                if (getVerbosity()>=vtype.kDebug):
                    # just to not redo len(bd) if not necessary
                    printout(vtype.kDebug, 
                             'msg incomplete: HDR+mlen={0}, buffer={1}'
                             .format(HEADER_LEN+self.needBytes,len(bd)))
                break # don't have the whole message yet. wait for more data
            else:
                self.getFromBuff(HEADER_LEN) # remove the header
                self.handlePayload(
                    self.getHandlerFor(self.msgtype,self.needBytes),
                    self.msgtype,self.needBytes)
            bd = b''.join(self.buff)
            
            printout(vtype.kDebugData, 'full buffer now:')
            printHex(vtype.kDebugData, bytearray(bd))
                
    def handleHeader(self, data):
        mcode = ROOT.Long()
        mlen  = ROOT.Long()
        hdbyt = ROOT.TSnIOHeaderFrame.PyReadFrom(data,
                                                 mcode,
                                                 mlen)
        printout(vtype.kDebug,"header: mcode=x{0:02x}, mlen={1}".format(\
                mcode, mlen))
        if (hdbyt == HEADER_LEN):
            self.needBytes  = mlen
            self.msgtype    = mcode
            # else unknown header code or invalid msg length
            # TODO: HANDLE THIS ERROR PROPERLY!
        else:
            printout(vtype.kError,"read wrong number of header bytes {0}"\
                     .format(hdbyt))

    def handlePayload(self, handler, mcode, mlen):
        msg = self.getFromBuff(mlen)
        if (handler!=None):
            printout(vtype.kDebug,"Calling handler")
            handler(msg, mcode, mlen)

    
