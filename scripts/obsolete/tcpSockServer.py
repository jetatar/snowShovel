import sys
import datetime
import array
import struct
import socket
import SocketServer
import base64

from stationIO import *

# port to listen to
lport = 6655

infn = ''
if (len(sys.argv)>1):
    infn = sys.argv[1]

inext = splitFilenameExt(infn)[1]
if ( (inext != 'dat') and (inext!='DAT') and (inext!='') ):
    print 'Need binary input file. Got [{0}]'.format(infn)
    sys.exit()


# spawned by the communicator to continuously listen for
# data from this connection
class tcpListener(threadListener):
    def Setup(self):
        self.reqRead = self.comm.request.recv
    
    def DoRead(self, nbytes):
        #print 'reading {0} bytes'.format(nbytes)
        m = self.reqRead(nbytes)
        #print 'got msg: {0}'.format(len(m))
        #printHex(vtype.kError, bytearray(m))
        return m
        #return self.reqRead(nbytes)

    def BytesWaiting(self):
        return 0
    
    def BytesToRecv(self):
        return 4096
        #return 1

# spawned by the server upon a connection
class tcpCommunikationizer(communikationizer, SocketServer.BaseRequestHandler):
    def Close(self):
        self.request.finish()
    
    def SendData(self, message, binary):
        self.request.sendall(message)

    def GetInConfName(self):
        return infn
    
    def GetAListener(self):
        return tcpListener()

# combine the TCPServer with a threading server
# want to launch a TCPServer in a new thread with each incoming connection
# (to handle communication with mulitple stations simultaneously)
class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass

address=('', lport)
server = ThreadedTCPServer(address, tcpCommunikationizer)
#server = SocketServer.TCPServer(address, tcpCommunikationizer)
printout(vtype.kInfo, 'starting server...')
server.timeout = 0.001
server.serve_forever()

