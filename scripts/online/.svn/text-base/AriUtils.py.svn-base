import os, sys, errno
import struct
import datetime, calendar

# constant: sequences per directory (match MBED)
SEQ_PER_DIR=int(100)

USE_LOG=True
try:
    from twisted.python import log
    USE_LOG=True
except ImportError:
    USE_LOG=False

import ROOT


def enum(**enums):
    return type('Enum', (), enums)

vtype=enum(kSilent=0,  kError=10, kUrgent=20,
           kWarn=30,   kInfo=40,  kFileTimer=50,
           kDebug=100, kDebugData=150)

verb=vtype.kInfo

def setVerbosity(level):
    global verb
    verb = level

def getVerbosity():
    global verb
    return verb

def printout(level, text):
    global verb
    if (verb>=level):
        if (USE_LOG):
            log.msg(text)
        else:
            print text

def printHex(level, x):
    global verb
    if (verb>=level):
        # avoid making the bytearray and string if we don't have to
        printout(level, ''.join( [ '%02x ' % i for i in bytearray(x) ] ))

def splitFilenameExt(filename):
    if '.' in filename:
        return filename.rsplit('.',1)
    else:
        return ['','']

def parseSnEvtsFilename(fn):
    # assumes filename has the form
    #  SnEvtsM0002F7F0C445r00014s00369.dat
    rst = fn.index("r")
    sst = fn.rindex("s")
    mac = fn[fn.index("M")+1:rst]
    run = int(fn[rst+1:sst])
    seq = 0
    if (fn.rfind("_")!=-1):
        seq = int(fn[sst+1:fn.rindex("_")])
    else:
        seq = int(fn[sst+1:fn.index(".")])
    return mac,run,seq

def getDirForSeq(seq):
    sdr = 's{0:05d}'.format( (int(seq)/SEQ_PER_DIR)*SEQ_PER_DIR )
    return sdr

def getSubDirFor(mac, run, seq):
    sdir = "{0}/r{1:05d}/{2}".format(mac,run,getDirForSeq(seq))
    return sdir

def getDateStrFrom(dtime):
    return dtime.strftime("%Y-%m-%d.%H-%M-%S")

def getDateStrFromTimeStamp(ts):
    s = ts.AsString("s")
    s = s.replace(' ','.')
    s = s.replace(':','-')
    return s

def getDateStr():
    return getDateStrFrom(datetime.datetime.now())

def getUnixTimeFrom(dattime):
    return calendar.timegm(dattime.utctimetuple())

def getTimeStampFrom(dattime):
    return ROOT.TTimeStamp(getUnixTimeFrom(dattime))

class macStnMap:
    unknownStn = 0
    unknownMac = "000000000000"
    stnToMac = {  3:"0002F7F0C3B6",  4:"0002F7F0C41C",
                  6:"0002F7F0C445",  8:"0002F7F0C0F8",
                 10:"0002F7F0C61A", 11:"0002F7F175B7",
                 12:"0002F7F0C561", 13:"0002F7F0AEE0"}
    macToStn = dict( zip(stnToMac.values(), stnToMac.keys()) )

def getMacAdrFromStn(stnnum):
    try:
        return macStnMap.stnToMac[stnnum]
    except KeyError as e:
        printout(vtype.kError, "Unknown station number [{0}]".format(stnnum))
        return macStnMap.unknownMac

def getStnFromMacAdr(mac):
    printout(vtype.kDebug, "getStnFromMacAdr({0})".format(mac))
    try:
        if (isinstance(mac,(int,long))):
            printout(vtype.kDebug, "convert to string")
            mac = ROOT.TSnCompMbed.GetMACFromVal(mac)
            printout(vtype.kDebug, "mac str = {0}".format(mac))
        return macStnMap.macToStn[mac]
    except KeyError as e:
        printout(vtype.kError, "Unknown mac address {0}".format(mac))
        return macStnMap.unknownStn
    
def checkDirExists(d):
    if not os.path.exists(d):
        try:
            os.makedirs(d)
        except OSError as ex:
            if ex.errno != errno.EEXIST:
                raise

def checkDirOfFileExists(fn):
    if ( (fn!=None) and (fn.find('/')>-1) ):
        checkDirExists(os.path.dirname(fn))

# convert the UChar_t value from TSnIOHeaderFrame to an
# integer that can be passed between python & ROOT
def castHeaderCode(code):
    return ROOT.Long(struct.unpack('<B', code)[0])

def getUInt(msg):
    # don't use for a TSnIOBuffer. instead, use TSnIOObject.ReadFromUInt
    printout(vtype.kDebug,'getUInt from:')
    printHex(vtype.kDebug, bytearray(msg))
    # station (MBED) is little endian
    return ROOT.Long(struct.unpack('<I', msg[0:4])[0])

def getUChar(msg):
    # don't use for a TSnIOBuffer. instead, use TSnIOObject.ReadFromUChar
    return ROOT.Long(struct.unpack('<B', msg[0:1])[0])

def getDataFromBuf(buf, fromCurrPos=True):
    # get a string from the TSnIOBuffer
    # this is the fastest way -- see http://stackoverflow.com/questions/18219718/python-buffer-copy-speed-why-is-array-slower-than-string
    s = 0
    if (fromCurrPos):
        s = int(buf.GetPosVal())
    return b''.join((buf.AsVector())[s:])



