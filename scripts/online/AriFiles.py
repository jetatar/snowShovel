from twisted.internet import defer, task, threads
import os
from AriUtils import *

# the script importing this file must call AriUtils.loadOnlineLibs()

def ensureNoOverwrite(fn):
    printout(vtype.kDebug,"ensureNoOver {0}".format(fn))
    # first add the date (before the extension)
    # hopefully that renders it unique
    pfn, ext = os.path.splitext(fn)
    dtag = getDateStr()
    ofn = fn
    if (os.path.exists(ofn)):
        ofn = "{0}_{1}{2}".format(pfn, dtag, ext)
        # in case it's still not unique, add a seq tag
        pfn, ext = os.path.splitext(ofn)
        seq = 1
        while os.path.exists(ofn):
            ofn = "{0}_{1}{2}".format(pfn,seq,ext)
            seq += 1
        printout(vtype.kDebug,"unique name {0}".format(ofn))
    return ofn


class AriAsyncFile(object):
    '''
    Base class for files that need asynchronous access.
    Use DeferUntilAvailable to get a deferred that will fire
    when the file is available (note: the file is not opened automatically).
    The default timeout is 20 seconds.
    '''

    def __init__(self, fname, lockTimeout=20):
        '''
        fname = filename
        lockTimeout = number of seconds after which to give up trying
                      to lock the file (None = try forever)
        '''
        self.fname      = fname
        self.timeout    = lockTimeout # seconds
        self.filelock   = None
        printout(vtype.kDebug, "Checking dir exists for [{0}]".format(fname))
        checkDirOfFileExists(fname)

    def __del__(self):
        # remove the file lock
        self.done()

    #
    # "private" functions
    #
    
    def _removeFilelock(self):
        if (self.filelock!=None):
            if (self.filelock.locked):
                self.filelock.unlock()

    def _uponFail(self, msg, loc):
        printout(vtype.kError, "FAIL at {0}: {1}".format(loc,msg))

    def _neverLocked(self, err=None):
        printout(vtype.kError, "ERROR. Unable to obtain access to file "
                 "[{0}] within [{1}] seconds: {2}"
                 .format(self.fname,self.timeout,err))
        return err
    
    #
    # "public" functions
    #

    def ensureNoOverwrite(self, fn):
        return ensureNoOverwrite(fn)

    def GetFilename(self):
        return self.fname
    
    def DeferUntilAvailable(self):
        '''
        return a deferred that fires when the file is available
        (ready to be opened). the file is not opened automatically.
        the deferred will errback if the file cannot be locked
        within the specified timeout
        '''
        if (self.filelock==None):
            self.filelock = defer.DeferredFilesystemLock(
                "{0}.lock".format(self.fname))
            ### to test what happens if we timeout
            #if (isinstance(self, AriBinFile)): # or whatever type
            #    def forever():
            #        pass
            #    self.filelock.lock = forever
            ###
        d = self.filelock.deferUntilLocked(self.timeout)
        d.addErrback(self._neverLocked)
        return d

    def setTimeout(self, lockTimeout):
        self.timeout = lockTimeout
        
    def done(self, unused=None):
        self._removeFilelock()


