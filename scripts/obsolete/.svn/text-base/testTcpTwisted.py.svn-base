import sys
import datetime
import array
import struct
import socket
import base64

# port to listen to
lport = 6666

def printHex(x):
    print ''.join( [ '%02x ' % i for i in x ] )

from twisted.internet import protocol, reactor

class testProtocol(protocol.Protocol):
    def connectionMade(self):
        self.peer = self.transport.getPeer()
        print "Connected from {0}:{1} ({2})"\
                     .format(self.peer.host, self.peer.port, self.peer.type)
    
    def dataReceived(self, data):
        print 'dataReceived:'
        printHex(bytearray(data))
        
    def connectionLost(self, reason):
        print "Disconnected from {0}:{1} ({2}"\
                     .format(self.peer.host, self.peer.port, reason.value)


tcpFactory = protocol.Factory()
tcpFactory.protocol = testProtocol


def newConnMsg():
    print "listening on port {0}".format(lport)

reactor.listenTCP(lport, tcpFactory)
reactor.callWhenRunning(newConnMsg)
reactor.run()


