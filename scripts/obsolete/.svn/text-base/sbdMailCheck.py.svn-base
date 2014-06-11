import os,sys
import base64
from email import utils as EmailUtils
from twisted.internet import protocol
from twisted.internet import ssl
from twisted.internet import defer
from twisted.internet import threads
from twisted.internet import task
from twisted.mail import imap4
from twisted.python import util
from twisted.python import filepath

# bring in our stuff
from AriGmailSmtp import *
from twistedIO import *

# change the verbosity
setVerbosity(vtype.kDebugData)
#setVerbosity(vtype.kInfo)


# parameters
mailhost = "imap.gmail.com"
mailport = 993
mailuser = "arianna.icicle"
mailfrom = "arianna.icicle@gmail.com"
mailpass = util.getPassword('Password for {0}@{1}: '
                            .format(mailuser,mailhost))
smtpuser = "ariannaproject"
#smtpuser = "arianna.icicle"
smtppass = util.getPassword('Password for {0}@{1}: '
                            .format(smtpuser,smtphost))
mailCheckPeriod = 30 # seconds. if 0, don't repeat; just check once
reconnectPeriod = -1 # seconds. if 0, don't repeat; just once
                     #          if < 0, never intentionally logout
maxReconnDelay  = 1800 # seconds. max time between reconnect attempts
rawoutdir = "SBDmail"

# constants
LINEBREAK = '\r\n'
SBDMAXLEN = 270 - ROOT.TSnIOHeaderFrame.kMaxSizeOf

# station to talk to
macadr="0002F7F0C0F8" # stn8 (test)
#macadr="0002F7F0C3B6" # stn3 (east)
#macadr="0002F7F0C445" # stn6 (north)
#macadr="0002F7F0C41C" # stn4 (center)
#macadr=None # not talking to anyone

# whether to disable actually sending emails
disableOutgoingEmails=False
# whether to send no config codes or not
disableNoConfigCodeEmail=False


infn = ''
isdefconf=False
inext = ''
sbdProtocols = {}

class AriStationProtocolSendToBuff(stationProtocol):
    
    def __init__(self, imapProt, ident):
        self.imapProtocol = imapProt
        self.ident        = ident
        self.macaddress   = []
        self.chunks       = []
        self.prevTot      = None
        self.prevNum      = None
        self.outBuff      = []
        printout(vtype.kDebug,
                 "GetInConfName={0}".format(self.GetInConfName()))
        printout(vtype.kDebug,
                 "IsDEFCONF={0}".format(self.IsDEFCONF()))
        
    
    def GetInConfName(self):
        global infn
        return infn
    
    def IsTalkingTo(self, mac):
        printout(vtype.kDebug, "mac={0}. macadr={1}, talkingTo={2}".format(
                mac, macadr, (mac==macadr)))
        if mac not in self.macaddress:
            self.macaddress.append(mac)
        printout(vtype.kDebug, "self.macaddress={0}".format(self.macaddress))
        return mac == macadr

    def IsDEFCONF(self):
        global isdefconf
        return isdefconf

    def clearOutBuff(self):
        self.outBuff = []
    
    def handleSbdBinIn(self, mnum, mtot, bmsg):
        printout(vtype.kDebug,'handleSbdBinIn({4}): prevTot={0}, mtot={1}. '\
                     'prevNum={2}, mnum={3}'.format(\
                self.prevTot, mtot, self.prevNum, mnum, self.macaddress))
        if ( (mnum!=None) and (mtot!=None) ):
            oktogo     = True
            resetCount = False
            if ( self.prevTot!=None ):
                # try to check that we're still on the same msg
                if ( (self.prevTot!=mtot) or (self.prevNum!=(mnum-1)) ):
                    # lost a chunk!
                    printout(vtype.kError,'Lost an SBD msg chunk.')
                    printout(vtype.kError,'prevTot={0}, mtot={1}. '\
                                 'prevNum={2}, mnum={3}'.format(\
                            self.prevTot, mtot, self.prevNum, mnum))
                    oktogo = False
            elif (mnum!=1):
                # picked up a message in the middle
                oktogo = False
            # build the binary message
            if (oktogo):
                self.chunks.append(bmsg)
                if (mnum==mtot): # count from 1. num==tot => done
                    fullmsg = b''.join(self.chunks)
                    printout(vtype.kDebug, "sending SBD binary msg to protocol")
                    printHex(vtype.kDebugData, bytearray(fullmsg))
                    self.dataReceived(fullmsg)
                    resetCount = True
            else:
                resetCount = True
            # set the msg num counters
            if (resetCount):
                self.prevTot = None
                self.prevNum = None
                self.chunks  = []
            else:
                self.prevTot = mtot
                self.prevNum = mnum
        
    
    def sendMsg(self, message):
        # disallow the no config code in order to not go over the
        # gmail message quota
        mcode = ROOT.Long()
        br = ROOT.TSnIOHeaderFrame.PyReadCodeFrom(message, mcode)
        isNoConfig = (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kNoConfigCode))
        if ( (isNoConfig==False) or
             ((disableNoConfigCodeEmail==False) and (isNoConfig)) ):
            if (mcode==castHeaderCode(ROOT.TSnIOHeaderFrame.kStringCode)):
                # just print the string
                printout(vtype.kUrgent, "SendString: {0}".format(message))
                printHex(vtype.kUrgent, bytearray(message))
            else:
                printout(vtype.kDebug, "SendToBuf:")
                printHex(vtype.kDebug, bytearray(message))
                self.outBuff.append(message)
        else:
            printout(vtype.kDebug, "No config code not appended to outBuff.")