class AriBinFile(AriAsyncFile):
    '''
    A simple binary file with asynchronous access (handled behind the
    scenes). Data can be written to the file in several ways:
    
    (a) By repeated calls to storeForWriting, followed by a call to
    writeBufWhenAble.
    This will will only attempt to write the data to disk once.
    A function (and optional parameters) may be specified that will
    be called via an errback (so the first parameter to the fcn will be
    the error) in case the file does not become available within the
    specified timeout.

    (b) By repeated calls to writeWhenAble.
    This might cause the file to be opened and closed multiple times,
    potentially once per call to writeWhenAble.

    In either case, a function (and optional parameters) may be
    specified that will be called via an errback (so the first
    parameter to the fcn will be the error) in case the file does not
    become available within the specified timeout.  NOTE: if a
    call-upon-timeout function is specified, it should be the same
    function in every writeWhenAble call for the file. This function
    is not updated on each call; it is simply added as an errback the
    first time DeferUntilAvailable is called.
    
    See storeForWriting for a description of the types of data that may be
    passed in.
    
    The filename passed to __init__ may not be the filename actually
    used, as a unique filename will be generated if necessary to avoid
    overwriting an existing file. See AriAsyncFile.ensureNoOverwrite.
    '''

    kFullBuffLen = 8192 # when the buffer reaches this many bytes, try to write

    def __init__(self, fname, openOption="ab"):
        fn = self.ensureNoOverwrite(fname)
        super( AriBinFile, self ).__init__(fn)
        self._buf           = ROOT.TSnIOBuffer()
        self._whenAvailable = None
        self._openOpt       = openOption

    #
    # "private" functions
    #
    
    def _clearBuffer(self):
        self._buf = ROOT.TSnIOBuffer()

    def _writeData(self, deferedResult=None):
        '''
        opens & closes the file each time...
        probably a lot of overhead, but on the positive side,
        we're assured that the data makes it to disk immediately.
        if the data gets written, the buffer is cleared, the file
        lock is removed and the deferred waiting for the file to
        be available is cleared
        '''
        
        # opening the file and writing via Python was very slow
        # the time was taken either making a bytearray out of the buffer
        # or looping over the buffer and writing.
        # having the buffer do it for us is much quicker
        s = self._buf.WriteTo(self.fname, self._buf.begin())
        if (s>0):
            self._clearBuffer()
        if (self._buf.size()==0):
            self.done() # remove the file lock
            self._noLongerWaiting() # clear the deferred
    
    @defer.inlineCallbacks
    def _calcCRC(self):
        '''
        Open the file and calculate the CRC. This may be expensive
        and it blocks, so we run it in another thread.
        It also opens the file, so the file should be closed and done
        with before calling.
        '''
        crc = yield threads.deferToThread(ROOT.NSnCRCUtils.GetCRC32for,
                                          self.fname)
        defer.returnValue(crc)


    def _noLongerWaiting(self, ignoredResult=None):
        '''
        no more waiting
        '''
        self._whenAvailable = None

    #
    # "public" functions
    #

    def storeForWriting(self, data, callIfUnable=None, *args, **kw):
        '''
        add the data to this file's buffer, but do not write it to disk
        (necessarily). writing the buffer to disk (via writeBufWhenAble)
        will be attempted if the buffer exceeds self.kFullBuffLen
        
        data can be:
         - a TSnIOBuffer => all data from the current position in the
                             buffer to the end will be written
         - a str or bytearray => entire string/bytearray will be written
         - a tuple: (TSnIOBuffer, length)
                    => a part of the buffer from the current position
                       of length 'length' will be written
         - a tuple: (TSnIOBuffer, startPos, length)
                    => a part of the buffer from and including startPos
                       of length 'length' will be written
        '''
        if ( (isinstance(data, ROOT.TSnIOBuffer)) or
             (isinstance(data, bytearray)) or
             (isinstance(data, str)) ):
            self._buf.AppendQuiet(data)
        elif ( (isinstance(data, tuple)) and (len(data)==2) ):
            if (isinstance(data[0], ROOT.TSnIOBuffer)):
                b = data[0]
                self._buf.AppendQuiet(b, b.GetPosVal(), data[1])
            else:
                raise ValueError("Cannot write tuple of data with first "
                                 "entry of type {0}".format(type(data[0])))
        elif ( (isinstance(data, tuple)) and (len(data)==3) ):
            if (isinstance(data[0], ROOT.TSnIOBuffer)):
                self._buf.AppendQuiet(data[0], data[1], data[2])
            else:
                raise ValueError("Cannot write tuple of data with first "
                                 "entry of type {0}".format(type(data[0])))
        else:
            raise ValueError("Cannot write data of type {0}".format(
                    type(data)))
        
        if (self._buf.size() > self.kFullBuffLen):
            # buffer bloated. write it.
            self.writeBufWhenAble(callIfUnable, *args, **kw)

    def writeWhenAble(self, data, callIfUnable=None, *args, **kw):
        '''
        open the file when it's available,
        write the data to the file and then
        close the file (see _writeData). probably a lot of overhead
        but it ensures the data makes it to disk.
        
        data can be (see storeForWriting):
         - a TSnIOBuffer => all data from the current position in the
                             buffer to the end will be written
         - a str or bytearray => entire string/bytearray will be written
         - a tuple: (TSnIOBuffer, length)
                    => a part of the buffer from the current position
                       of length 'length' will be written
         - a tuple: (TSnIOBuffer, startPos, length)
                    => a part of the buffer from and including startPos
                       of length 'length' will be written
        
        it's fine to call this fcn multiple times before the file lock
        is obtained, as data is stored in the buffer until the file is
        available.
        '''

        # add to the buffer
        self.storeForWriting(data)
        # write the buffer
        self.writeBufWhenAble(callIfUnable, *args, **kw)
    
    def writeBufWhenAble(self, callIfUnable=None, *args, **kw):
        if (self._whenAvailable==None):
            if (self._buf.size()>0):
                self._whenAvailable = self.DeferUntilAvailable()
                # wait for the file to be available, then write the buffer
                if (callIfUnable!=None):
                    # add this first because the deferred may already have
                    # fired, and _writeData will clear the deferred.
                    self._whenAvailable.addErrback(callIfUnable, *args, **kw)
                else:
                    # catch the exception passed on by _neverLocked
                    def donuttin(x=None):
                        pass
                    self._whenAvailable.addErrback(donuttin)
                self._whenAvailable.addCallback(self._writeData)
            # else nothing to write
        # else already waiting
        return self._whenAvailable # for use by inline callbacks

    @defer.inlineCallbacks
    def GetCRC(self):
        '''
        Wait until file is available and then calculate
        '''
        crc = ROOT.Long(0)
        try:
            # wait for availability
            yield self.DeferUntilAvailable()
            # wait for the crc to be calculated
            crc = yield self._calcCRC()
        except Exception as err:
            self._uponFail("Could not get CRC due to error [{0}]".format(
                    str(err)), "GetCRC")
        # pass it on
        defer.returnValue(crc)

        
