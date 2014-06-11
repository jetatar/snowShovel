from twistedIO import *
from twisted.internet.serialport import SerialPort

#baud=921600
baud=460800
#baud=230400
#baud=115200
#baud=9600

# station to talk to
#macadr="0002F7F0C3B6" # stn3 (east)
#macadr="0002F7F0C445" # stn6 (north)
#macadr="0002F7F0C41C" # stn4 (center)
macadr="0002F7F0C0F8" # stn8 (test)
#macadr=None # not talking to anyone

#setVerbosity(vtype.kDebug)
setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebugData)

usbdevice = ''
infn = ''

class AriUSBStationProtocol(stationProtocol):
    def GetInConfName(self):
        return infn

    def IsTalkingTo(self, mac):
        return mac==macadr

def startupMsg():
    printout(vtype.kInfo, "Listening to {0}".format(usbdevice))

def main():
    # start logging - this must come after AriUtils is imported
    print 'starting logger'
    log.startLogging(sys.stdout)
    
    if (len(sys.argv)<2):
        printout(vtype.kError,
                 'First argument is USB usbdevice (i.e. "/dev/ttyACM0")')
        sys.exit()
    else:
        usbdevice = sys.argv[1]

    infn = ''
    if (len(sys.argv)>2):
        infn = sys.argv[2]

    inext = splitFilenameExt(infn)[1]
    if ( (inext != 'dat') and (inext!='DAT') and (inext!='') and
         (inext != 'bin') and (inext!='BIN') ):
        print 'Need binary input file. Got [{0}]'.format(infn)
        sys.exit()

    usbFactory = protocol.Factory()
    usbFactory.protocol = AriUSBStationProtocol
    usbAriProtocol = usbFactory.buildProtocol(None)

    from twisted.internet import reactor
    
    port = SerialPort(usbAriProtocol, usbdevice, reactor, baud)
    # reset signal
    printout(vtype.kInfo,"Resetting MBED...")
    port.sendBreak()
    port.sendBreak()
    reactor.callWhenRunning(startupMsg)
    reactor.run()

if __name__=="__main__":
    main()

