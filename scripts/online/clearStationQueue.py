import os,sys
import ROOT

from twisted.internet import defer

from writeConfigFromText import *
from AriStage import *

@defer.inlineCallbacks
def clearStationQueue(macadr, reactor, actn, stop=True):

    # get the actor
    stage = AriStage(reactor)
    try :
        act = yield stage.getActorFor(macadr, actn)
        if (act!=None):
            act.ClearAllComms()
            act.Write(act.GetName(), ROOT.TObject.kOverwrite)
            act.Print()
            stage.doneWithActor()
        else:
            printout(vtype.kError, "Could not get actor named [{0}]"
                     .format(actn))
        if (stop):
            print 'done'
            reactor.stop()
    except Exception as e:
        print 'exception {0}'.format(e)
        print 'unable to do it'
        reactor.stop()

def main():
    if (len(sys.argv)<2):
        print 'usage: python clearStationQueue.py [stn mac adr] ("status" or "file" actor. default: status)'
        sys.exit()
    
    actn = STATUS_ACTOR
    if (len(sys.argv)>2):
        if (sys.argv[2].lower() == "file"):
            actn = FILE_ACTOR

    from twisted.internet import reactor
    reactor.callWhenRunning(clearStationQueue, sys.argv[1], reactor, actn)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
