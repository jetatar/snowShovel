import base64
from twisted.mail import imap4
from twisted.internet import task
from email import utils as EmailUtils

from AriEmailSmtp import *
from AriStationProtocol import *

# change the verbosity
#setVerbosity(vtype.kDebugData)
setVerbosity(vtype.kDebug)
#setVerbosity(vtype.kInfo)

# incoming SBD email account properties
mailhost = "imap.gmail.com"
mailport = 993
mailuser = "arianna.icicle"
mailfrom = "arianna.icicle@gmail.com"
#mailuser = "arianna.icebox"
#mailfrom = "arianna.icebox@gmail.com"
mailpass = util.getPassword('Password for {0}@{1} (to check SBD messages): '
                            .format(mailuser,mailhost))

# how often to check the email account for unread messages
mailCheckPeriod = 30 # seconds. if 0, don't repeat; just check once
# how often to log back into the email account
reconnectPeriod = 1800 # seconds. if 0, don't repeat; just once
                       #          if < 0, never intentionally logout
# max time between reconnect attempts
maxReconnDelay  = 1800 # seconds.

# whether to disable actually sending emails (even emergency thermals off)
disableOutgoingEmails=False
# whether to send no config codes or not
disableNoConfigCodeEmail=False

# where to store the raw emails
rawoutdir = "SBDmail"

# email for commerical SBD modems
datemail = "data@sbd.iridium.com"
# email for military SBD modems
milemail = "data@sbd.pac.disa.mil"
# list of IMEI's of commerical SBD modems
commercialSbds = [300234011935770]

# constants
LINEBREAK = '\r\n'
SBDMAXLEN = 270 - ROOT.TSnIOHeaderFrame.kMaxSizeOf

# store the AriStationProtocols for each SBD modem globally,
# since messages may arrive slowly, between re-connections to gmail
sbdProtocols = {}
sbdProtFactory = None

#
# SBD Utility Functions
#

def getToEmail(sbdImei):
    if (int(sbdImei) in commercialSbds):
        return datemail
    else:
        return milemail

def buildMsgFile(fromAdr, sbdImei, binaryData, 
                 atmTag="sbddata"):
    # build the file-object for a single email
    # with a binary attachment

    toAdr = getToEmail(sbdImei)
    afn = '{0}.sbd'.format(atmTag)

    return buildGenericMsgFileWithData(toAdr, fromAdr,
                                       '{0}'.format(sbdImei),
                                       None,
                                       binaryData,
                                       afn)

def parseHeaderForOutImei(msghdr):
    headers = msghdr.split(LINEBREAK)
    sbdimei = None
    for h in headers:
        if h.startswith("Subject"):
            # get the SBD IMEI number
            if "SBD Msg From Unit:" in h:
                sbdimei = h.split("SBD Msg From Unit:")[-1].strip()
            break
    return sbdimei

def parseHeaderForImeiAndDate(msghdr):
    headers = msghdr.split(LINEBREAK)
    sbdimei = parseHeaderForOutImei(msghdr)
    sbddate = None
    for h in headers:
        if h.startswith("Date"):
            try:
                dt = EmailUtils.parsedate_tz(h.split("Date:")[-1].strip())
                # try to get it in UTC
                if (dt[9]!=None):
                    # python2.x's date/time classes are too difficult
                    x = ROOT.TTimeStamp( dt[0],dt[1],dt[2],
                                         dt[3],dt[4],dt[5],
                                         0, False, -dt[9] )
                    sbddate = datetime.datetime.utcfromtimestamp(x.GetSec())
                else:
                    # no timezone info
                    sbddate = datetime.datetime( dt[0], dt[1], dt[2],
                                                 dt[3], dt[4], dt[5] )
            except:
                pass
        if (sbdimei==None):
            if h.startswith("Subject"):
                # get the SBD IMEI number
                if "Queued for Unit:" in h:
                    sbdimei = h.split("Queued for Unit:")[-1].strip()
        if (sbddate!=None and sbdimei!=None):
            break
    return sbdimei, sbddate

def parseBodyForDateMomsn(msgbody):
    bodies = msgbody.split(LINEBREAK)
    mdate = None
    momsn = None
    for b in bodies:
        if b.startswith("MOMSN"):
            try:
                momsn = int(b.split(":")[-1].strip())
            except:
                pass
        elif b.startswith("Time of Session"):
            try:
                mdate = datetime.datetime.strptime(\
                    b.split("): ")[-1].strip(),"%a %b %d %H:%M:%S %Y")
            except:
                pass
        if ( (mdate!=None) and (momsn!=None) ):
            break
    return mdate, momsn

