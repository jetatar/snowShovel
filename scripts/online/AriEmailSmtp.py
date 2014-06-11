import cStringIO
from OpenSSL.SSL import TLSv1_METHOD
from email import MIMEMultipart, MIMEText
from email import encoders as EMsgEncoders
from twisted.mail.smtp import ESMTPSenderFactory
from twisted.internet import protocol
from twisted.internet import ssl
from twisted.internet import defer
import types

from AriUtils import *

#smtphost = "smtp.gmail.com"
smtphost = "smtp.uci.edu"
smtpport = 587

def buildGenericMsgFile(toAdr, fromAdr, subject, msg):
    # build a simple email with text only
    m = MIMEText.MIMEText(msg)
    if isinstance(toAdr, types.ListType):
        toAdr = ', '.join(toAdr)
    m['To'] = toAdr
    m['From'] = fromAdr
    m['Subject'] = subject

    # return a (fake) file object
    return cStringIO.StringIO(m.as_string()), toAdr    

def buildGenericMsgFileWithData(toAdr, fromAdr, subject, msg, data, atchname):
    # build the file-object for a single email
    # with a binary attachment
    
    m = MIMEMultipart.MIMEMultipart()
    # headers
    if isinstance(toAdr, types.ListType):
        toAdr = ', '.join(toAdr)
    m['To'] = toAdr
    m['From'] = fromAdr
    m['Subject'] = subject
    if (msg!=None):
        # text body
        m.attach(MIMEText.MIMEText(msg))

    # message data attachment
    atm = MIMEMultipart.MIMEBase('application','octet-stream',
                                 name=atchname)
    atm.set_payload(data)
    EMsgEncoders.encode_base64(atm)
    atm.add_header('Content-Disposition','attachment',filename=atchname)
    m.attach(atm)
    
    # return a (fake) file object
    return cStringIO.StringIO(m.as_string()), toAdr
    

def sendEmail(reactor,
              authUser, authPass,
              fromAdr,
              messageFile,
              toAdr,
              host=smtphost, port=smtpport):
    """
    @param authUser: email username
    @param authPass: email password
    @param fromAdr: email address to use in FROM
    @param toAdr: email address to use in TO
    @param messageFile: a file-like (can read() and close()) object containing
    the full message to be sent (i.e. the result of buildGenericMsgFile)
    @param host: the smtp host
    @param port: the port to use (for TLSv1)
    @return: A deferred that fires when the message has been sent (or fails)
    """
    
    contextFactory = ssl.ClientContextFactory()
    contextFactory.method = TLSv1_METHOD
    
    try:
        printout(vtype.kDebug, "Sending SBD email:")
        printout(vtype.kDebug, messageFile.getvalue())
    except:
        pass
    
    whenSent = defer.Deferred()
    
    senderFactory = ESMTPSenderFactory(
        authUser, authPass, fromAdr, toAdr,
        messageFile,
        whenSent,
        contextFactory=contextFactory)
    
    reactor.connectTCP(host, port, senderFactory)
    
    return whenSent

