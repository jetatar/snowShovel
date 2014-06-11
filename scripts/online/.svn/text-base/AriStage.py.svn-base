from AriUtils import *
from AriFiles import *
from AriEmailSmtp import buildGenericMsgFile, buildGenericMsgFileWithData,\
                         sendEmail, smtphost
# the script importing this file must call AriUtils.loadOnlineLibs()
from twisted.internet import utils, threads, defer
from twisted.python import util

import buildDefaultActors

# constants
STAGE_DIR="/data/ONLINE/Stage"
STAGE_PRF="AriStage."
STATUS_ACTOR="AriStatusActor"
FILE_ACTOR="AriFileActor"
#ALERT_TO_EMAILS=["cjreed@uci.edu", "jtatar@uci.edu", "barwick@HEP.ps.uci.edu"]
ALERT_TO_EMAILS=["cjreed@uci.edu"]
ALERT_FROM_ADDRESS="ariannaproject@uci.edu"
SMTP_USER="ariannaproject"
SMTP_PASS=util.getPassword('Password for {0}@{1} (to send alerts/SBD msgs): '
                           .format(SMTP_USER,smtphost))


def GetActorFilename(macadr):
    # get the name of the file containing the actor for the station
    adir = "{0}/{1}{2}.root".format(STAGE_DIR,STAGE_PRF,macadr)
    checkDirOfFileExists(adir)
    printout(vtype.kDebug, "ActorFN for {0} is {1}".format(macadr,adir))
    return adir


