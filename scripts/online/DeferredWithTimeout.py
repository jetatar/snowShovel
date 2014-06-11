from twisted.internet import defer

class DeferredWithTimeout(defer.Deferred):
    '''
    A deferred that allows a specified function to be called-back
    if the deferred does not fire before some specified timeout.
    '''
    def __init__(self, canceller=None):
        defer.Deferred.__init__(self, canceller)
        
    def _finish(self, r, t):
        '''
        Function to be called (internally) after the Deferred
        has fired, in order to cancel the timeout.
        '''
        if ( (t!=None) and (t.active()) ):
            t.cancel()
        return r

    def addTimeoutCallback(self, reactr, timeout,
                           callUponTimeout, *args, **kw):
        '''
        The function 'callUponTimeout' (with optional args or keywords)
        will be called after 'timeout' seconds, unless the Deferred fires.
        '''
        
        def timeoutCallback():
            self.cancel()
            callUponTimeout(*args, **kw)
        toc = reactr.callLater(timeout, timeoutCallback)
        return self.addCallback(self._finish, toc)
        

