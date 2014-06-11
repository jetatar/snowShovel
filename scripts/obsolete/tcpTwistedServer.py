import sys
import datetime
import array
import struct
import socket
import base64
from stationIO import *

# port to listen to
lport = 6655


from twisted.internet import protocol, reactor
from twisted.internet.defer import Deferred

infn = ''
if (len(sys.argv)>1):
    infn = sys.argv[1]

inext = splitFilenameExt(infn)[1]
if ( (inext != 'dat') and (inext!='DAT') and (inext!='') and
     (inext != 'bin') and (inext!='BIN') ):
    print 'Need binary input file. Got [{0}]'.format(infn)
    sys.exit()


class stationProtocol(communikationizer, protocol.Protocol):
    firstHeader = True
    
    def connectionMade(self):
        self.peer = self.transport.getPeer()
        printout(vtype.kInfo, "Connected from {0}:{1} ({2})"\
                     .format(self.peer.host, self.peer.port, self.peer.type))
        self.setup()
    
    def dataReceived(self, data):
        printout(vtype.kDebugData, 'dataReceived:')
        printHex(vtype.kDebugData, bytearray(data))
        
        self.buff.append(data)
        bd = b''.join(self.buff)
        
        printout(vtype.kDebugData, 'full buffer now:')
        printHex(vtype.kDebugData, bytearray(bd))
        
        while ( len(bd) >= self.HEADER_LEN ):
            
            printout(vtype.kDebug, 'get header from:')
            printHex(vtype.kDebug, bytearray(bd[0:self.HEADER_LEN]))
            
            self.handleHeader(bd)
            self.firstHeader = False
            if ( len(bd) < self.HEADER_LEN + self.mlen ):
                printout(vtype.kDebug, 
                         'msg incomplete: HDR+mlen={0}, buffer={1}'
                         .format(self.HEADER_LEN+self.mlen,len(bd)))
                break # don't have the whole message yet. wait for more data
            else:
                self.getFromBuff(self.HEADER_LEN) # remove the header
                msg = self.getFromBuff(self.mlen)
                self.handleMessage(msg)
            bd = b''.join(self.buff)
            
            printout(vtype.kDebugData, 'full buffer now:')
            printHex(vtype.kDebugData, bytearray(bd))
        
    def SendData(self, message, binary):
        self.transport.write(message)

    def GetInConfName(self):
        return infn
    
    def connectionLost(self, reason):
        printout(vtype.kInfo, "Disconnected from {0}:{1} ({2})"\
                     .format(self.peer.host, self.peer.port, reason.value))


tcpFactory = protocol.Factory()
tcpFactory.protocol = stationProtocol


def newConnMsg():
    printout(vtype.kInfo, "listening on port {0}".format(lport))

reactor.listenTCP(lport, tcpFactory)
reactor.callWhenRunning(newConnMsg)
reactor.run()

