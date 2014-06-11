from addConfToQueue import *
from twisted.internet import task

def startloopy(reactor):
    ctextfns = ['sbdTestSVnosend.txt','sbdTestSendVolts.txt','sbdTest.txt']

    loopy = task.LoopingCall(addConfsToQueue, ctextfns, '0002F7F175B7', reactor,
                             False)
    loopy.start(3600)

def main():

    from twisted.internet import reactor
    reactor.callWhenRunning(startloopy, reactor)
    reactor.run()
    

if __name__=="__main__":
    main()

