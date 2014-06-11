import os,sys
import ROOT

from twisted.internet import defer

from AriStage import *

@defer.inlineCallbacks
def printStationActors(macadr, actors, reactor,
                       popts="queue action label",
                       stop=True):

     # get the stage
    stage = AriStage(reactor)
    for actnm in actors:
        try :
            act = yield stage.getActorFor(macadr, actnm, False)
            if (act!=None):
                print '\n\n\n'
                act.Print(popts)
                stage.doneWithActor()
            else:
                print "Could not get actor named [{0} for [{1}]"\
                    .format(actnm, macadr)
        except Exception as e:
            print 'exception {0}'.format(e)
            print 'unable to do it'
    if (stop):
        print 'done'
        reactor.stop()

def main():
    if (len(sys.argv)<3):
        print 'usage: python printStationActors.py [station MAC address] '\
            '[status / file / both] (print options)'
        sys.exit()
    
    actnms = []
    if (sys.argv[2]=="file" or sys.argv[2]=="both"):
        actnms.append(FILE_ACTOR)
    if (sys.argv[2]=="status" or sys.argv[2]=="both"):
        actnms.append(STATUS_ACTOR)
    if (len(actnms)==0):
        print 'unknown option [{0}]. expect "status", "file" or "both".'\
            .format(sys.argv[2])
    else:
        popts="queue action label"
        if (len(sys.argv)>3):
            popts = sys.argv[3]
        from twisted.internet import reactor
        reactor.callWhenRunning(printStationActors, sys.argv[1], actnms, reactor,
                                popts)
        reactor.run()

if __name__=="__main__":
    main()
