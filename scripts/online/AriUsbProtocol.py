from AriStationProtocol import *
from twisted.internet.serialport import SerialPort

#baud=460800
baud=115200
usbdevice='/dev/ttyACM0'
igTime=2

setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebug)
#setVerbosity(vtype.kDebugData)

def startupMsg():
    global baud
    global usbdevice
    printout(vtype.kInfo,"Listening to [{0}] at [{1}] baud..."
             .format(usbdevice, baud))

class AriUsbStnProtocol(AriStationProtocol):
    def __init__(self):
        AriStationProtocol.__init__(self)
        self.ignoreIncoming = True
    
    def dataReceived(self, data):
        if (self.ignoreIncoming):
            pass
        else:
            AriStationProtocol.dataReceived(self, data)

    def StopIgnoring(self, x=None):
        self.ignoreIncoming = False


def main():
    global baud
    global usbdevice
    global igTime
    
    print 'starting logger'
    log.startLogging(sys.stdout)
    
    if (len(sys.argv)>1):
        usbdevice = sys.argv[1]

    from twisted.internet import reactor
 
   
    apFactory = AriStnProtFactory(reactor)
    apFactory.protocol = AriUsbStnProtocol
    usbAriProtocol = apFactory.buildProtocol(None)
    
    port = SerialPort(usbAriProtocol, usbdevice,
                      reactor, baud)

    # reset signal -- we cannot receive bytes in the middle of a transfer!
    # TCP and SBD are not susceptible to this as the data gets buffered
    printout(vtype.kInfo,"Resetting MBED...")
    port.sendBreak()
    printout(vtype.kInfo,"Ignoring incoming bytes for {0} seconds "
             "to clear RX buffer..."
             .format(igTime))
    reactor.callWhenRunning(startupMsg)
    reactor.callLater(igTime, usbAriProtocol.StopIgnoring)
    reactor.run()
    

if __name__=="__main__":
    main()

