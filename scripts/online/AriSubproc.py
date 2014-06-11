# code from http://code.activestate.com/recipes/578021-async-subprocess-check_output-replacement-for-twis/

import os
from AriUtils import printout,vtype
from twisted.internet.defer import Deferred
from twisted.internet.error import ProcessDone
from twisted.internet.protocol import ProcessProtocol

class SubprocessProtocol(ProcessProtocol):
    outBuffer = ""
    errBuffer = ""

    def connectionMade(self):
        self.d = Deferred()

    def outReceived(self, data):
        printout(vtype.kInfo, data)
        self.outBuffer += data

    def errReceived(self, data):
        printout(vtype.kError, data)
        self.errBuffer += data

    def processEnded(self, reason):
        if reason.check(ProcessDone):
            self.d.callback(self.outBuffer)
        else:
            self.d.errback(reason)

def async_check_output(args, ireactorprocess=None):
    """
    :type args: list of str
    :type ireactorprocess: :class: twisted.internet.interfaces.IReactorProcess
    :rtype: Deferred
    """
    if ireactorprocess is None:
        from twisted.internet import reactor
        ireactorprocess = reactor
        
    pprotocol = SubprocessProtocol()
    #ireactorprocess.spawnProcess(pprotocol, args[0], args)
    ireactorprocess.spawnProcess(pprotocol, args[0], args, os.environ)
    return pprotocol.d
