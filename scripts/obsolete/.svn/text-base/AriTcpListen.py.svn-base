from twistedIO import *

infn = ''
inext = ''

# port to listen to
lport = 6655

# station to talk to
#macadr="0002F7F0C3B6" # stn3 (east)
#macadr="0002F7F0C445" # stn6 (north)
#macadr="0002F7F0C41C" # stn4 (center)
macadr=None # not talking to anyone

setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebugData)

class AriTcpStationProtocol(stationProtocol):
    def GetInConfName(self):
        global infn
        return infn

    def IsTalkingTo(self, mac):
        global macadr
        return mac == macadr

def startupMsg():
    printout(vtype.kInfo, "Listening on port {0}".format(lport))

def main():
    
    global infn
    global inext
    
    if (len(sys.argv)>1):
        infn = sys.argv[1]

    inext = splitFilenameExt(infn)[1]
    if ( (inext != 'dat') and (inext!='DAT') and (inext!='') and
         (inext != 'bin') and (inext!='BIN') ):
        print 'Need binary input file. Got [{0}]'.format(infn)
        sys.exit()

    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)

    tcpFactory = protocol.Factory()
    tcpFactory.protocol = AriTcpStationProtocol

    from twisted.internet import reactor
    reactor.listenTCP(lport, tcpFactory)
    reactor.callWhenRunning(startupMsg)
    reactor.run()

if __name__=="__main__":
    main()
