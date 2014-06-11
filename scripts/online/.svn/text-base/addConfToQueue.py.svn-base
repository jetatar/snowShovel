import os,sys
import ROOT

from twisted.internet import defer

from writeConfigFromText import *
from AriStage import *

@defer.inlineCallbacks
def addConfToQueue(ctextfn, macadr, reactor, stop=True):

    # make the config dat file
    r = readConfTextAndWriteDat(ctextfn)
    if (r!=None):
        conf, runf, nets, odn, ciov = r
        ccp = ROOT.TSnCommConfPack(conf, runf, nets, ciov)
        
        # get the actor
        stage = AriStage(reactor)
        try :
            act = yield stage.getActorFor(macadr, STATUS_ACTOR)
            if (act!=None):
                act.AddComm(ccp)
                spa = act.FindAction("TSnCommActSendPeriodic")
                if (spa!=None):
                    spa.ResetReplacing()
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

@defer.inlineCallbacks
def addConfsToQueue(ctextfns, macadr, reactor, stop=True):
    '''
    ctextfns is a list of configs to add, in order
    '''
    for cfn in ctextfns:
        yield addConfToQueue(cfn, macadr, reactor, False)
    if (stop):
        print 'done'
        reactor.stop()
    
            
def main():
    if (len(sys.argv)<3):
        print 'usage: python addConfToQueue.py [config.txt] [stn mac adr]'
        sys.exit()

    from twisted.internet import reactor
    reactor.callWhenRunning(addConfToQueue, sys.argv[1], sys.argv[2], reactor)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
