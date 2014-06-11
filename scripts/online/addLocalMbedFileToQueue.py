import os,sys
import ROOT

from twisted.internet import defer

from writeConfigFromText import *
from AriStage import *

@defer.inlineCallbacks
def addLocalMbedFileToQueue(mlfn, macadr, reactor, stop=True):
    
    # get the mbed local file
    mlf = ROOT.TSnCommMbedLocalFile(mlfn)
    
    # get the actor
    stage = AriStage(reactor)
    try :
        act = yield stage.getActorFor(macadr, STATUS_ACTOR)
        if (act!=None):
            act.AddComm(mlf)
            act.Write(act.GetName(), ROOT.TObject.kOverwrite)
            act.Print()
            stage.doneWithActor()
            if (stop):
                print 'done'
                reactor.stop()
    except Exception as e:
        print 'exception {0}'.format(e)
        print 'unable to do it'
        reactor.stop()

def main():
    if (len(sys.argv)<3):
        print 'usage: python addLocalMbedFileToQueue.py '\
            '[filename] [stn mac adr]'
        sys.exit()

    from twisted.internet import reactor
    reactor.callWhenRunning(addLocalMbedFileToQueue,
                            sys.argv[1], sys.argv[2], reactor)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
