import sys
import time
import datetime
import array
import struct
import serial
import base64

from stationIO import *

#baud=460800
#baud=230400
baud=115200
#baud=9600

# set the verbosity (default defined in stationIO)
#verb = vtype.kInfo


usbdevice = ''
if (len(sys.argv)<2):
    printout(vtype.kError,
             'First argument is USB usbdevice (i.e. "/dev/ttyACM0")')
    sys.exit()
else:
    usbdevice = sys.argv[1]

infn = ''
if (len(sys.argv)>2):
    infn = sys.argv[2]

# can't transfer a new binary file via usb, because once a local file
# handle is opened on the mbed, it disconnects the usb
# this is done to prevent the mbed and the external usb connection from
# writing to the same local "file" space
inext = splitFilenameExt(infn)[1]
if ( (inext != 'dat') and (inext!='DAT') and (inext!='') ):
    # and (inext != 'bin') and (inext!='BIN') ):
    printout(vtype.kError,
             'Need binary input file. Got [{0}]'.format(infn))
    sys.exit()


# make the serial object
s = serial.Serial(usbdevice, baud)
s.flushInput()
s.flushOutput()

# spawned by the communicator to continuously listen for
# data from this connection
class serialListener(threadListener):
    def Setup(self):
        self.conn      = s
        self.connRead  = self.conn.read
        self.conInWait = self.conn.inWaiting
    
    def DoRead(self, nbytes):
        return self.connRead(nbytes)

    def BytesToRecv(self):
        return 1

    def BytesWaiting(self):
        return self.conInWait()

# spawned by the server upon a connection
class usbCommunikationizer(communikationizer):
    def SetSerial(self, conn):
        self.conn = conn
    
    def Close(self):
        self.conn.close()
    
    def SendData(self, message, binary):
        return self.conn.write(message)

    def GetInConfName(self):
        return infn

    def GetAListener(self):
        return serialListener()

printout(vtype.kInfo, 'sending break (restart)...')

s.sendBreak()
s.sendBreak()

printout(vtype.kInfo, 'starting usb server...')

start = time.time()
ucomm = usbCommunikationizer()
ucomm.SetSerial(s)
ucomm.setup()
ucomm.handle()

printout(vtype.kFileTimer, 'ran in {0} seconds'.format(time.time() - start))
