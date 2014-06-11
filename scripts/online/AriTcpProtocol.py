from AriStationProtocol import *

# TCP port to listen to
lport = 6655

setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebug)
#setVerbosity(vtype.kDebugData)


def startupMsg():
    printout(vtype.kInfo, "Listening on port {0}".format(lport))

def main():
    global lport

    log.startLogging(sys.stdout)

    from twisted.internet import reactor
    
    tcpFactory = AriStnProtFactory(reactor)
    
    reactor.listenTCP(lport, tcpFactory)
    reactor.callWhenRunning(startupMsg)
    reactor.run()
    

if __name__=="__main__":
    main()
