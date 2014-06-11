import sys, os
import array, struct

from twisted.internet import protocol, defer

import ROOT
from AriStage import *
from AriUtils import *
from DeferredWithTimeout import *

# the script importing this file must call AriUtils.loadOnlineLibs()


# switch: directory in which to put raw status update files
rstatDir="rawStatus"
# switch: directory in which to put binary data files
binDir="binFiles"
# switch: directory in which to put signal strength files
sigstrDir="rawSigStr"
# switch: update num events received every...
updEvtCount=25
# switch: verbosity level (see vtype enum)
setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kFileTimer)
#setVerbosity(vtype.kDebug)
#setVerbosity(vtype.kDebugData)

# constants
HEADER_LEN = ROOT.TSnIOHeaderFrame.SizeOf()
MAXMESGLEN = 1300 # above this, we know it's an error

def uponFail(x, loc=None):
    printout(vtype.kError,"FAIL<{0}>: {1}".format(loc,x))


class AriStationProtocol(protocol.Protocol):
    '''
    The station protocol: handle all MBED header codes, read the binary
    messages, pass incoming info to the appriorate actor on the stage and
    send the response back to the MBED. Writes raw data to binary files
    as needed.
    
    requires its factory to have the following variables:
        .reactor   (the reactor running all this)
    '''
    
    #
    # Protocol fcns
    #

    def __init__(self):
        self.setup(False)

    def setup(self, isConn):
        # initialize the protocol
        self.reactor = None
        self.stage   = None
        self.buf     = None
        self.msgCode = None
        self.msgLen  = None
        # status
        self.whenGotStatusPwr = None
        self.whenGotStatusEvt = None
        self.statFile         = None
        self.stEvtTimeout     = 180 # (s) how long to wait for status' evt
        # data file
        self.rawFile     = None
        # instantiate
        if (isConn):
            self.reactor = self.factory.reactor
            self.stage   = AriStage(self.reactor)
            self.makeBuffer()
        

    def connectionMade(self):
        printout(vtype.kDebug,"connectionMade")
        try:
            self.peer = self.transport.getPeer()
            printout(vtype.kInfo, "Connected from {0}:{1} ({2})"\
                         .format(self.peer.host, self.peer.port,
                                 self.peer.type))
        except:
            pass
        self.setup(True)

    def dataReceived(self, data):
        if (getVerbosity()>=vtype.kDebug):
            printout(vtype.kDebug, "dataReceived {0}".format(len(data)))
            printout(vtype.kDebug, "msgCode={0}, msgLen={1}"
                     .format(self.msgCode, self.msgLen))
            if (self.msgCode!=None):
                printout(vtype.kDebug, "msgCode=x{0:02x}".format(self.msgCode))
            printout(vtype.kDebug, "buf len = {0}"
                     .format(self.buf.GetBytesRemaining()))
        if (self.buf==None):
            printout(vtype.kError,"dataReceived but self.buf is None!")
            printout(vtype.kError,"making new buffer.")
            self.makeBuffer()
        
        self.buf.AppendQuiet(data)

        if (getVerbosity()>=vtype.kDebugData):
            self.buf.Print()

        # if the rest of a message we were waiting for has arrived
        if ( (self.msgCode!=None) and 
             (self.buf.GetBytesRemaining()>=self.msgLen) ):
            printout(vtype.kDebug,
                     "rest of msg arrived. calling handlePayload")
            self.handlePayload()
        else:
            printout(vtype.kDebug, "still waiting for msg. "
                     "msgCode={0}, buflen={1}, msgLen={2}"
                     .format(self.msgCode, self.buf.GetBytesRemaining(),
                             self.msgLen))
        
        # if we are not, or are no longer, waiting for the rest of a message
        if (self.msgCode==None):
            while ( self.buf.GetBytesRemaining() > HEADER_LEN ):
                self.handleHeader()
                
                if (getVerbosity()>=vtype.kDebugData):
                    printout(vtype.kDebug, "Buffer after handle header:")
                    self.buf.Print()
                
                if (self.msgLen>MAXMESGLEN):
                    printout(vtype.kError,
                             "dataReceived: Message length {0} too long."\
                             .format(self.msgLen))
                    self.resetBuffer()
                    break
                elif ( self.buf.GetBytesRemaining() < self.msgLen ):
                    # need to wait for more data
                    if (getVerbosity()>=vtype.kDebug):
                        printout(vtype.kDebug, "dataReceived: need more data "
                                 "msgLen={0}, buf={1}".format(
                                self.msgLen, self.buf.GetBytesRemaining()))
                    # wait for the rest of the message
                    break
                else:
                    self.handlePayload()

                if (getVerbosity()>=vtype.kDebugData):
                    printout(vtype.kDebug, "Buffer after handle payload:")
                    self.buf.Print()

        # else: we're still waiting on the rest of this message
        
        # lose the data that's been read from the buffer
        self.buf = ROOT.TSnIOBuffer(self.buf)
        
    def connectionLost(self, reason=protocol.connectionDone):
        printout(vtype.kDebug, "connectionLost")
        try:
            printout(vtype.kInfo, "Disconnected from {0}:{1} ({2})"\
                         .format(self.peer.host, self.peer.port, reason.value))
        except:
            pass
    
        self.delBuffer()

    def sendIObuf(self, b):
        printout(vtype.kDebug,"sendIObuf")
        if (getVerbosity()>=vtype.kDebugData):
            b.Print()
        t = self.transport
        m = getDataFromBuf(b)
        if (t!=None):
            if (m!=None):
                if (getVerbosity()>=vtype.kDebug):
                    printout(vtype.kDebug, "Sending {0} bytes to station."
                             .format(len(m)))
                    printHex(vtype.kDebugData, bytearray(m))
                t.write(m)
                printout(vtype.kInfo, "Sent {0} bytes to the station."
                         .format(len(m)))
            else:
                printout(vtype.kWarn, "Can't send null buffer.")
        else:
            printout(vtype.kError, "No transport to send message:")
            printHex(vtype.kError, bytearray(m))

    def sendCommObj(self, comm):
        printout(vtype.kDebug,"sendCommObj")
        if (comm!=None):
            ##self.sendIObuf(comm.GetIOByteArray())
            # calling GetIOByteArray doesn't work in PyRoot for some reason
            # instead, use TSnIOBuffer's ctor that takes a comm obj
            printout(vtype.kInfo, "Sending station:")
            comm.Print()
            try:
                self.sendIObuf( ROOT.TSnIOBuffer(comm) )
            except:
                printout(vtype.kError, "sendCommObj unable to send [{0}]"\
                             .format(comm))
                mac = "unknown"
                if (self.stage.incStat!=None):
                    stat = self.stage.incStat.GetStatus()
                    if (stat!=None):
                        mac = stat.GetMacAdrAsStr()
                self.stage.SendAlert("AriStationProtocol",
                                     "sendCommObj unable to send ",
                                     "Unable to send comm object [{0}] "
                                     "to [{1}]".format(comm, mac))
        else:
            printout(vtype.kError, "Cannot send null comm object.")
            
        
    #
    # utility fcns
    #
    
    def sendUnableToWriteAlert(self, err, outf):
        '''
        send an email alert via the stage, including the data
        which could not be written to disk.
        
        outf must be an AriBinFile
        '''
        printout(vtype.kDebug, "sendUnableToWriteAlert")
        if (self.stage==None):
            printout(vtype.kError, "sendUnableToWriteAlert called but "
                     "no stage yet. Initializing...")
            self.setup(True)
        source = "AriStationProtocol"
        if (self.stage.incStat!=None):
            stat = self.stage.incStat.GetStatus()
            if (stat!=None):
                source += " ({0})".format(stat.GetMacAdrAsStr())
        msg = "Unable to open file {0} for writing! "\
            "Unsaved data in attachment.".format(outf.fname)
        self.stage.SendAlert(source,
                             "Unable to write to {0}".format(outf.fname),
                             msg,
                             getDataFromBuf(outf._buf, False),
                             os.path.basename(outf.fname))

    def copyHeaderToBinFile(self, outf, assumeAtStartOfPayload=True):
        if ( (self.buf.GetPosVal()>=HEADER_LEN) and assumeAtStartOfPayload
             and (self.buf[self.buf.GetPosVal()-HEADER_LEN]==self.msgCode) ):
            # header is still in the buffer (the check on msgCode IS needed)
            return self.copyLastFromBuffToBinFile(outf, HEADER_LEN)
        else:
            # header is not in the buffer anymore
            b = ROOT.TSnIOBuffer()
            ROOT.TSnIOHeaderFrame.WriteToW(b, self.msgCode, self.msgLen)
            b.SetPosVal(0)
            outf.storeForWriting( (b, b.GetBytesRemaining()),
                                  self.sendUnableToWriteAlert, outf )
            return True

    def pullFromBuffToBinFile(self, outf, withHeader=True):
        # first copy the header and then put the msg in the file
        # we must be at the start of the payload in the buffer for
        # this method to work
        if (outf!=None):
            ok = True
            if (withHeader):
                # write the header
                ok = self.copyHeaderToBinFile(outf, True)
            if (ok):
                # write the message
                outf.storeForWriting( (self.buf, self.msgLen),
                                      self.sendUnableToWriteAlert, outf )
                self.buf.IncrementPos(self.msgLen)
                return True
        else:
            printout(vtype.kError, "ERROR. No binary file in which to store "
                     "data.")
        return False
    
    def copyLastFromBuffToBinFile(self, outf, siz=None):
        if (siz==None):
            siz = self.msgLen
        bkup = self.buf.GetPosVal() - siz
        if ( (outf!=None) and (bkup>=0) ):
            outf.storeForWriting( (self.buf, bkup, siz),
                                  self.sendUnableToWriteAlert, outf )
            return True
        else:
            printout(vtype.kError, "ERROR. No binary file in which to store "
                     "data.")
            # if this happens, it's a software problem.
            # raise so that the issue gets fixed
            raise RuntimeError("Could not copy to bin file [{0}] size [{1}] "
                               "(buf pos [{2}])".format(outf, siz, 
                                                        self.buf.GetPosVal()))
            return False

    def writeBinFileToDisk(self, outf):
        printout(vtype.kDebug, "writeBinFileToDisk")
        return outf.writeBufWhenAble( self.sendUnableToWriteAlert, outf )

    def makeBuffer(self):
        printout(vtype.kDebug, "makeBuffer")
        self.buf = ROOT.TSnIOBuffer()

    def resetBuffer(self):
        printout(vtype.kInfo, "Resetting buffer!")
        self.clearHeader()
        self.makeBuffer()
        
    def delBuffer(self):
        printout(vtype.kDebug, "delBuffer")
        self.buf = None

    def clearHeader(self):
        printout(vtype.kDebug, "Clearing header!")
        self.msgCode = None
        self.msgLen  = None

    def isMsgCode(self, code):
        return (self.msgCode == castHeaderCode(code))

    @defer.inlineCallbacks
    def saveSignalStr(self, ss, ssvers):
        printout(vtype.kDebug, "saveSignalStr")
        ssfile = AriSigStrBinFile(sigstrDir, ss)
        sb = ROOT.TSnIOBuffer()
        ROOT.TSnIOHeaderFrame.WriteToW(sb, ROOT.TSnIOHeaderFrame.kSignalStrCode,
                                  ROOT.TSnIOSignalStrengthFrame.SizeOf(ssvers))
        ROOT.TSnIOSignalStrengthFrame.WriteToW(sb, ss, ssvers)
        sb.SetPosVal(0) # go back to the beginning or we won't write anything
        printout(vtype.kDebug, 
                 "writing sig str to {0}".format(ssfile.GetFilename()))
        yield ssfile.writeWhenAble( sb, self.sendUnableToWriteAlert, ssfile )
    
    @defer.inlineCallbacks
    def stageStatus(self):
        printout(vtype.kDebug,"stageStatus")
        yield self.writeBinFileToDisk(self.statFile)
        self.statFile.done()
        r = yield self.stage.DeferredResponseToStatus()
        self.sendCommObj(r)
        self.statFile = None

    @defer.inlineCallbacks
    def fileReceived(self):
        fcrc = ROOT.Long(0)
        yield self.writeBinFileToDisk(self.rawFile)
        self.rawFile.done()
        try:
            fcrc = yield self.rawFile.GetCRC()
        except:
            printout(vtype.kDebug,"fileReceived: Could not get CRC")
        self.stage.FileReceivedChecksum(fcrc)
        resp = yield self.stage.DeferredResponseToFile()
        self.sendCommObj(resp)
        self.rawFile = None
    
    #
    # Input handlers
    #

    def handleHeader(self):
        self.msgCode = ROOT.Long()
        self.msgLen  = ROOT.Long()
        br    = ROOT.TSnIOHeaderFrame.PyReadFrom(self.buf,
                                                 self.msgCode, self.msgLen)
        if (getVerbosity()>=vtype.kDebug):
            printout(vtype.kDebug,"handleHeader: mcode=x{0:02x}, mlen={1}"
                     .format(self.msgCode, self.msgLen))
        if (br!=HEADER_LEN):
            printout(vtype.kError,"handleHeader: read wrong number of "
                     "bytes {0}!".format(br))

    def handlePayload(self):
        if (self.isMsgCode(ROOT.TSnIOHeaderFrame.kStatusCode)):
            self.handleStatusUpd()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileHeadrCode)):
            self.handleFileHeader()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFilenameCode)):
            self.handleFilename()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kConfigCode)):
            self.handleConfig()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kEventCode)):
            self.handleEvent()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kPowerCode)):
            self.handlePower()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kHeartbeatCode)):
            self.handleHeartbeat()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kMbedFileChksCode)):
            self.handleMbedFileChksCode()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileNevtsCode)):
            self.handleNevtsSent()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileNpwrsCode)):
            self.handleNpwrsSent()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileTrgStrtCode)):
            self.handleClockSetTrgStart()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileTrgStopCode)):
            self.handleClockSetTrgStop()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kFileReadFailCode)):
            self.handleReadFail()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kHnShDemandCode)):
            self.handleHandshakeDemand()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kStringCode)):
            self.handleString()
        elif (self.isMsgCode(ROOT.TSnIOHeaderFrame.kSignalStrCode)):
            self.handleSignalStr()
        else:
            # TODO: how should this be handled?
            printout(vtype.kError,"Unknown header code {0:02x}"
                     .format(self.msgCode))
            self.buf.IncrementPos(self.msgLen)
        # done with this message
        self.clearHeader()
        
    def handleStatusUpd(self):
        printout(vtype.kDebug, 'handleStatusUpd')
        status = ROOT.TSnStatusUpdate()
        event  = ROOT.TSnEvent()
        stvers = ROOT.Long()
        br = ROOT.TSnIOStatusFrame.BytesReadFrom(
            self.buf, status, event, stvers)
        if (getVerbosity()>=vtype.kDebug):
            printout(vtype.kDebug,"Read {0} bytes for status.".format(br))
            if (getVerbosity()>=vtype.kDebugData):
                printout(vtype.kDebugData,"Buffer after status read:")
                self.buf.Print()
        if (br==self.msgLen):
            # send status to stage
            self.stage.IncomingStatus(status, stvers)
            # save the raw status update
            self.statFile = AriStatusBinFile(rstatDir, status)
            printout(vtype.kDebug, 'Made raw status file [{0}]'
                     .format(self.statFile.GetFilename()))
            # copy the header and the status update out of the buffer
            ok = self.copyHeaderToBinFile(self.statFile)
            ok = self.copyLastFromBuffToBinFile(self.statFile,
                                                self.msgLen)
            if not ok:
                printout(vtype.kError,"Unable to write raw status update!")
            # check version number of status; only v1 is packaged with an evt
            evtInStatus = (stvers==1)
            if (evtInStatus):
                event.CalcCRC()
            else:
                if (self.whenGotStatusEvt!=None):
                    # another status update arrived before finishing
                    # the last one. this can happen in SBD comms.
                    # stop waiting for the old stuff.
                    d, self.whenGotStatusEvt = self.whenGotStatusEvt, None
                    d.cancel()
                self.whenGotStatusEvt = DeferredWithTimeout()
                # put it in the file when it arrives
                self.whenGotStatusEvt.addCallback(
                    self.pullFromBuffToBinFile)
                # or just timeout and stage the status
                def sendit(x=None):
                    d, self.whenGotStatusEvt = self.whenGotStatusEvt, None
                    #d.cancel() - done by DeferredWithTimeout
                    self.stageStatus()
                self.whenGotStatusEvt.addTimeoutCallback(self.reactor,
                                                         self.stEvtTimeout,
                                                         sendit)
                self.whenGotStatusEvt.addErrback(uponFail,
                                                 'whenGotStatusEvt')
            if (getVerbosity()>=vtype.kInfo):
                # print the status
                status.Print()
                if (evtInStatus):
                    event.Print()
            if (self.whenGotStatusPwr!=None):
                # another status update arrived before finishing
                # the last one. this can happen in SBD comms.
                # stop waiting for the old stuff.
                d, self.whenGotStatusPwr = self.whenGotStatusPwr, None
                d.cancel()
            self.whenGotStatusPwr = defer.Deferred()
            self.whenGotStatusPwr.addCallback(
                self.pullFromBuffToBinFile)
            self.whenGotStatusPwr.addErrback(uponFail,
                                             'whenGotStatusPwr')
        else:
            printout(vtype.kError,"handleStatusUpd: "
                     "Read {0} bytes but needed {1}!".format(br, self.msgLen))

    def handleFileHeader(self):
        printout(vtype.kDebug, 'handleFileHeader')
        
        fv = ROOT.TSnIODataFile.PeekIOversion(self.buf)
        self.stage.IncomingFileVersion(fv)
        
        # write directly to the file
        ok = self.pullFromBuffToBinFile(self.rawFile, False)
        if not ok:
            printout(vtype.kError, 'Got file header but '
                     'no open file to write to! Discarding file header.')
            

    def handleFilename(self):
        printout(vtype.kDebug, 'handleFilename')
        fname = ROOT.TSnIOObject.ReadStrFrom(self.buf, self.msgLen)
        ROOT.SetOwnership(fname, True)
        printout(vtype.kDebug, "filename=[{0}]".format(fname))
        if (self.rawFile==None):
            self.rawFile   = AriBinFile("{0}/{1}".format(binDir, fname))
            self.stage.IncomingFilename(fname)
            printout(vtype.kInfo, "Saving raw file [{0}]".format(
                    self.rawFile.fname))
        else:
            printout(vtype.kError, "Got incoming filename, but still "
                     "dealing with file [{0}]".format(
                    self.rawFile.GetFilename()))

    def handleConfig(self):
        printout(vtype.kDebug, 'handleConfig')
        ok = self.pullFromBuffToBinFile(self.rawFile)
        if not ok:
            printout(vtype.kError, 'Got file config but '
                     'no open file to write to! Discarding.')
    
    def handleEvent(self):
        printout(vtype.kDebug, 'handleEvent')
        if (self.whenGotStatusEvt!=None):
            # we've been waiting for the status update event.
            # put it in the status file and send the reply
            printout(vtype.kDebug, "callback for whenGotStatusEvt")
            d, self.whenGotStatusEvt = self.whenGotStatusEvt, None
            d.callback(self.statFile)
            self.stageStatus()
        else:
            # this event is from a data file
            self.stage.IncomingFileEvent() # increment counter
            if (self.rawFile!=None):
                printout(vtype.kDebug, 'saving to event binf')
                ok = self.pullFromBuffToBinFile(self.rawFile)
                if not ok:
                    printout(vtype.kError, 'Got file event but '
                             'no open file to write to! Discarding.')
            else:
                printout(vtype.kError, "ERROR. No binary file in which to "
                         "store event.")
            if (getVerbosity()>=vtype.kInfo):
                n = self.stage.GetNevtsRcvd()
                if ((n % updEvtCount)==0):
                    printout(vtype.kInfo, "got {0} events".format(n))
        
    def handlePower(self):
        printout(vtype.kDebug, 'handlePower')
        if (self.whenGotStatusPwr!=None):
            # we've been waiting for the status update power reading.
            printout(vtype.kDebug, "callback for whenGotStatusPwr")
            # send it to the stage
            power = ROOT.TSnPowerReading()
            pvers = ROOT.Long()
            ROOT.TSnIOPowerFrame.BytesReadFrom(self.buf, power, pvers)
            self.stage.IncomingStatusPowerReading(power, pvers)
            # print the power
            if (getVerbosity()>=vtype.kInfo):
                power.Print()
                power = None
            self.buf.DecrementPos(self.msgLen) # rollback for writing to file
            d, self.whenGotStatusPwr = self.whenGotStatusPwr, None
            d.callback(self.statFile) # put it in the status file
        else:
            # this power is in a data file
            self.stage.IncomingFilePowerReading() # increment counter
            printout(vtype.kDebug, 'saving power reading to event binf')
            ok = self.pullFromBuffToBinFile(self.rawFile)
            if not ok:
                printout(vtype.kError, 'Got file power reading but '
                         'no open file to write to! Discarding.')

    def handleHeartbeat(self):
        printout(vtype.kDebug, 'handleHeartbeat')
        printout(vtype.kDebug, 'saving heartbeat to event binf')
        ok = self.pullFromBuffToBinFile(self.rawFile)
        if not ok:
            printout(vtype.kError, 'Got file heartbeat but '
                     'no open file to write to! Discarding.')
        
    def handleMbedFileChksCode(self):
        printout(vtype.kDebug, 'handleMbedFileChksCode')
        try:
            n = ROOT.TSnIOObject.ReadFromUInt(self.buf)
            self.stage.IncomingFileChecksum(n)
        except:
            printout(vtype.kError, 'failed to read UInt from buffer')

    def handleNevtsSent(self):
        printout(vtype.kDebug, 'handleNevtsSent')
        try:
            n = ROOT.TSnIOObject.ReadFromUInt(self.buf)
            self.stage.IncomingFileEventsSent(n)
        except:
            printout(vtype.kError, 'failed to read UInt from buffer')
        
    def handleNpwrsSent(self):
        printout(vtype.kDebug, 'handleNpwrsSent')
        try:
            n = ROOT.TSnIOObject.ReadFromUInt(self.buf)
            self.stage.IncomingFilePowerReadingsSent(n)
            # num powers marks the end of the file transfer
            self.fileReceived()
        except:
            printout(vtype.kError, 'failed to read UInt buffer')

    def handleClockSet(self):
        printout(vtype.kDebug, 'handleClockSet')
        printout(vtype.kDebug, 'saving clock set to event binf')
        ok = self.pullFromBuffToBinFile(self.rawFile)
        if not ok:
            printout(vtype.kError, 'Got clock set '
                     'but no open file to write to! Discarding.')

    def handleClockSetTrgStart(self):
        printout(vtype.kDebug, 'handleClockSetTrgStart')
        # nothing special for starting clock set
        self.handleClockSet()

    def handleClockSetTrgStop(self):
        printout(vtype.kDebug, 'handleClockSetTrgStop')
        # save to file
        self.handleClockSet()
        # this is the end of the file in fileVersion >= 4, but
        # the MBED will still tell us how many evts/pwrs it sent,
        # so this is not the end of the file *transfer*

    def handleReadFail(self):
        printout(vtype.kWarn, 'Station read file failed!')
        
    def handleString(self):
        printout(vtype.kUrgent, 'received string:')
        s = ROOT.TSnIOObject.ReadStrFrom(self.buf, self.msgLen)
        ROOT.SetOwnership(s, True)
        printout(vtype.kUrgent, s)

    def handleSignalStr(self):
        printout(vtype.kDebug, 'handleSignalStr')
        sigstr = ROOT.TSnSignalStrengthInfo()
        ssvers = ROOT.Long()
        br = ROOT.TSnIOSignalStrengthFrame.BytesReadFrom(self.buf, sigstr,
                                                         ssvers)
        if (br==self.msgLen):
            if (getVerbosity()>=vtype.kInfo):
                sigstr.Print()
            self.saveSignalStr(sigstr, ssvers)
        else:
            printout(vtype.kError, 
                     "Read {0} bytes for sig str but "
                     "expected {1}".format(br, self.msgLen))
        
    @defer.inlineCallbacks
    def handleHandshakeDemand(self):
        printout(vtype.kDebug, "handleHandshakeDemand")
        resp = yield self.stage.DeferredResponseToHandshake(self.msgCode)
        self.sendCommObj(resp)


class AriStnProtFactory(protocol.Factory):
    def __init__(self, reactr):
        self.reactor = reactr
        self.protocol = AriStationProtocol