def parseForOutImeiDateMOMSN(hdbd):
    sbddate = None
    sbdmomn = None
    sbdimei = parseHeaderForOutImei(hdbd[0])
    if (sbdimei):
        # then it's an SBD-outgoing message
        sbddate, sbdmomn = parseBodyForDateMomsn(hdbd[1])
    return sbdimei, sbddate, sbdmomn

@defer.inlineCallbacks
def writeSbdMail(msghdr, msgbdy):
    printout(vtype.kDebug, 'writeSbdMail')
    sbdimei, sbddate = parseHeaderForImeiAndDate(msghdr)
    if ((sbdimei!=None) and (sbddate!=None)):
        sbdfile = AriSBDEmailFile(rawoutdir, sbdimei, sbddate)
        sbdfile.storeForWriting(msghdr)
        sbdfile.storeForWriting(msgbdy)
        # doesn't send an email with the data in case this
        # write to disk fails (as is done in AriStationProtocol)
        # because this information is already stored in gmail
        yield sbdfile.writeBufWhenAble()


def readSbdMsg(msg):
    '''
    process the sbd message, to get sbd num of total and the payload.
    'msg' must be a ROOT.TSnIOBuffer, not a string.
    '''
    printout(vtype.kDebug, 'readSbdMsg')
    if (getVerbosity()>=vtype.kDebugData):
        msg.Print()
    # process the SBD header first
    sbdcode = ROOT.Long()
    sbdlen  = ROOT.Long()
    hdbyt = ROOT.TSnIOHeaderFrame.PyReadFrom(msg,
                                             sbdcode,
                                             sbdlen)
    msgnum = None
    msgtot = None
    if ( (hdbyt==HEADER_LEN) and 
         (sbdcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kSbdMsgCode)) ):
        msgnum = int(ROOT.TSnIOHeaderFrame.GetSbdMsgNumFromHdLen(sbdlen))
        msgtot = int(ROOT.TSnIOHeaderFrame.GetSbdMsgTotFromHdLen(sbdlen))
    printout(vtype.kDebug, 'sbd msg {0}/{1}'.format(msgnum, msgtot))
    return msgnum, msgtot, msg

@defer.inlineCallbacks
def recursiveSendChunk(reactor, imei, chunks, num=0, tot=None):
    '''
    given an array of chunks, the current chunk number and the total
    number of chunks to send...
    prepend each chunk with the SBD header and then email it.
    do it recursively so we're sure the messages go out in order.
    
    num counts from 0 to tot, even though the SBD msg number counts from 1.
    
    tot is only passed so that we don't have to compute len(chunks) every time.
    
    NO CHECK is performed here to verify that each chunk is small enough
    to be an acceptable SBD message. this should be done already.
    '''
    printout(vtype.kDebug, "recursiveSendChunk")
    
    if (tot==None):
        tot = len(chunks)
    
    printout(vtype.kDebug, "num={0}, tot={1}".format(num,tot))
    if (num<tot):
        chk = chunks[num]
        num += 1 # msg num starts from 1 not 0
        bmsg = ROOT.TSnIOBuffer()
        ROOT.TSnIOHeaderFrame.WriteToW(bmsg,
                                       ROOT.TSnIOHeaderFrame.kSbdMsgCode,
                                       ROOT.TSnIOHeaderFrame.
                                          GetSbdHdLenFrom(num, tot) )
        # prepend the chk with the sbd header
        bmsg.AppendQuiet(chk)
        bmsg.SetPosVal(0) # the WriteToW will have incremented the buf pos

        if (getVerbosity()>=vtype.kDebugData):
            printout(vtype.kDebugData,"binary msg for email:")
            bmsg.Print()

        # send it
        if (disableOutgoingEmails):
            printout(vtype.kInfo, "Outgoing email to {0} disabled."
                     .format(imei))
        else:
            msgfile, toAdr = buildMsgFile(mailfrom, imei, getDataFromBuf(bmsg))
            try:
                yield sendEmail(reactor,
                                SMTP_USER, SMTP_PASS,
                                mailfrom,
                                msgfile, toAdr)
                try:
                    yield recursiveSendChunk(reactor, imei, chunks, num, tot)
                except Exception as e:
                    uponFail(e, 'recursiveSendChunk, {0} to {1} / {2}'\
                                 .format(num,num+1,tot))
            except Exception as e:
                uponFail(e, 'recursiveSendChunk,sendEmail')