class AriStatusBinFile(AriBinFile):
    '''
    an AriBinFile that knows how to set its own name given a
    TSnStatusUpdate object. a unique filename is guaranteed by
    AriBinFile.
    '''
    
    def __init__(self, sdir, status):
        fn = self._getOutFname(status)
        fname = "{0}/{1}".format(sdir, fn)
        super( AriStatusBinFile, self ).__init__(fname)
        
    #
    # "private" functions
    #
    
    def _getOutFname(self, status):
        return 'status.{0}.{1:08d}-{2:06d}.dat'.format(
            status.GetMacAdrAsStr(),
            status.GetStatusTime().GetDate(),
            status.GetStatusTime().GetTime())

class AriSBDEmailFile(AriBinFile):
    '''
    an AriBinFile that knows how to set its own name given a and SBD
    IMEI and a datetime.datetime object. a unique filename is
    guaranteed by AriBinFile.
    '''
    
    def __init__(self, odir, imei, sbddatetime):
        fn = self._getOutFname(imei, sbddatetime)
        fname = "{0}/{1}".format(odir, fn)
        super( AriSBDEmailFile, self ).__init__(fname)
        
    #
    # "private" functions
    #
    
    def _getOutFname(self, imei, sbddatetime):
        datest = sbddatetime.strftime('%Y-%m-%d_%H-%M-%S')
        return "SBD_{0}_{1}.msg".format(imei,datest)

class AriSigStrBinFile(AriBinFile):
    '''
    an AriBinFile that knows how to set its own name given a
    ROOT.TSnSignalStrengthInfo object. a unique filename is
    guaranteed by AriBinFile.
    '''
    
    def __init__(self, odir, sigstr):
        fn = self._getOutFname(sigstr)
        fname = "{0}/{1}".format(odir, fn)
        super( AriSigStrBinFile, self ).__init__(fname)
        
    #
    # "private" functions
    #
    
    def _getOutFname(self, sigstr):
        ct = ROOT.TSnDAQConfig.PyGetDatPackBitName(sigstr.PyGetCommType())
        ROOT.SetOwnership(ct, True)
        ds = getDateStrFromTimeStamp(sigstr.GetSigStrTime())
        return "SigStr_ct{0}_{1}.msg".format(ct, ds)


class AriROOTFile(AriAsyncFile):
    def __init__(self, fname, fopt="READ", lockTimeout=120):
        super( AriROOTFile, self ).__init__(fname, lockTimeout)
        self.fopt = fopt
        self.RF   = None

    def __del__(self):
        self.Close()

    #
    # private functions
    #
    
    def _openRootFile(self, unused, d):
        self.RF = ROOT.TFile(self.fname, self.fopt)
        d.callback(self.RF)

    #
    # public functions
    #
    
    def DeferUntilOpen(self):
        '''
        try to open the ROOT file when it's available
        
        returns a deferred that fires when the file is open
        (or immediately, if the file is already opened)
        
        the result of the deferred is the ROOT.TFile object
        '''
        
        d = defer.Deferred()
        # is the file already open? then don't wait for it.
        doit = (self.RF==None)
        if (doit==False):
            doit = (self.RF.IsOpen()==False)
        if (doit):
            # _openRootFile will fire the callback
            whenAv = self.DeferUntilAvailable()
            whenAv.addCallback(self._openRootFile, d)
        else:
            # already open -- fire the callback now
            d.callback(self.RF)
        return d
    
    def Close(self):
        # close the ROOT file
        if (self.RF!=None):
            self.RF.Close()
            self.RF = None
        self.done()

    
