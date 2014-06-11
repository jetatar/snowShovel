import os,sys
import ROOT

from twisted.internet import defer

from AriStage import *

@defer.inlineCallbacks
def addRunRequestToQueue(run, macadr, reactor, stop=True):

    try:
        r = int(run)
        p = ROOT.TSnCommProtocolMsg(ROOT.TSnIOHeaderFrame.kHnShOkReqRnCode, r)
        o = ROOT.TSnCommAlertPack.NewAlertPackPassComm(p,
                                                       "addRunRequestToQueue",
                                                       "Requesting run [{0}]."
                                                       .format(r),
                                                       "",
                                                       ROOT.TSnCommAlertPack
                                                       .kLogInfo)
        ROOT.SetOwnership(o, True)

        # get the actor
        stage = AriStage(reactor)
        try :
            act = yield stage.getActorFor(macadr, FILE_ACTOR)
            if (act!=None):
                act.AddComm(o)
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
        
    except Exception as e:
        printout(vtype.kError,"FAIL<{0}>: {1}".format("addRunRequestToQueue",e))
        reactor.stop()

@defer.inlineCallbacks
def addRunRequestsToQueue(runlist, macadr, reactor, stop=True):
    '''
    runlist is a string with a comma separated list of runs to request, in order
    '''
    rl = runlist.split(',')
    for run in rl:
        yield addRunRequestToQueue(run, macadr, reactor, False)
    if (stop):
        print 'done'
        reactor.stop()
    
            
def main():
    if (len(sys.argv)<3):
        print 'usage: python addRunRequestToQueue.py [comma separated run list with no spaces, like 34,56,77] [stn mac adr]'
        sys.exit()

    from twisted.internet import reactor
    reactor.callWhenRunning(addRunRequestsToQueue, sys.argv[1], sys.argv[2],
                            reactor)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
