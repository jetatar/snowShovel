import sys
import tornado
import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import cPickle
import datetime
import array
import struct
import base64

print 'tornado version = {0}'.format(tornado.version)

assert float(tornado.version) >= 2.3

import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat.so")
ROOT.gSystem.Load("$SNS/lib/libonline.so")

# switch: send text or binary
sendBinary = False

def enum(**enums):
    return type('Enum', (), enums)

# convert the UChar_t value from TSnIOHeaderFrame to an
# integer that can be passed between python & ROOT
kStatusCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kStatusCode)[0])
kFileCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kFileCode)[0])
kFilenameCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kFilenameCode)[0])
kConfAndEvtsCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kConfAndEvtsCode)[0])
kConfigCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kConfigCode)[0])
kEventCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kEventCode)[0])
kHandshakeCode=ROOT.Long(struct.unpack('B', ROOT.TSnIOHeaderFrame.kHandshakeCode)[0])

if (len(sys.argv)<2):
    print 'Need filename'
    sys.exit()

infn = sys.argv[1]

def splitFilenameExt(filename):
    return filename.rsplit('.',1)

if (splitFilenameExt(infn)[1] != 'dat'):
    print 'Need binary input file. Got [{0}]'.format(infn)
    sys.exit()


def printHex(x):
    print ''.join( [ '%02x ' % i for i in x ] )


def getDateStr():
    return datetime.datetime.now().strftime("%Y-%m-%d.%H-%M-%S")