class AriGmailImap4Client(imap4.IMAP4Client):
    connectedAt = None
    loopy       = None
    
    def doLogout(self, x):
        printout(vtype.kInfo,'Logging out.')
        d = self.logout()
        d.addErrback(uponFail, "doLogout")
        return d

    def connectionMade(self):
        printout(vtype.kDebug, "connectionMade")
        self.connectedAt = self.factory.reactor.seconds()
    
    def serverGreeting(self, caps):
        printout(vtype.kDebug, "serverGreeting")
        self.serverCapabilities = caps
        d = self.uponServerGreet(mailuser, mailpass)
        d.addCallback(self.doLogout)
        d.addErrback(uponFail, "Couldn't connect.")
        #d.addErrback(self.doLogout) can't log out if we didn't connect
        #d.addBoth(uponClose)

    def uponServerGreet(self, usernm, passwd):
        printout(vtype.kDebug, 'uponServerGreet')
        # gmail only allows plain login :(
        whenAuthed = self.login(usernm, passwd)
        whenAuthed.addCallback(self.repeatInboxSel)
        whenAuthed.addErrback(uponFail, 'whenAuthed')
        return whenAuthed

    def uponAuthentication(self):
        printout(vtype.kDebug, 'uponAuth')
        whenSel = self.select('Inbox')
        whenSel.addCallback(self.uponBoxSel)
        whenSel.addErrback(uponFail, 'whenSel')
        return whenSel

    def repeatInboxSel(self, result):
        printout(vtype.kDebug, 'repeatInboxSel')
        if (mailCheckPeriod==0):
            # connect just this once
            return self.uponAuthentication()
        else:
            self.loopy = task.LoopingCall(self.uponNextAuthentication)
            return self.loopy.start(mailCheckPeriod)

    def uponNextAuthentication(self):
        if ( (reconnectPeriod>0) and 
             ((self.factory.reactor.seconds() - self.connectedAt)
              > reconnectPeriod) ):
            # time to re-connect
            if (self.loopy!=None):
                self.loopy.stop()
        else:
            # check the inbox again
            return self.uponAuthentication()

    def uponBoxSel(self, boxinfo):
        printout(vtype.kDebug, 'uponBoxSel')
        whenUnreadListed = self.search("(UNSEEN)")
        whenUnreadListed.addCallback(self.uponUnreadListed)
        whenUnreadListed.addErrback(uponFail, 'whenUnreadListed')
        return whenUnreadListed

    def uponUnreadListed(self, msgids):
        printout(vtype.kDebug, 'uponUnreadListed')
        nmsgs = len(msgids)
        printout(vtype.kInfo,'{0} unread messages'.format(nmsgs))
        if (nmsgs>0):
            msgset = imap4.parseIdList(str(msgids).strip('[').strip(']'))
            whenHdrsFetched = self.fetchHeaders(msgset)
            whenHdrsFetched.addErrback(uponFail, 'whenHdrsFetched')
            whenBdysFetched = self.fetchBody(msgset)
            whenBdysFetched.addErrback(uponFail, 'whenBdysFetched')
            whenAllFetched = defer.DeferredList(
                [whenHdrsFetched, whenBdysFetched])
            whenAllFetched.addCallback(self.uponUnreadFetched)
            whenAllFetched.addErrback(uponFail, 'whenAllFetched')
            return whenAllFetched
        else:
            return defer.succeed('')

    def uponUnreadFetched(self, hdrsBdys):
        printout(vtype.kDebug, 'uponUnreadFetched')
        # write each message to a separate file
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
                whenWritten = self.writeSbdMail(msghdr, msgbdy)
                dl.append(whenWritten)
                msgs[msgnum] = (msghdr, msgbdy)
            d = defer.DeferredList(dl)
            d.addCallback(self.uponAllWritten, msgs)
            d.addErrback(uponFail, 'uponAllWritten')
            return d
        else:
            return defer.fail("Could not get unread headers.")

    def writeSbdMail(self, msghdr, msgbdy):
        printout(vtype.kDebug, 'writeSbdMail')
        sbdimei, sbddate = parseHeaderForImeiAndDate(msghdr)
        if ((sbdimei!=None) and (sbddate!=None)):
            datest = sbddate.strftime('%Y-%m-%d_%H-%M-%S')
            fname  = "{0}/SBD_{1}_{2}".format(rawoutdir,sbdimei,datest)
            ofname = fname
            fname += '.msg'
            # prevent an overwrite
            seq    = 1;
            while os.path.exists(fname):
                fname = "{0}_{1}.msg".format(ofname,seq)
                seq  += 1
            with open(fname,"w") as outf:
                outf.write(msghdr)
                outf.write(msgbdy)
        # return a fake deferred in case we can make this asynch at some point
        return defer.succeed('')

    def uponAllWritten(self, dlresult, msgs):
        printout(vtype.kDebug, 'uponAllWritten')
        # separate by imei (a bundle of msgs for each station)
        # then will order by date
        allbdys = {}
        for hdbd in msgs.itervalues():
            sbdimei, sbddate, momsn = parseForOutImeiDateMOMSN(hdbd)
            if (sbdimei!=None):
                if sbdimei not in allbdys:
                    allbdys[sbdimei] = []
                allbdys[sbdimei].append( (sbddate, momsn, hdbd[1]) )
        # sort all the messages by date, momsn, body on each imei
        dl = []
        for imei,datsbdys in allbdys.iteritems():
            # the order in which the imei's are handled is not important
            d = threads.deferToThread( sorted, datsbdys )
            d.addCallback(self.uponBdysSorted, imei)
            d.addErrback(uponFail, 'uponBdysSorted')
            dl.append(d)
        ad = defer.DeferredList(dl)
        ad.addCallback(self.uponAllProcd)
        ad.addErrback(uponFail, 'uponAllProcd')
        return ad
        
    def uponBdysSorted(self, allbdys, imei):
        printout(vtype.kDebug, 'uponBdysSorted')
        # make a (binary) protocol for this SBD modem
        sbdprot = None
        global sbdProtocols
        if imei in sbdProtocols:
            sbdprot = sbdProtocols[imei]
        else:
            sbdprot = AriStationProtocolSendToBuff(self, imei)
            sbdprot.connectionMade()
            sbdProtocols[imei] = sbdprot
        # loop over the messages from this SBD
        for date, momsn, body in allbdys:
            bdys = body.split(LINEBREAK)
            bytes = 0
            for b in bdys:
                if (b.startswith("Message Size")):
                    bytes = int(b.split(":")[1].strip())
                    break
            if (bytes>0):
                # got an actual SBD message from the station
                printout(vtype.kDebug, "SBD({0}) has msg w/ {1} bytes".format(
                        imei, bytes))

                b64msg = body[body.rfind("base64")+len("base64")\
                                  :body.rfind("--SBD.Boundary")]\
                                  .replace("\r","")\
                                  .replace("\n","")
                binmsg = base64.b64decode(b64msg)

                mnum, mtot, bmsg = self.handleSbdMsg(binmsg)

                sbdprot.handleSbdBinIn(mnum, mtot, bmsg)

        # check if we had any output we need to send to the station
        printout(vtype.kDebug, "{0} bytes in protocol output buffer".format(\
                len(sbdprot.outBuff)))
        if (len(sbdprot.outBuff)>0):
            # TODO: we could filter the outBuff and not send redundant
            # responses, in case we are reading many old emails...
            # But if we send one communication per email, the station will
            # only use the last few messages in the queue (according to
            # how many bytes it's expecting to get)
            for outMsg in sbdprot.outBuff:
                printout(vtype.kDebug, "Sending email to {0}".format(imei))
                printHex(vtype.kDebugData, bytearray(outMsg))
                self.mailAllToSBD(imei, outMsg)
            sbdprot.clearOutBuff()
        #sbdprot = None # help the garbage collection?
        return defer.succeed('')

    def handleSbdMsg(self, msg):
        printout(vtype.kDebug, 'handleSbdMsg')
        printHex(vtype.kDebug, bytearray(msg))
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
        return msgnum, msgtot, msg[hdbyt:]

    def uponAllProcd(self, result):
        printout(vtype.kDebug, 'uponAllProcd')
        # TODO: something?
        return defer.succeed('')

    def mailAllToSBD(self, sbdImei, binaryData,
                     fromAdr=mailfrom, atmTag="sbddata"):
        # send all the binary data to the SBD modem
        # breaking it up into chunks if necessary

        printout(vtype.kDebug, 'mailAllToSBD')

        # break up the data so it will fit in an SBD message
        def recurChunkify(bdata, chunks, clen):
            if (len(bdata)>0):
                chunks.append(bdata[:clen])
                recurChunkify(bdata[clen:], chunks, clen)
        chunks=[]
        recurChunkify(binaryData, chunks, SBDMAXLEN)

        def printChunks(chnks, tag=''):
            printout(vtype.kDebugData, "{0}chunks = {1}".format(tag,chnks))
            if (getVerbosity()>=vtype.kDebugData):
                for x in chnks:
                    printHex(vtype.kDebugData, bytearray(x))
                    
        printChunks(chunks)
        
        # prepend each chunk with the SBD header and then email it
        # do it recursively so we're sure the messages go out in order
        def sendOneChunk(result, chunks, num, tot):
            #printChunks(chunks, "SOC: ")

            if (num<tot):
                chk = chunks[num]
                num += 1 # msg num starts from 1 not 0
                shd = array.array('c', '0'*HEADER_LEN)
                ROOT.TSnIOHeaderFrame.WriteTo(shd,
                                              ROOT.TSnIOHeaderFrame.kSbdMsgCode,
                                              ROOT.TSnIOHeaderFrame.
                                                  GetSbdHdLenFrom(num, tot) )
                # prepend the chk with the sbd header
                bin = b''.join( [ b''.join(shd), chk ] )
                
                printout(vtype.kDebugData,"binary msg for email:")
                printHex(vtype.kDebugData,bytearray(bin))
                
                # send it
                msgfile, toAdr = buildMsgFile(mailfrom, sbdImei, bin)
                if ( (disableOutgoingEmails 
                      #or (macadr not in sbdProtocols[sbdImei].macaddress)
                      )
                      and (sbdProtocols[sbdImei].turnOffThmHook==False) ):
                    # only talk to the one station so that we don't use
                    # up our outgoing mail quota
                    printout(vtype.kInfo, "Outgoing email to {0} disabled."
                             .format(sbdImei))
                    printout(vtype.kDebugData, "Unsent message:")
                    printHex(vtype.kDebugData,bytearray(bin))
                    return defer.succeed("sent")
                else:
                    #d = sendGmail(mailuser, mailpass,
                    d = sendGmail(self.factory.reactor,
                                  smtpuser, smtppass,
                                  mailfrom,
                                  msgfile, toAdr)
                    # send the next after
                    d.addCallback(sendOneChunk, chunks, num, tot)
                    d.addErrback(uponFail, 'sendOneChunk')
                    return d
            else:
                return defer.succeed('all sent')

        ctot = len(chunks)
        d = sendOneChunk('', chunks, 0, ctot)
        d.addCallback(self.uponAllMailedToSBD, sbdImei)
        d.addErrback(uponFail, 'uponAllMailedToSBD')
        return d

    def uponAllMailedToSBD(self, result, sbdImei):
        printout(vtype.kDebug, 'uponAllMailedToSBD')
        sbdProtocols[sbdImei].turnOffThmHook = False
        return defer.succeed('')


