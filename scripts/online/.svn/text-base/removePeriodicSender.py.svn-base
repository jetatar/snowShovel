import os,sys
import ROOT

from twisted.internet import defer

from writeConfigFromText import *
from AriStage import *

@defer.inlineCallbacks
def addPeriodicConf(macadr, reactor):

    # get the actor
    stage = AriStage(reactor)
    try :
        act = yield stage.getActorFor(macadr, STATUS_ACTOR)
        if (act!=None):
            spa = act.FindAction("TSnCommActSendPeriodic")
            if (spa!=None):
                act.Remove("TSnCommActSendPeriodic")
                # save changes
                act.Write(act.GetName(), ROOT.TObject.kOverwrite)
                act.Print()
                stage.doneWithActor()
                print 'done'
                reactor.stop()
    except Exception as e:
        print 'exception {0}'.format(e)
        print 'unable to do it'
        reactor.stop()
            
def main():
    if (len(sys.argv)<2):
        print 'usage: python removePeriodicSender.py [stn mac adr] '
        sys.exit()

    from twisted.internet import reactor
    reactor.callWhenRunning(addPeriodicConf,
                            sys.argv[1],
                            reactor)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
