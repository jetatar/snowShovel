import os,sys
import ROOT

from twisted.internet import defer

from writeConfigFromText import *
from AriStage import *

@defer.inlineCallbacks
def addPeriodicConf(ctextfn, start, per, dur, macadr, reactor):

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
                spa = act.FindAction("TSnCommActSendPeriodic")
                if (spa==None):
                    act.AddAfter("TSnCommActConfResend",
                                 "TSnCommActSendPeriodic")
                    spa = act.FindAction("TSnCommActSendPeriodic")
                spa.SetFirstTime(start)
                spa.SetPeriod(per)
                spa.SetDuration(dur)
                spa.SetCommObject(ccp)
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
    if (len(sys.argv)<6):
        print 'usage: python addPeriodicConf.py [config.txt] '\
            '[start unix time] [period] [duration] [stn mac adr]'
        sys.exit()

    from twisted.internet import reactor
    reactor.callWhenRunning(addPeriodicConf,
                            sys.argv[1], int(sys.argv[2]), int(sys.argv[3]),
                            int(sys.argv[4]), sys.argv[5],
                            reactor)
    print 'running'
    reactor.run()

if __name__=="__main__":
    main()
