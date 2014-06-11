import sys
import datetime
import array
import struct
import time
import base64
import threading
import Queue

import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat")
ROOT.gSystem.Load("$SNS/lib/libevdat")
ROOT.gSystem.Load("$SNS/lib/libonline")

def enum(**enums):
    return type('Enum', (), enums)

vtype=enum(kSilent=0,  kError=10, kUrgent=20,
           kWarn=30,   kInfo=40,  kFileTimer=50,
           kDebug=100, kDebugData=150)

ftype=enum(kConfig=0,   kMbedBin=10)

# TODO: fully implement the listenTimeout

# switch: send text or binary
sendBinary = True
# switch: fraction of events or power readings received
#         out of total sent by station, above which is
#         considered an acceptable partial receipt
partialOkFrc = 0.90
# switch: directory in which to put root files
rootDir="rootFiles"
# switch: directory in which to put binary files
binDir="binFiles"
# switch: listen timeout (s)
listenTimeout=30
# switch: name of file to save bytes tranfered timing ntuple
#         this is only used if verb>=kFileTimer
filetimerNToutfn = "FileTimerNtuple.txt"
# switch: update num events received every...
updEvtCount=10
# switch: verbosity level (see vtype enum)
verb=vtype.kInfo
#verb=vtype.kFileTimer
#verb=vtype.kDebug
#verb=vtype.kDebugData


def printout(level, text):
    global verb
    if (verb>=level):
        print text

def printHex(level, x):
    printout(level, ''.join( [ '%02x ' % i for i in x ] ))

# convert the UChar_t value from TSnIOHeaderFrame to an
# integer that can be passed between python & ROOT
def castHeaderCode(code):
    return ROOT.Long(struct.unpack('<B', code)[0])

def getUInt(msg):
    printout(vtype.kDebug,'getUInt from:')
    printHex(vtype.kDebug, bytearray(msg))
    # station (MBED) is little endian
    return ROOT.Long(struct.unpack('<I', msg[0:4])[0])

def getUChar(msg):
    return ROOT.Long(struct.unpack('<B', msg[0:1])[0])

def splitFilenameExt(filename):
    if '.' in filename:
        return filename.rsplit('.',1)
    else:
        return ['','']

def getDateStr():
    return datetime.datetime.now().strftime("%Y-%m-%d.%H-%M-%S")

def readConfigFile(infname):
    with open(infname,'rb') as inf:
        fbuf  = inf.read()
        mcode = ROOT.Long()
        mlen  = ROOT.Long()
        br = ROOT.TSnIOHeaderFrame.PyReadFrom(fbuf, mcode, mlen)
        if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kConfigCode)):
            conf = ROOT.TSnDAQConfig()
            runf = ROOT.TSnRunInfo()
            nets = ROOT.TSnNetAdrSet()
            ROOT.TSnIOConfigFrame.ReadFrom(fbuf[br:], conf, runf, nets)
            printout(vtype.kInfo, 'read config from {0}'.format(infname))
            if (verb>=vtype.kInfo):
                runf.Print();
                conf.Print();
                nets.Print();
            return conf, runf, nets, mcode, mlen
        else:
            printout(vtype.kError, 'File does not contain config '\
                '(or is an old version with no header).')
        return None, None, None, None, None

# A base class for a thread that will do nothing but listen to the
# port and buffer incoming data. See the run() function.
# The listener puts entries in a Queue, each entry being a list
# of strings. Each string is a chunk of data read from the connection.
#
# Derived classes may override:
#  Setup(): do stuff before starting to listen
#  DoRead(): try to read the specified number of bytes from the connection
#  BytesToRecv(): return number of bytes the first read should attempt to get
#  BytesWaiting(): return number of bytes the next read should attempt to get
class threadListener(threading.Thread):
    def Setup(self):
        # may be overridden
        pass

    def DoRead(self, nbytes):
        # should be overridden
        return 0

    def BytesToRecv(self):
        # should be overridden
        return 1

    def BytesWaiting(self):
        # should be overridden
        return 0

    def setComm(self, communikationizer):
        self.comm = communikationizer
        self.Qbuf = communikationizer.Qbuff
    
    def run(self):
        # small speedup maybe?
        qbufPutNW = self.Qbuf.put_nowait
        while True:
            try:
                bytes = self.BytesToRecv()
                buff = []
                buffAppend = buff.append
                while (bytes>0):
                    buffAppend(self.DoRead(bytes))
                    bytes = self.BytesWaiting()
                    #print '***** {0}'.format(buff)
                qbufPutNW(buff)
            except IOError as e:
                printout(vtype.kError, e)
                #break