def getOrMakeAriSBDStnProtocol(imei, reactr):
    global sbdProtocols
    global sbdProtFactory
    if imei in sbdProtocols:
        sbdprot = sbdProtocols[imei]
    else:
        printout(vtype.kDebug, 
                 'making new AriSBDStationProtocol for {0}'.format(imei))
        if (sbdProtFactory==None):
            printout(vtype.kDebug, "Making new AriSBDStnProtFactory")
            sbdProtFactory = AriSBDStnProtFactory()
        sbdprot = sbdProtFactory.buildProtocol(imei)
        sbdProtocols[imei] = sbdprot
        sbdprot.connectionMade()
    

#
# SBD classes
#

class AriSBDStationProtocol(AriStationProtocol):
    '''
    class to patch SBD chunks together and send the data
    to the normal AriStationProtocol. any resulting outgoing
    messages are then emailed to the SBD modem.
    '''
    
    def __init__(self, imei):
        AriStationProtocol.__init__(self)
        self.prevTot      = None
        self.prevNum      = None
        self.imei         = imei
        self.stEvtTimeout = min(300,mailCheckPeriod*10) # sbd can take a while

    def handleSbdBinIn(self, binmsg):
        '''
        handle the incoming binary (base64 decoded) sbd message.
        check that the messages are in order, and for each completed
        bunch, send it to the normal AriStationProtocol.dataReceived
        '''
        printout(vtype.kDebug,'handleSbdBinIn')
        
        mnum, mtot, bmsg = readSbdMsg(ROOT.TSnIOBuffer(binmsg))
        
        printout(vtype.kDebug,'handleSbdBinIn: prevTot={0}, mtot={1}. '\
                     'prevNum={2}, mnum={3}'.format(\
                self.prevTot, mtot, self.prevNum, mnum))
        if ( (mnum!=None) and (mtot!=None) ):
            oktogo     = True
            resetCount = False
            if ( self.prevTot!=None ):
                # try to check that we're still on the same msg
                if ( (self.prevTot!=mtot) or (self.prevNum!=(mnum-1)) ):
                    # lost a chunk!
                    printout(vtype.kError,'!!! Lost an SBD msg chunk.')
                    printout(vtype.kError,'prevTot={0}, mtot={1}. '\
                                 'prevNum={2}, mnum={3}'.format(\
                            self.prevTot, mtot, self.prevNum, mnum))
                    oktogo = False
            elif (mnum!=1):
                # picked up a message in the middle
                printout(vtype.kError,"!!! Missed first of SBD msg packet.")
                printout(vtype.kError,'prevTot={0}, mtot={1}. '\
                             'prevNum={2}, mnum={3}'.format(\
                            self.prevTot, mtot, self.prevNum, mnum))
                oktogo = False

            if (oktogo):
                # this is the next message we were expecting (or a new one)
                # pass it on to the protocol
                printout(vtype.kDebug, "send message to dataReceived")
                self.dataReceived(getDataFromBuf(bmsg))
                #self.dataReceived(bmsg)
                resetCount = (mnum==mtot) # if true, we're done
            else:
                # clear the station protocol buffer, as we are giving up
                # until a new SBD message bunch begins.
                self.resetBuffer()
                # reset counters to start waiting for a new bunch
                resetCount = True

            # set the msg num counters
            if (resetCount):
                self.prevTot = None
                self.prevNum = None
            else:
                self.prevTot = mtot
                self.prevNum = mnum


    def sendIObuf(self, b):
        '''
        Overrides AriStationProtocol.sendIObuf in order to send the
        message in email(s) to the station.
        '''
        printout(vtype.kDebug, "AriSBDStationProtocol.sendIObuf")
        if (getVerbosity()>=vtype.kDebugData):
            b.Print()
        
        mcode = ROOT.Long(ROOT.TSnIOHeaderFrame.PyPeekCodeFrom(b))
        isNoConfig = (mcode == 
                      castHeaderCode(ROOT.TSnIOHeaderFrame.kNoConfigCode))
        if ( (isNoConfig==False) or
             ((disableNoConfigCodeEmail==False) and (isNoConfig)) ):
            if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kStringCode)):
                # just print the string
                printout(vtype.kUrgent,
                         "SendString: {0}".format(''.join(b.AsVector())))
            else:
                nb = b.GetBytesRemaining()
                printout(vtype.kDebug, 
                         "Send Message ({0} bytes):".format(nb))
                printHex(vtype.kDebugData, b)
                # TODO: we could filter the outgoing msgs and not send
                # redundant responses, in case we are reading many old
                # emails...  But if we send one communication per
                # email, the station will only use the last few
                # messages in the queue (according to how many bytes
                # it's expecting to get)
                self.mailAllToSBD(b)
        else:
            printout(vtype.kDebug, "No config code not appended to outBuff.")
        
    @defer.inlineCallbacks
    def mailAllToSBD(self, msgbuf):
        '''
        break the msg buffer up into chunks if necessary and send
        all the data in emails to the SBD modem
        
        msgbuf must be a ROOT.TSnIOBuffer
        '''
        
        printout(vtype.kDebug, "mailAllToSBD")
        if (getVerbosity()>=vtype.kDebugData):
            msgbuf.Print()

        # break up the data so it will fit in an SBD message
        def recurChunkify(bdata, chunks, clen):
            if (len(bdata)>0):
                chunks.append(bdata[:clen])
                recurChunkify(bdata[clen:], chunks, clen)

        chunks=[]
        recurChunkify(getDataFromBuf(msgbuf), chunks, SBDMAXLEN)
        
        printout(vtype.kDebug, "made {0} chunks".format(len(chunks)))
        if (getVerbosity()>=vtype.kDebugData):
            for x in chunks:
                printHex(vtype.kDebugData, bytearray(x))

        # send each chunk to the SBD modem
        try:
            yield recursiveSendChunk(self.factory.reactor, self.imei, chunks)
        except Exception as e:
            uponFail(e, 'mailAllToSBD, recursiveSendChunk')
        

