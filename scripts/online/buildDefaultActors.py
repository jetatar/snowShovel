import os,sys
import ROOT

import AriStage

def GetDefaultStatusActor(mac):
    # status:
    #  1) send the next thing in the queue, if there is anything
    #  2) check if rates are high and stn needs triggers off
    #  3) check if the station did not get the last config that was sent
    #  commented out X) send the heartbeat config if it's been long enough
    #  4) send no new config signal
    statusActor = ROOT.TSnCommActor(AriStage.STATUS_ACTOR, mac)
    statusActor.AddFirst("TSnCommActSendQueued")
    statusActor.AddLast("TSnCommActHiRate")
    statusActor.AddLast("TSnCommActConfResend")
    #statusActor.AddLast("TSnCommActSendPeriodic")
    statusActor.AddLast("TSnCommActSendNoConf")
    return statusActor

def GetDefaultFileActor(mac):
    # file:
    #  1) check if the station is demanding a handshake and send
    #     "no reply". NOTE: this should be replaced if an actual
    #     task is desired (deleting all data, requesting a run, etc)
    #  2) if the station has sent a file, send the appropriate reply
    #     based on the fraction of events and power readings that arrived
    fileActor = ROOT.TSnCommActor(AriStage.FILE_ACTOR, mac)
    fileActor.AddFirst("TSnCommFiActSendQueued")
    fileActor.AddLast("TSnCommFiActNoReplyHndshk")
    fileActor.AddLast("TSnCommFiActTransComplete")
    return fileActor

def GetDefaultActor(aname, mac):
    if (aname == AriStage.STATUS_ACTOR):
        return GetDefaultStatusActor(mac)
    elif (aname == AriStage.FILE_ACTOR):
        return GetDefaultFileActor(mac)
    else:
        return None

def main():
    if (len(sys.argv)<2):
        print "Usage: buildDefaultActors.py [mac address]"
    mac=sys.argv[1]

    afn = AriStage.GetActorFilename(mac)
    af = ROOT.TFile(afn,"update")
    af.cd()
    statusActor = GetDefaultStatusActor(mac)
    fileActor   = GetDefaultFileActor(mac)
    statusActor.Write(statusActor.GetName(), ROOT.TObject.kOverwrite)
    fileActor.Write(fileActor.GetName(), ROOT.TObject.kOverwrite)
    af.Close()

    print "Wrote file {0}".format(afn)

if __name__=="__main__":
    main()