class AriGmailImap4ClientFactory(protocol.ReconnectingClientFactory):
    protocol = AriGmailImap4Client
    reactor  = None
    
    def __init__(self, usernm, context, reactr):
        self.ctx      = context
        self.usernm   = usernm
        self.maxDelay = maxReconnDelay
        self.reactor  = reactr
    
    def buildProtocol(self, addr):
        # connected
        self.resetDelay() # so we can reconnect quickly next time
        p = self.protocol(self.ctx)
        p.factory = self
        #p.registerAuthenticator(imap4.PLAINAuthenticator(self.usernm))
        return p


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
                dt = EmailUtils.parsedate(h.split("Date:")[-1].strip())
                sbddate = datetime.datetime(
                    dt[0], dt[1], dt[2], dt[3], dt[4], dt[5] )
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
                mdate = datetime.strptime(\
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

def uponFail(msg, loc):
    printout(vtype.kError,'FAIL <{0}>: {1}'.format(loc, msg))
    return msg

def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)
    
    global infn
    global isdefconf
    global inext
    if (len(sys.argv)>1):
        infn = sys.argv[1]
    if (len(sys.argv)>2):
        isdefconf = (sys.argv[2]=="IsDEFCONF")
    inext = splitFilenameExt(infn)[1]
    if ( (inext != 'dat') and (inext!='DAT') and (inext!='') and
         (inext != 'bin') and (inext!='BIN') ):
        printout(vtype.kError,'Need binary input file. Got [{0}]'.format(infn))
        sys.exit()


    from twisted.internet import reactor

    ccf     = ssl.ClientContextFactory()
    factory = AriGmailImap4ClientFactory(mailuser, ccf, reactor)
    
    reactor.connectSSL(mailhost, mailport, factory, ccf)
    
    printout(vtype.kInfo,'Starting...')
    reactor.run()

if __name__=="__main__":
    main()