class AriSBDStnProtFactory(protocol.Factory):
    def __init__(self, reactr):
        self.reactor = reactr
        self.protocol = AriSBDStationProtocol

    def buildProtocol(self, imei):
        p = self.protocol(imei)
        p.factory = self
        return p

class AriSBDGmailImap4Client(imap4.IMAP4Client):
    '''
    client to fetch and process SBD emails from gmail. the messages
    contained in the emails are sent to the AriSBDStationProtocol for
    this sbd modem.
    '''
    
    def __init__(self, contextFactory=None):
        imap4.IMAP4Client.__init__(self, contextFactory)
        self.connectedAt        = None
        self.loopy              = None
    
    def connectionMade(self):
        printout(vtype.kDebug, "connectionMade")
        self.connectedAt = self.factory.reactor.seconds()
        
    @defer.inlineCallbacks
    def serverGreeting(self, caps):
        printout(vtype.kDebug, "serverGreeting")
        # log in
        try:
            yield self.login(mailuser, mailpass)
            try:
                yield self.repeatInboxSel()
            except Exception as e:
                uponFail(e, "repeatInboxSel")
        except Exception as e:
            uponFail(e, "logging in")
        
        # done. log out
        try:
            yield self.logout()
        except Exception as e:
            uponFail(e, "logging out")
        
    @defer.inlineCallbacks
    def repeatInboxSel(self):
        printout(vtype.kDebug, "repeatInboxSel")
        if (mailCheckPeriod==0):
            # connect just this once
            yield self.uponAuthentication()
        else:
            self.loopy = task.LoopingCall(self.uponNextAuth)
            yield self.loopy.start(mailCheckPeriod)

    @defer.inlineCallbacks
    def uponNextAuth(self):
        if ( (reconnectPeriod>0) and
             ((self.factory.reactor.seconds() - self.connectedAt) 
              > reconnectPeriod) ):
            # time to re-connect
            printout(vtype.kInfo, "reconnecting")
            if (self.loopy!=None):
                # this will fire the deferred that loopy.start returned,
                # causing repeatInboxSel to finish, leading to a logout.
                # the reconnecting factory will then start it all over.
                self.loopy.stop()
        else:
            # check the inbox again
            yield self.uponAuthentication()

    @defer.inlineCallbacks
    def uponAuthentication(self):
        printout(vtype.kDebug, 'uponAuthentication')
        try:
            yield self.select('Inbox')
            try:
                msgids = yield self.search("(UNSEEN)")
                try:
                    yield self.fetchUnread(msgids)
                except Exception as e:
                    uponFail(e, "fetchUnread")
            except Exception as e:
                uponFail(e, "searching unread")
        except Exception as e:
            uponFail(e, "selecting inbox")

    @defer.inlineCallbacks
    def fetchUnread(self, msgids):
        printout(vtype.kDebug, 'fetchUnread')
        nmsgs = len(msgids)
        printout(vtype.kInfo, "{0} unread messages".format(nmsgs))
        if (nmsgs>0):
            msgset = imap4.parseIdList(str(msgids).strip('[').strip(']'))
            whenHdrsFetched = self.fetchHeaders(msgset)
            whenBdysFetched = self.fetchBody(msgset)
            try:
                hdrsBdys = yield defer.DeferredList([whenHdrsFetched,
                                                     whenBdysFetched])
                # write the sbd emails
                gotHeadrs = (hdrsBdys[0][0] == True)
                gotBodies = (hdrsBdys[1][0] == True)
                if (gotHeadrs):
                    dl = []
                    msgs = {}
                    for msgnum, msghdrdict in hdrsBdys[0][1].iteritems():
                        msghdr = msghdrdict['RFC822.HEADER']
                        msgbdy = ''
                        if (gotBodies):
                            msgbdy = hdrsBdys[1][1][msgnum]['RFC822.TEXT']
                        whenWritten = writeSbdMail(msghdr, msgbdy)
                        dl.append(whenWritten)
                        msgs[msgnum] = (msghdr, msgbdy)
                    try:
                        # wait for them all to be written
                        yield defer.DeferredList(dl)
                        try:
                            # process the emails from each imei
                            yield self.processEmails(msgs)
                        except Exception as e:
                            uponFail(e, "processing emails")
                    except Exception as e:
                        uponFail(e, "writing sbd emails")
            except Exception as e:
                uponFail(e, "fetching headers and bodies")

    @defer.inlineCallbacks
    def processEmails(self, msgs):
        printout(vtype.kDebug, 'processEmails')
        # separate by imei (a bundle of msgs for each station)
        # then will order by date
        allbdys = {}
        for hdbd in msgs.itervalues():
            sbdimei, sbddate, momsn = parseForOutImeiDateMOMSN(hdbd)
            if (sbdimei!=None):
                if sbdimei not in allbdys:
                    allbdys[sbdimei] = []
                allbdys[sbdimei].append( (momsn, sbddate, hdbd[1]) )
        # sort all the messages by momsn, date, body on each imei
        dl = []
        for imei,datsbdys in allbdys.iteritems():
            # the order in which the imei's are handled is not important
            d = threads.deferToThread( sorted, datsbdys )
            d.addCallback(self.uponBdysSorted, imei)
            d.addErrback(uponFail, 'uponBdysSorted')
            dl.append(d)
        try:
            yield defer.DeferredList(dl)
        except Exception as e:
            uponFail(e, "sorting emails")
        
    def uponBdysSorted(self, allbdys, imei):
        '''
        make a (binary) protocol for this SBD modem and
        process all the incoming emails from this SBD
        '''
        printout(vtype.kDebug, 'uponBdysSorted')
        sbdprot = None
        global sbdProtocols
        if imei in sbdProtocols:
            sbdprot = sbdProtocols[imei]
        else:
            printout(vtype.kDebug, 
                     'making new AriSBDStationProtocol for {0}'.format(imei))
            
            sbdprot = AriSBDStationProtocol(imei)
            sbdprot.factory = self.factory
            sbdprot.connectionMade()
            sbdProtocols[imei] = sbdprot
        # loop over the messages from this SBD
        for momsn, date, body in allbdys:
            bdys = body.split(LINEBREAK)
            bytez = 0
            for b in bdys:
                if (b.startswith("Message Size")):
                    bytez = int(b.split(":")[1].strip())
                    break
            if (bytez>0):
                # got an actual SBD message from the station
                printout(vtype.kInfo,
                         "Process msg MOMSN {0} from {1} at {2}".format(
                        momsn, imei, date))
                printout(vtype.kDebug, "SBD({0}) has msg w/ {1} bytes".format(
                        imei, bytez))

                b64msg = body[body.rfind("base64")+len("base64")\
                                  :body.rfind("--SBD.Boundary")]\
                                  .replace("\r","")\
                                  .replace("\n","")
                binmsg = base64.b64decode(b64msg)

                sbdprot.handleSbdBinIn(binmsg)


class AriGmailImap4ClientFactory(protocol.ReconnectingClientFactory):
    '''
    A factory to make AriSBDGmailImap4Client protocols, and to
    always re-login to gmail to check SBD messages.
    '''
    
    protocol = AriSBDGmailImap4Client
    
    def __init__(self, context, reactr):
        self.ctx     = context
        self.reactor = reactr
        self.maxDelay = maxReconnDelay

    def buildProtocol(self, addr):
        # connected
        self.resetDelay() # so we can reconnect quickly next time
        p = self.protocol(self.ctx)
        p.factory = self
        return p

def startupMsg():
    printout(vtype.kInfo, 
             "Checking {0} at {1}:{2}".format(mailuser,mailhost,mailport))
        
def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)
    
    from twisted.internet import reactor

    ccf     = ssl.ClientContextFactory()
    factory = AriGmailImap4ClientFactory(ccf, reactor)
    
    reactor.connectSSL(mailhost, mailport, factory, ccf)
    reactor.callWhenRunning(startupMsg)
    reactor.run()

if __name__=="__main__":
    main()