class WSHandler(tornado.websocket.WebSocketHandler):
    def on_close(self):
        print 'connection closed'

    def open(self):
        print 'new connection'
        self.chunks    = {kStatusCode:[],
                          kFileCode:[],
                          kConfAndEvtsCode:[],
                          kConfigCode:[],
                          kEventCode:[],
                          kHandshakeCode:[]}
        self.mcode     = ROOT.Long()
        self.mlen      = ROOT.Long()
        self.outConf   = None
        self.outRunf   = None
        self.inConf    = None
        self.inRunf    = None
        self.inStatus  = None
        self.inEvent   = None
        self.inFilen   = None
        self.rootFile  = None
        self.incomEvts = 0
        self.numInEvts = 0
    
    def sendMsg(self, message):
        bnry=isinstance(message, bytes)
        if (sendBinary):
            if (bnry):
                self.write_message(mesage, binary=True)
            else:
                raise RuntimeError(
                    "sendBinary true, but message is not binary")
        else:
            if (bnry):
                message = base64.b64encode(message)
            self.write_message(message, binary=False)


    def sendConf(self):
        with open(infn,'rb') as inf:
            mm = [array.array('c', '0'*ROOT.TSnIOHeaderFrame.SizeOf()),
                  inf.read()]
            ROOT.TSnIOHeaderFrame.WriteTo(mm[0], kConfigCode, len(mm[1]))
            x = b''.join(y for y in mm[0])
            x = b''.join( [ x, mm[1] ] )
            print 'sending config: {0}'.format(len(x))
            printHex(bytearray(x))
            #self.write_message(x,binary=isinstance(x, bytes))
            self.sendMsg(x)
            print 'sent config'
            self.outConf = ROOT.TSnDAQConfig()
            self.outRunf = ROOT.TSnRunInfo()
            br = ROOT.TSnIOConfigFrame.BytesReadFrom(
                mm[1],self.outConf,self.outRunf)
            self.outConf.Print()
            self.outRunf.Print()
    
    def handleStatusUpd(self):
        msg = b''.join(self.chunks[kStatusCode]) # faster than string +=
        self.inStatus = ROOT.TSnStatusUpdate()
        self.inEvent  = ROOT.TSnEvent()
        br = ROOT.TSnIOStatusFrame.BytesReadFrom(
            msg,self.inStatus,self.inEvent,len(msg))
        if (br>0): # could check br == msgLen from header
            # message reading successful
            print 'status complete: msg len {0}'.format(len(msg))
            #printHex(bytearray(msg))
            self.inStatus.Print()
            self.inEvent.Print()
            # save stat/evt in root
            routfn = 'afarStatus/status.ws.{0}.root'.format(getDateStr())
            routf  = ROOT.TFile.Open(routfn,"recreate")
            routf.cd()
            self.inStatus.Write("status")
            self.inEvent.Write("evt")
            routf.Close()
            # send the config
            self.sendConf()
            
            # clear chunks
            self.chunks[kStatusCode] = []
        else:
            self.inStatus = None
            self.inEvent  = None
    
    def getRootOutf(self, infn, tag=''):
        if (self.rootFile!=None):
            return self.rootFile
        else:
            if (infn!=None):
                fn = 'afarStatus/{0}{1}.root'.format(
                    splitFilenameExt(infn)[0], tag)
                self.rootFile = ROOT.TFile.Open(
                    fn, 'recreate')
                return self.rootFile
            else:
                print 'getRootOutf: no infn'
                return None
    
    def sendHandshake(self):
        om = array.array('c', '0'*ROOT.TSnIOHeaderFrame.SizeOf())
        ROOT.TSnIOHeaderFrame.WriteTo(om, kHandshakeCode, 0)
        x = b''.join(y for y in om)
        print 'sending handshake: {0}'.format(len(x))
        printHex(bytearray(x))
        #self.write_message(x,binary=isinstance(x, bytes))
        self.sendMsg(x)
        print 'sent handshake'
        
    def handleEvent(self, loseLSB, loseMSB, baseline, blen):
        msg = b''.join(self.chunks[kEventCode]) # faster than string +=
        if (len(msg)>=self.mlen):
            self.inEvent = ROOT.TSnEvent()
            br = ROOT.TSnIOEventFrame.BytesReadFrom(
                msg, self.inEvent, loseLSB, loseMSB, baseline, self.mlen)
            if (br==self.mlen):
                outf = self.getRootOutf(self.inFilen)
                outf.cd()
                self.inEvent.Write()
                self.numInEvts += 1
                if (self.numInEvts >= self.incomEvts):
                    print 'got all events ({0}/{1})!'.format(
                        self.numInEvts, self.incomEvts)
                    self.incomEvts = 0
                    self.sendHandshake()
            else:
                print 'event read failed with br={0}'.format(br)
                self.inEvent = None
        else:
            print 'handleEvent: len(msg)={0} < self.mlen={1}'\
                .format(len(msg),self.mlen)
    
    def handleFilename(self):
        msg = b''.join(self.chunks[kFilenameCode])
        self.inFilen = str(msg[:self.mlen])
        print 'got filename [{0}]'.format(self.inFilen)

    def handleFile(self):
        msg = b''.join(self.chunks[kFileCode])
        if (len(msg)>=self.mlen):
            if (self.inFilen!=None):
                # TODO use TSnIODataFile to go to root file!
                with open('afarStatus/{0}'.format(self.inFilen),
                          'wb') as outf:
                    outf.write(msg)
            else:
                print 'FILE: inFilen is None!'
        else:
            print 'FILE: got {0} bytes, expect {1}'.format(
                len(msg), self.mlen)
    
    def handleConfAndEvts(self):
        msg = b''.join(self.chunks[kConfAndEvtsCode])
        if (len(msg)>=self.mlen):
            self.inConf = ROOT.TSnDAQConfig()
            self.inRunf = ROOT.TSnRunInfo()
            br = ROOT.TSnIOConfigFrame.BytesReadFrom(
                msg, self.inConf, self.inRunf)
            if (br==self.mlen):
                if (self.inStatus != None):
                    # add station label to run info
                    self.inRunf.SetStationLabel(
                        self.inStatus.GetMacAdrAsStr())
                outf = self.getRootOutf(self.inFilen, "Partial")
                outf.cd()
                self.inConf.Write()
                self.inRunf.Write()
                self.incomEvts = outConf.GetCommWinSendData()
                self.numInEvts = 0
            else:
                print 'handleConfAndEvts: read {0} bytes, wanted {1}'\
                    .format(br, self.mlen)
                self.inConf = None
                self.inRunf = None
            
        else:
            print 'CONFandEVTS: got {0} bytes, expect {1}'.format(
                len(msg), self.mlen)
            
        
    
    def on_message(self, message):
        print 'on_message'
        bnry = isinstance(message, bytes)
        print 'binary = {0}'.format(bnry)
        if (not bnry):
            print 'b64 encoded:'
            print message
            message = base64.b64decode(message)
            print 'b64 decoded:'
        printHex(bytearray(message))
        # try to read
        hdbyt = ROOT.TSnIOHeaderFrame.PyReadFrom(message, 
                                                 self.mcode,
                                                 self.mlen)
        print 'header: mcode={0:02x}, mlen={1}'.format(self.mcode,
                                                       self.mlen)
        self.chunks[self.mcode].append(message[hdbyt:])
        print 'chunks:'
        print self.chunks
        if (self.mcode == kStatusCode):
            print 'status!'
            self.handleStatusUpd()
        elif (self.mcode == kFileCode):
            print 'file!'
            self.handleFile()
        elif (self.mcode == kFilenameCode):
            print 'filename!'
            self.handleFilename()
        elif (self.mcode == kConfAndEvtsCode):
            print 'ConfAndEvts!'
            self.handleConfAndEvts()
        elif (self.mcode == kConfigCode):
            print 'config! NOT HANDLED'
        elif (self.mcode == kEventCode):
            print 'event!'
            self.handleEvents()
        else:
            raise NotImplementedError(
                "Unknown expected message type {0:02x}"
                .format(self.mcode))
        
          
application = tornado.web.Application([(r'/ws', WSHandler)],debug=True)

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(6776)
    print 'listening...'
    tornado.ioloop.IOLoop.instance().start()