# The main class for handling the various blocks of data received from
# a station.
#
# setup() must be called before any incoming data is handled.
# handle() begins the loop listening for data on the connection.
# (Thus SerialSocket will call these functions automagically.)
#
# Derived classes must override:
#   SendData(): try to send a string of bytes through the communication,
#               return number of bytes sent
#   GetAListener(): return a listener object of the desired type
#   GetInConfName(): return the name of the binary config file that should
#                    be sent to the station
class communikationizer():
    HEADER_LEN = ROOT.TSnIOHeaderFrame.SizeOf()
    start   = None
    fstart  = None
    tbytes  = 0
    fbytes  = 0
    if (verb>=vtype.kFileTimer):
        ntof = open(filetimerNToutfn,"wt")

    def SendData(self, message, binary):
        # should be overridden
        # return number of bytes sent
        return 0

    def GetAListener(self):
        # should be overridden
        # return the appropriate listener object
        return None

    def GetInConfName(self):
        # should be overridden
        # return the name of the config file to send to the station
        return ''

    def handle(self):
        # start getting data, begin with the first header (status update)
        self.firstHeader = True # first
        while (self.getHeader()):
            self.firstHeader = False
            message = self.getBlock()
            if (message!=None):
                self.handleMessage(message)

    def getData(self, nbytes):
        br = 0
        msg = []
        # first check the buffer
        part = self.getFromBuff(nbytes-br)
        br += len(part)
        msg.append(part)
        # now get from the listener
        while (br<nbytes):
            ##print 'MSG({0}:{1}/{2})={3}'.format(len(msg),br,nbytes,msg)
            ##print 'BUFF({0})={1}'.format(len(b''.join(self.buff)),
            ##                             b''.join(self.buff))
            # block & wait for more data coming in
            self.getFromListener()
            part = self.getFromBuff(nbytes-br)
            br += len(part)
            msg.append(part)
        return b''.join(msg)
    
    def receiveAll(self, nbytes):
        br=0
        begin = time.time()
        rchunks = []
        printout(vtype.kDebug,'trying to receive nbytes={0}'.format(nbytes))
        while (nbytes>br):
            if ( (time.time() - begin) > listenTimeout ):
                printout(vtype.kWarn,'timeout! time={0}, beg={1}, to={2}'\
                    .format(time.time(), begin, listenTimeout))
                break
            try:
                data = self.getData(nbytes-br)
                if data:
                    br += len(data)
                    rchunks.append(data)
                    printout(vtype.kDebug,'got some. br={0}.'.format(br))
                    printout(vtype.kDebug, rchunks)
                else:
                    time.sleep(0.1)
            except IOError as e:
                printout(vtype.kError, e)
                break
        if (br==nbytes):
            return b''.join(rchunks)
        else:
            printout(vtype.kWarn,'returning None. br={0}, nbytes={1}'\
                .format(br, nbytes))
            return None

    def getFromBuff(self, nbytes):
        br = 0
        msg = []
        for idx,chunk in enumerate(self.buff):
            part = chunk[0:(nbytes-br)]
            lpar = len(part)
            msg.append(part)
            br += lpar
            if (len(chunk)>=lpar):
                # only read a bit of this chunk
                # remove the read bit
                self.buff[idx] = chunk[lpar:]
            if (nbytes<=br):
                # done
                break
        # remove empty chunks
        self.buff = filter(None, self.buff)
        #return b''.join(msg)
        m = b''.join(msg)
        lm = len(m)
        self.tbytes += lm
        self.fbytes += lm
        return m

    def getFromListener(self):
        self.buff.append(b''.join(self.Qbuff.get()))
        self.Qbuff.task_done()


    def getHeader(self):
        # keep receiving data until we have the whole header
        # or until timeout
        hdr = self.receiveAll(self.HEADER_LEN)
        if (self.firstHeader):
            printout(vtype.kUrgent, 'Incoming header:')
            printHex(vtype.kUrgent, bytearray(hdr))
            if (hdr=='No di'):
                printout(vtype.kError, ">>> MBED didn't find SD card. "
                         "Insert and/or restart.")
                sys.exit()
        if (hdr!=None):
            self.handleHeader(hdr)
            return True
        else:
            printout(vtype.kError,'could not get header')
            return False

    def handleHeader(self, hdr):
        hdbyt = ROOT.TSnIOHeaderFrame.PyReadFrom(hdr,
                                                 self.mcode,
                                                 self.mlen)
        if (hdbyt == self.HEADER_LEN):
            printout(vtype.kDebug,'header: mcode={0:02x}, mlen={1}'\
                         .format(self.mcode, self.mlen))
            if (self.firstHeader):
                if (self.mcode != \
                        castHeaderCode(ROOT.TSnIOHeaderFrame.kStatusCode)):
                    printout(vtype.kError,'expect status update first. '\
                                 'got mcode={0:02x} instead.'\
                                 .format(self.mcode))
                    sys.exit()
        else:
            self.mcode=0
            self.mlen=0
            raise RuntimeError("Read header got wrong number of bytes ({0})"\
                                   .format(hdbyt))


    def getBlock(self):
        return self.receiveAll(self.mlen)

    def on_close(self):
        printout(vtype.kInfo,'connection closed')

    def setup(self):
        printout(vtype.kInfo,'new connection')
        self.buff  = []
        self.Qbuff = Queue.Queue()
        # start the listener
        self.listener = self.GetAListener()
        if (self.listener != None):
            self.listener.setComm(self)
            self.listener.Setup()
            self.listener.daemon = True
            self.listener.start()
        # for the block headers
        self.mcode     = ROOT.Long()
        self.mlen      = ROOT.Long()
        # for sending a config
        self.outConf   = None
        self.outRunf   = None
        self.outNets   = None
        self.doSendConf = False
        self.doSendBinf = False
        self.sendFtype  = ftype.kConfig
        inext = splitFilenameExt(self.GetInConfName())[1]
        if (inext!=''):
            if ( (inext=='dat') or (inext=='DAT') ):
                self.sendFtype = ftype.kConfig
            elif ( (inext=='bin') or (inext=='BIN') ):
                self.sendFtype = ftype.kMbedBin
            else:
                printout(vtype.kError,
                         'Unknown output file type [{0}]'.format(inext))
                sys.exit()
        # for the file header info
        self.inFileRunInfo = None
        self.inFileMbed    = None
        self.inFileSeq     = None
        self.inFilePower   = None
        # for an incoming file name
        self.inFilen   = None
        # for the file config info
        self.inConf    = None
        self.inRunf    = None
        self.inNets    = None
        # for a status update
        self.inStatus  = None
        # for an incoming event
        self.inWvPack  = None
        self.inEvent   = None
        # for an incoming power reading
        self.inPower   = None
        # the output root file
        self.rootFile  = None
        # the output binary file
        self.binFile   = None
        # for checking if enough (file) data arrived
        self.numInEvts = 0
        self.numInPows = 0
        self.numPowsSent = 0
        self.numEvtsSent = 0
    
    def sendMsg(self, message):
        bnry=isinstance(message, bytes)
        if (sendBinary):
            if (bnry):
                self.SendData(message, binary=True)
            else:
                raise RuntimeError(
                    "sendBinary true, but message is not binary")
        else:
            if (bnry):
                message = base64.b64encode(message)
            self.SendData(message, binary=False)
    
    def sendBinf(self):
        if (self.doSendBinf and (self.sendFtype==ftype.kMbedBin) ):
            try:
                printout(vtype.kInfo,"SENDING NEW BIN FILE!")
                # TODO - change GetInConfName to reflect input file types
                lfname = self.GetInConfName()
                bfname = lfname[lfname.rfind('/')+1:]
                if (len(bfname.split('.')[0])>8):
                    printout(vtype.kWarn,"Making filename conform to 8.3!")
                    bfname = ''.join( [ (bfname.split('.')[0])[0:8],
                                        '.',
                                         bfname.split('.')[1] ] )
                printout(vtype.kInfo,"Local file={0}".format(lfname))
                printout(vtype.kInfo,"Filename={0}".format(bfname))
                # send the filename
                hdar = array.array('c','0'*self.HEADER_LEN)
                ROOT.TSnIOHeaderFrame.WriteTo(hdar, 
                   ROOT.TSnIOHeaderFrame.kMbedFilenameCode, len(bfname))
                x = b''.join(hdar)
                x = b''.join( [ x, b''.join(z for z in bfname) ] )
                printout(vtype.kDebug,"send filename header")
                printHex(vtype.kDebugData,bytearray(x))
                self.sendMsg(x)
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
                    printout(vtype.kDebug,"send file bytes...")
                    printHex(vtype.kDebugData,bytearray(x))
                    printHex(vtype.kDebugData,bytearray(ab))
                    self.sendMsg(x)
                    self.sendMsg(ab)
                    printout(vtype.kDebug,"sent file")
                    # now send the CRC
                    ROOT.TSnIOHeaderFrame.WriteTo(hdar,
                       ROOT.TSnIOHeaderFrame.kMbedFileChksCode, crc)
                    printout(vtype.kDebug,"send checksum {0}".format(crc))
                    printHex(vtype.kDebugData,bytearray(hdar))
                    self.sendMsg( b''.join(hdar) )
                self.doSendBinf = False
            except IOError as e:
                printout(vtype.kDebug, e)
                printout(vtype.kInfo, 'Not sending any MBED binary file.')
                self.doSendBinf = False
        else:
            printout(vtype.kDebug,
                     'not sending binf: doSendBinf={0}, sendFtype={1}'.format(
                    self.doSendBinf, self.sendFtype))
    
    def sendConf(self):
        if (self.doSendConf and (self.sendFtype==ftype.kConfig) ):
            try:
                self.outConf, self.outRunf, self.outNets, mcode, mlen \
                    = readConfigFile(self.GetInConfName())
                if ( (self.outConf != None) and 
                     (self.outRunf != None) and
                     (self.outNets != None) and
                     (mcode        != None) and
                     (mlen         != None) ):
                    hdar = array.array('c', '0'*self.HEADER_LEN)
                    ROOT.TSnIOHeaderFrame.WriteTo(hdar, mcode, mlen)
                    cfar = array.array('c', '0'*ROOT.TSnIOConfigFrame.SizeOf(
                            ROOT.TSnIOConfigFrame.kIOvers, self.outConf))
                    ROOT.TSnIOConfigFrame.WriteTo(cfar,
                                                  self.outConf,
                                                  self.outRunf,
                                                  self.outNets)
                    # put header in buffer 'x'
                    x = b''.join(y for y in hdar)
                    # put config in buffer 'x'
                    x = b''.join( [ x, b''.join(z for z in cfar) ] )
                    printout(vtype.kDebug,'sending config: {0}'.format(len(x)))
                    printHex(vtype.kDebugData, bytearray(x))
                    # send the buffer
                    self.sendMsg(x)
                    printout(vtype.kDebug,'sent config')
                    # print the config we sent
                    self.doSendConf = False
                    if (verb>=vtype.kDebug):
                        self.outConf.Print()
                        self.outRunf.Print()
                        self.outNets.Print()
            except IOError as e:
                printout(vtype.kDebug, e)
                printout(vtype.kInfo, 'Not sending any new config.')
                hd = array.array('c', '0'*self.HEADER_LEN)
                ROOT.TSnIOHeaderFrame.WriteTo(hd,
                     ROOT.TSnIOHeaderFrame.kNoConfigCode, 0)
                x = b''.join(hd)
                printout(vtype.kDebugData, 'sending no config signal:')
                printHex(vtype.kDebugData, bytearray(x))
                self.sendMsg(x)
                self.doSendConf = False
        else:
            printout(vtype.kDebug,
                     'not sending conf: doSendConf={0}, sendFtype={1}'.format(
                    self.doSendConf, self.sendFtype))

    def handleStatusUpd(self, msg):
        self.inStatus = ROOT.TSnStatusUpdate()
        self.inEvent  = ROOT.TSnEvent()
        # as of V2, the status doesn't actually contain the event
        # but ReadFrom wants an event anyway, in case we receive
        # a status V1
        br = ROOT.TSnIOStatusFrame.BytesReadFrom(
            msg,self.inStatus,self.inEvent,len(msg))
        if (br>0): # could check br == msgLen from header
            # message reading successful
            printout(vtype.kDebug,
                     'status complete: msg len {0}'.format(len(msg)))
            # store the wave pack settings in case an event comes next
            self.inWvPack = { 'LSB':self.inStatus.GetWvLoseLSB(),
                              'MSB':self.inStatus.GetWvLoseMSB(),
                              'base':self.inStatus.GetWvBaseline() }
            self.inEvent.CalcCRC()
            evtInStatus = getUChar(msg)==1
            if (verb>=vtype.kInfo):
                self.inStatus.Print()
                if ( evtInStatus ): # check i/o version of status
                    self.inEvent.Print()
            # save stat/evt in root
            routfn = 'afarStatus/status.ws.{0}.root'.format(getDateStr())
            routf  = ROOT.TFile.Open(routfn,"recreate")
            routf.cd()
            self.inStatus.Write("status")
            if (evtInStatus):
                self.inEvent.Write("evt")
            routf.Close()
            # send the config
            if ( self.sendFtype==ftype.kConfig ):
                self.doSendConf = True
            elif ( self.sendFtype==ftype.kMbedBin ):
                self.doSendBinf = True
    
    def getRootOutf(self, infn, tag=''):
        if (self.rootFile!=None):
            return self.rootFile
        else:
            if (infn!=None):
                fn = '{0}/{1}{2}.root'.format(
                    rootDir, splitFilenameExt(infn)[0], tag)
                self.rootFile = ROOT.TFile.Open(
                    fn, 'recreate')
                return self.rootFile
            else:
                printout(vtype.kError, 'getRootOutf: no infn')
                return None

    def getBinOutf(self, fn, tag=''):
        if (self.binFile!=None):
            return self.binFile
        else:
            if (fn!=None):
                ofn = '{0}/{1}{2}.dat'.format(
                    binDir, splitFilenameExt(fn)[0], tag)
                self.binFile = open(ofn, 'wb')
                return self.binFile
            else:
                printout(vtype.kError, 'getBinOutf: no fn')
                return None
    
    def writeToBinf(self, data):
        # writes the current header plus the contents of data
        # to the binary file
        # only the length of data specified in the current header
        # is written
        om = array.array('c', '0'*self.HEADER_LEN)
        ROOT.TSnIOHeaderFrame.WriteTo(om, self.mcode, self.mlen)
        x = b''.join(y for y in om)
        binf = self.getBinOutf(self.inFilen)
        if (binf!=None):
            printout(vtype.kDebug, 'writing to binf')
            if (self.mcode!=castHeaderCode(
                    ROOT.TSnIOHeaderFrame.kFileHeadrCode)):
                binf.write(x) # the header for this block
            binf.write(data[0:self.mlen])
        else:
            printout(vtype.kError, 'no bin file to write to')
    
    def sendHandshake(self, hnShCode):
        om = array.array('c', '0'*self.HEADER_LEN)
        ROOT.TSnIOHeaderFrame.WriteTo(om, hnShCode, 0)
        x = b''.join(y for y in om)
        printout(vtype.kInfo, 'sending handshake: {0}'.format(hnShCode))
        printHex(vtype.kDebug, bytearray(x))
        self.sendMsg(x)
        printout(vtype.kDebug, 'sent handshake')

    def handleConfig(self, msg):
        self.inConf = ROOT.TSnDAQConfig()
        self.inRunf = ROOT.TSnRunInfo()
        self.inNets = ROOT.TSnNetAdrSet()
        br = ROOT.TSnIOConfigFrame.BytesReadFrom(
            msg, self.inConf, self.inRunf, self.inNets)
        if (br==self.mlen):
            # store wave pack settings for incoming events
            self.inWvPack = { 'LSB':self.inConf.GetWvLoseLSB(),
                              'MSB':self.inConf.GetWvLoseMSB(),
                              'base':self.inConf.GetWvBaseline() }
            self.writeToBinf(msg[0:self.mlen])
            outf = self.getRootOutf(self.inFilen)
            if (outf != None):
                outf.cd()
                self.inConf.Write()
                self.inRunf.Write()
                self.inNets.Write()
            else:
                printout(vtype.kError, "no output file to write config to")

    def handleFileHeader(self, msg):
        printout(vtype.kDebug, 'handleFileHeader:')
        printout(vtype.kDebug, 'len(msg)={0}, mlen={1}'.\
                     format(len(msg),self.mlen))
        self.inFileRunInfo = ROOT.TSnRunInfo()
        self.inFileMbed    = ROOT.TSnCompMbed()
        #self.inFileSeq     = array.array("H",[0]) # doesn't work with 5.34.01
        self.inFileSeq     = ROOT.Long()
        self.inFilePower   = ROOT.TSnPowerReading()
        br = ROOT.TSnIODataFile.PyReadFileHeader(msg,
                                                 self.inFileRunInfo,
                                                 self.inFileMbed,
                                                 self.inFileSeq,
                                                 self.inFilePower)
        printHex(vtype.kDebug, bytearray(msg))
        if (br==self.mlen):
            printout(vtype.kDebug, 'read {0} bytes'.format(br))
            printout(vtype.kInfo, 'Receiving run {r}, seq {s} from MBED {m}'.\
                         format(r=self.inFileRunInfo.GetRun(),
                                s=self.inFileSeq,
                                m=self.inFileMbed.GetMAC()))
            self.writeToBinf(msg[0:self.mlen])
            outf = self.getRootOutf(self.inFilen)
            if (outf != None):
                outf.cd()
                self.inFileRunInfo.Write()
                self.inFileMbed.Write()
                printout(vtype.kDebug, 'self.inFileSeq:')
                printout(vtype.kDebug, self.inFileSeq)
                pn = 'seq'
                p = ROOT.TParameter('UShort_t')(pn,self.inFileSeq)
                p.Write(pn)
                self.inFilePower.Write()
            else:
                printout(vtype.kError, 'no outf to write file header to')
    
    def handlePower(self, msg):
        self.inPower = ROOT.TSnPowerReading()
        br = ROOT.TSnIOPowerFrame.BytesReadFrom(
            msg, self.inPower)
        if (br==self.mlen):
            self.writeToBinf(msg[0:self.mlen])
            self.numInPows += 1
            outf = self.getRootOutf(self.inFilen)
            if (outf != None):
                outf.cd()
                self.inPower.Write()
            else:
                printout(vtype.kError, 'no outf to write power to')
        else:
            printout(vtype.kError, 'power read failed with br={0}'.format(br))
        
    def handleEvent(self, msg):
        if (self.inWvPack==None):
            raise RuntimeError("wave pack settings unknown")
        self.inEvent = ROOT.TSnEvent()
        br = ROOT.TSnIOEventFrame.BytesReadFrom(
            msg, self.inEvent, self.inWvPack['LSB'], 
            self.inWvPack['MSB'], self.inWvPack['base'], self.mlen)
        if (br==self.mlen):
            self.numInEvts += 1
            if ( (verb>=vtype.kInfo) and
                 ((self.numInEvts % updEvtCount)==0) ):
                sys.stderr.write("got {0} events           \r"
                                 .format(self.numInEvts))
            self.writeToBinf(msg[0:self.mlen])
            outf = self.getRootOutf(self.inFilen)
            if (outf != None):
                outf.cd()
                self.inEvent.Write()
            else:
                printout(vtype.kError, 'no outf to write event to')
        else:
            printout(vtype.kError, 'event read failed with br={0}'.format(br))
    
    def handleFilename(self, msg):
        self.inFilen = str(msg[:self.mlen])
        printout(vtype.kInfo, 'got filename [{0}]'.format(self.inFilen))
        self.numInEvts = 0
        self.numInPows = 0
        self.fbytes    = 0
        self.fstart    = time.time()
        if (self.start == None):
            self.start = time.time()
        self.binFile   = None
        self.rootFile  = None
        binf = self.getBinOutf(self.inFilen)
        outf = self.getRootOutf(self.inFilen)
        if (outf != None):
            outf.cd()
        else:
            printout(vtype.kError, 'could not make outf')

    def sendDataRcvResponse(self):
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
            rcvPowsFr = float(self.numInPows)/float(self.numPowsSent)
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
        self.numInPows = 0
        self.numInEvts = 0
        self.numPowsSent = 0
        self.numEvtsSent = 0
        if (self.rootFile!=None):
            self.rootFile.Close()
            self.rootFile = None
        if (self.binFile!=None):
            self.binFile.close()
            self.binFile = None
    
    def handleString(self, msg):
        printout(vtype.kUrgent, 'received string:')
        printout(vtype.kUrgent, msg)
    
    def handleHeartbeat(self, msg):
        hbt = ROOT.TTimeStamp()
        hbn = ROOT.Long()
        br = ROOT.TSnIOHeartbeatFrame.PyReadFrom(
            msg, hbt, hbn)
        if (br==self.mlen):
            self.writeToBinf(msg[0:self.mlen])
            outf = self.getRootOutf(self.inFilen)
            if (outf != None):
                outf.cd()
                outf.WriteObject(hbt, "heartbeat_{0:05d}".format(hbn))
            else:
                printout(vtype.kError, 'no outf to write heartbeat to')
        else:
            printout(vtype.kError, 'heartbeat read failed with br={0}'.format(br))
        

    def handleReadFail(self, msg):
        printout(vtype.kWarn, 'station read file failed!')
        try:
            x = getUInt(msg)
        except:
            printout(vtype.kError, 'read num for read fail failed')
        
    def handleNevtsSent(self, msg):
        try:
            self.numEvtsSent = getUInt(msg)
            printout(vtype.kInfo, 'Received {0}/{1} events'.\
                         format(self.numInEvts, self.numEvtsSent))
        except:
            printout(vtype.kError, 'failed to read UInt from:')
            printHex(vtype.kError, bytearray(msg))
                
    def handleNpwrsSent(self, msg):
        try:
            self.numPowsSent = getUInt(msg)
        except:
            printout(vtype.kError, 'failed to read UInt from:')
            printHex(vtype.kError, bytearray(msg))

        printout(vtype.kInfo, 'Received {0}/{1} powers'.\
                     format(self.numInPows, self.numPowsSent))
        if (verb>=vtype.kFileTimer):
            dt  = time.time() - self.start
            dft = time.time() - self.fstart
            tr  = self.tbytes/dt
            fr  = self.fbytes/dft
            printout(vtype.kFileTimer, 
                     "average total rate = {0} Bps ({1} bytes / {2} secs)".
                     format(tr, self.tbytes, dt))
            printout(vtype.kFileTimer, 
                     "this file ave rate = {0} Bps ({1} bytes / {2} secs)".
                     format(fr, self.fbytes, dft))
            self.ntof.write("{0} {1} {2} {3} {4} {5}\r\n"
                       .format(tr, self.tbytes, dt, fr, self.fbytes, dft) )
            self.ntof.flush()
        # Npwrs is the last thing sent
        # so the handshake is sent from here
        try:
            self.sendDataRcvResponse()
        except:
            printout(vtype.kError, sys.exc_info()[0])
            raise
    
    def handleMessage(self, message):
        printout(vtype.kDebugData, 'handleMessage:')
        printHex(vtype.kDebugData, bytearray(message))
        if (self.mcode == castHeaderCode(ROOT.TSnIOHeaderFrame.kStatusCode)):
            printout(vtype.kDebug, 'status!')
            self.handleStatusUpd(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileHeadrCode)):
            printout(vtype.kDebug, 'file header!')
            self.handleFileHeader(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFilenameCode)):
            printout(vtype.kDebug, 'filename!')
            self.handleFilename(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kConfigCode)):
            printout(vtype.kDebug, 'config!')
            self.handleConfig(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kEventCode)):
            printout(vtype.kDebug, 'event!')
            self.handleEvent(message)
            printout(vtype.kDebug,"doSendConf={0}, doSendBinf={1}".format(
                        self.doSendConf, self.doSendBinf))
            if (self.doSendConf):
                self.sendConf()
            elif (self.doSendBinf):
                self.sendBinf()
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kPowerCode)):
            printout(vtype.kDebug, 'power!')
            self.handlePower(message)
#            printout(vtype.kDebug, 'DEBUG:')
#            while (True):
#                sys.stdout.write(self.getData(1))
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kHeartbeatCode)):
            printout(vtype.kDebug, 'heartbeat!')
            self.handleHeartbeat(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileNevtsCode)):
            printout(vtype.kDebug, 'nevts!')
            self.handleNevtsSent(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileNpwrsCode)):
            printout(vtype.kDebug, 'npwrs!')
            self.handleNpwrsSent(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kFileReadFailCode)):
            printout(vtype.kDebug, 'read fail!')
            self.handleReadFail(message)
        elif (self.mcode == castHeaderCode(
                ROOT.TSnIOHeaderFrame.kStringCode)):
            printout(vtype.kDebug, 'string!')
            self.handleString(message)
        else:
            raise NotImplementedError(
                "Unknown expected message type {0:02x}"
                .format(self.mcode))