class AriStage(object):

    def __init__(self, reactor):
        self.reactor = reactor
        # file storing the actors for this stage
        self.actfile = None
        # incoming status
        self.incStat = None
        # incoming file
        self.incFile = None
    
    #
    # "private" functions
    #

    def _buildIncomingFromHandshake(self, hndshk):
        # make a TSnCommIncFile object out of the handshake
        o = ROOT.TSnCommIncHandshake(hndshk)
        if (self.incStat!=None):
            o.SetIncStatFrom(self.incStat)
        else:
            printout(vtype.kError, "Got handshake {0:02x} "
                     "but no status update!".format(hndshk))
        return o

    @defer.inlineCallbacks
    def _getResponseFromActorFor(self, actor, incoming):
        # get the response from the actor, check if further action is
        # needed (see _handleResponder)
        #
        # returns a deferred that fires after GetResponseFor finishes
        # running (in another thread)
        # 
        # callbacks added on to this deferred will receive the response
        # from actor.GetResponseFor as the incoming result (maybe None)
        resp = None
        if (actor!=None):
            resp = yield threads.deferToThread(actor.GetResponseFor, incoming)
            if (resp!=None):
                ROOT.SetOwnership(resp, True)
            rsn = None
            action = actor.PullLastResponder()
            if (action!=None):
                rc = action.IsA()
                if (rc!=None):
                    rsn = rc.GetName()
            printout(vtype.kInfo, "Responder = {0}".format(rsn))
            self._handleResponder(resp, actor)
            # re-write actor so any changes are saved (update last action, etc)
            self.actfile.RF.cd()
            actor.Write(actor.GetName(), ROOT.TObject.kOverwrite)
        else:
            printout(vtype.kError,"Can't get response from null actor.")
        defer.returnValue(resp)

    def _handleResponder(self, r, actor):
        # check if any further action is needed, depending on the
        # type of response or the responder
        
        #action = actor.PullLastResponder()
        if isinstance(r, ROOT.TSnCommAlertPack):
            if (r.ShouldSendEmail()):
                self.SendAlert(r.GetSource(), r.GetSubject(), r.GetMessage())
            else:
                printout(vtype.kWarn, "ALERT from {0}.\r\n"
                         "Subject: {1}\r\n"
                         "Message: {2}".format(
                        r.GetSource(), r.GetSubject(), r.GetMessage()))

    @defer.inlineCallbacks
    def _deferredResponseTo(self, actname, incoming):
        # the status of the incoming obj must not be None.  (this
        # shouldn't be a problem, as the status signals the begging of
        # a comm win)
        #
        # returns a deferred that is fired after opening the actor
        # file, getting the actor object, sending it the incoming
        # info and getting the response.
        # the callback fires with the response that should be sent
        # to the station
        if (getVerbosity()>=vtype.kDebug):
            printout(vtype.kDebug, "_deferredResponseTo ({0})".format(actname))
            printout(vtype.kDebug, "incoming:")
            if (incoming!=None):
                incoming.Print()
            else:
                printout(vtype.kDebug, "<None>")
        if (incoming!=None):
            status = incoming.GetStatus()
            if (status!=None):
                mac = status.GetMacAdrAsStr()
                printout(vtype.kDebug, "mac={0}".format(mac))
                actor = yield self.getActorFor(mac, actname)
                if (getVerbosity()>=vtype.kDebug):
                    printout(vtype.kDebug, "Got actor:")
                    if (actor!=None):
                        actor.Print()
                    else:
                        printout(vtype.kDebug, "<None>")
                resp  = yield self._getResponseFromActorFor(actor, incoming)
                if (getVerbosity()>=vtype.kDebug):
                    printout(vtype.kDebug, "Got response:")
                    if (resp!=None):
                        resp.Print()
                    else:
                        printout(vtype.kDebug, "<None>")
                defer.returnValue(resp)
            else:
                printout(vtype.kError,"Could not get status.")
                defer.returnValue(None)
        else:
            printout(vtype.kError,"Response requested for [{0}], but "
                     "no incoming object provided!".format(actname))
    
    #
    # "public" functions
    #
    
    @defer.inlineCallbacks
    def getActorFor(self, macadr, actorname, makeDefIfNone=True):
        # return a deferred that fires with the actor as its result
        printout(vtype.kDebug,
                 "getActorFor {0}, {1}".format(macadr, actorname))
        afn = GetActorFilename(macadr)
        if ( (self.actfile==None) or (self.actfile.fname!=afn) ):
            printout(vtype.kInfo, "Opening actor file {1}"
                     .format(macadr, afn))
            self.actfile = AriROOTFile(afn, "UPDATE")
        rf = yield self.actfile.DeferUntilOpen()
        if (rf!=None):
            a = rf.Get(actorname)
            if (a==None):
                printout(vtype.kError,
                         "Could not get actor object named [{0}] from [{1}]"
                         .format(actorname, afn))
                if (makeDefIfNone):
                    printout(vtype.kWarn,"Making default [{0}] for mac [{1}]."
                             .format(actorname, macadr))
                    a = buildDefaultActors.GetDefaultActor(actorname, macadr)
            if (a!=None):
                ROOT.SetOwnership(a, True)
            defer.returnValue(a)
        else:
            printout(vtype.kError,"No root file in AriROOTFile [{0}]."
                     .format(afn))
        defer.returnValue(None)

    def doneWithActor(self, ignoredResult=None):
        # close the file
        #
        # ignoredResult allows this fcn to be a callback
        if (self.actfile!=None):
            self.actfile.Close()


    def IncomingStatus(self, status, stvers):
        printout(vtype.kDebug, "IncomingStatus")
        s = getTimeStampFrom(datetime.datetime.utcnow())
        self.incStat = ROOT.TSnCommIncStatus(s, status, stvers)
        
    def IncomingStatusPowerReading(self, power, pvers):
        printout(vtype.kDebug, "IncomingStatusPowerReading")
        if (self.incStat!=None):
            self.incStat.SetPower(power)
            self.incStat.SetPowerIOversion(pvers);
        else:
            printout(vtype.kError, "Got inc power reading but no status update!")

    def IncomingFilename(self, filename):
        printout(vtype.kInfo, "IncomingFilename [{0}]".format(filename))
        # a new file; lose the old one
        self.incFile = ROOT.TSnCommIncFile(filename)
        if (self.incStat!=None): # how could it be?
            self.incFile.SetIncStatFrom(self.incStat)
            if (getVerbosity()>=vtype.kDebug):
                printout(vtype.kDebug, "Set incoming status, timee")
                self.incFile.Print()
        elif (getVerbosity()>=vtype.kDebug):
            printout(vtype.kDebug,"No incoming status!")
        else:
            printout(vtype.kError, "Got inc filename but no status update!")

    def IncomingFileVersion(self, fversion):
        printout(vtype.kDebug, "IncomingFileVersion")
        if (self.incFile!=None):
            self.incFile.SetFileVersion(fversion)
        else:
            printout(vtype.kError, "Got inc file version but no inc file obj!")
            
            
    def IncomingFileEvent(self):
        printout(vtype.kDebug, "IncomingFileEvent")
        if (self.incFile!=None):
            self.incFile.IncrementNevtsRcvd()
        else:
            printout(vtype.kError, "Got inc file event but no inc file obj!")

    def IncomingFilePowerReading(self):
        printout(vtype.kDebug, "IncomingFilePowerReading")
        if (self.incFile!=None):
            self.incFile.IncrementNpwrsRcvd()
        else:
            printout(vtype.kError, "Got inc pwr reading but no inc file obj!")

    def IncomingFileChecksum(self, n):
        printout(vtype.kDebug, "IncomingFileChecksum")
        if (self.incFile!=None):
            self.incFile.SetMbedCRC(n)
            self.incFile.PrintMbedCRC()
        else:
            printout(vtype.kError, "[{0}] file CRC received, but "
                     "no incoming file info pack!".format(n))

    def FileReceivedChecksum(self, n):
        printout(vtype.kDebug, "FileReceivedChecksum")
        if (self.incFile!=None):
            self.incFile.SetLocalCRC(n)
            self.incFile.PrintLocalCRC()
        else:
            printout(vtype.kError, "[{0}] file CRC received, but "
                     "no incoming file info pack!".format(n))
        
    def IncomingFileEventsSent(self, n):
        printout(vtype.kDebug, "IncomingFileEventsSent")
        if (self.incFile!=None):
            self.incFile.SetNevtsSent(n)
            self.incFile.PrintEvts()
        else:
            printout(vtype.kError, "[{0}] evts sent signal received, but "
                     "no incoming file info pack!".format(n))

    def IncomingFilePowerReadingsSent(self, n):
        printout(vtype.kDebug, "IncomingFilePowerReadingsSent")
        if (self.incFile!=None):
            self.incFile.SetNpwrsSent(n)
            self.incFile.PrintPwrs()
        else:
            printout(vtype.kError, "[{0}] pwrs sent signal received, but "
                     "no incoming file info pack!".format(n))

    def GetNevtsRcvd(self):
        if (self.incFile!=None):
            return self.incFile.GetNevtsRcvd()
        else:
            printout(vtype.kError, "Request for num evts received, but "
                     "no incoming file info pack!")
            return 0

    @defer.inlineCallbacks
    def DeferredResponseToStatus(self):
        # return a deferred that fires with the response from the actor
        #
        # incStat and status must not be None.
        # (this shouldn't be a problem, as they signal the begging
        # of a comm win)
        printout(vtype.kDebug, "DeferredResponseToStatus")
        resp = yield self._deferredResponseTo(STATUS_ACTOR, self.incStat)
        self.doneWithActor()
        # don't clear the status here, as it may be needed for incoming data
        # clear it only upon receipt of the next status
        defer.returnValue(resp)

    @defer.inlineCallbacks
    def DeferredResponseToFile(self):
        # return a deferred that fires with the response from the actor
        #
        # incStat and status must not be None.
        # (this shouldn't be a problem, as they signal the begging
        # of a comm win)
        printout(vtype.kDebug, "DeferredResponseToFile")
        resp = yield self._deferredResponseTo(FILE_ACTOR, self.incFile)
        self.doneWithActor()
        # should be safe to clear the file after responding to it
        self.incFile = None
        defer.returnValue(resp)

    @defer.inlineCallbacks
    def DeferredResponseToHandshake(self, hndshk):
        # return a deferred that fires with the response from the actor
        #
        # incStat and status must not be None.
        # (this shouldn't be a problem, as they signal the begging
        # of a comm win)
        printout(vtype.kDebug, "DeferredResponseToHandshake")
        incobj = self._buildIncomingFromHandshake(hndshk)
        resp = yield self._deferredResponseTo(FILE_ACTOR, incobj)
        self.doneWithActor()
        defer.returnValue(resp)

    def SendAlert(self, source, subject, message,
                  binaryData=None, atchName=None):
        # use ssmtp to send an email to ALERT_TO_EMAILS
        # with the specified subject and message

        printout(vtype.kDebug, "SendAlert")

        msg = "Alert from {0}:\r\n\r\n{1}".format(source, message)

        printout(vtype.kDebug, msg)

        emailmsg = None
        if (binaryData==None):
            # text email
            emailmsg, toAdr = buildGenericMsgFile(ALERT_TO_EMAILS,
                                                  ALERT_FROM_ADDRESS,
                                                  str(subject), str(msg))
        else:
            # email with binary attachment
            if (atchName==None):
                atchName = "data.bin"
            emailmsg, toAdr = buildGenericMsgFileWithData(ALERT_TO_EMAILS,
                                                          ALERT_FROM_ADDRESS,
                                                          str(subject),
                                                          str(msg),
                                                          binaryData,
                                                          atchName)
        
        sendEmail(self.reactor,
                  SMTP_USER, SMTP_PASS,
                  ALERT_FROM_ADDRESS,
                  emailmsg,
                  ALERT_TO_EMAILS)

        

