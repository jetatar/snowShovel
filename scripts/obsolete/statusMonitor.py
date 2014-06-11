import os,sys
import subprocess

from twisted.internet import defer, task, threads

from AriUtils import *
from twistedIO import rstatDir, uponFail, HEADER_LEN, castHeaderCode
from readStatusPyRoot import *
from dataOrganizer import chartsBaseDir, statChartPrf, makeStripCharts

# switch: base directory in which to store status files
storeBaseDir="/data/2013_InSitu/rawStatus"
# switch: html server
htmlServ="arianna.ps.uci.edu"
# switch: directory on html server
htmlDir="/home/arianna/www/status"
# switch: temporary directory in which to store the html files
statOutDir="/tmp"
# switch: prefix of html output filename
statOutPrf="cstatus"
# switch: extention of html output file
statOutExt="html"
# switch: min/max dates to use in charts
statChartMinT=1351728000
statChartMaxT=1414800000
# switch: length of time between checks for new status updates
checkStatusPeriod=300 # seconds

# constants
GRNAME_ERvsT = "gStERvsT"
GRNAME_VvsT  = "gStVvsT"
GRNAME_CvsT  = "gStCvsT"
ALL_GNAMES   = [GRNAME_ERvsT, GRNAME_VvsT, GRNAME_CvsT]

setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebug)


import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat")
ROOT.gSystem.Load("$SNS/lib/libevdat")
ROOT.gSystem.Load("$SNS/lib/libonline")

def startupMsg():
    printout(vtype.kInfo, "Checking for status updates in {0} "
             "every {1} seconds.".format(rstatDir, checkStatusPeriod))

#class SFTPSession(SSHChannel):
#    name = 'session'
#    
#    def channelOpen(self, x):
#        d = self.conn.sendRequest(
#            self, 'subsystem', NS('sftp'), wantReply=True)
#        d.addCallbacks(self.uponSFTP)
#        
#    def uponSFTP(self, result):
#        client = FileTransferClient()
#        client.makeConnection(self)
#        self.dataReceived = client.dataReceived
#        self.conn.whenSftp.callback(client)
#
#class SFTPConnection(SSHConnection):
#    def serviceStarted(self):
#        self.openChannel(SFTPSession())
#
#def sftp(user, host, port):
#    opts = ClientOptions()
#    opts['host'] = host
#    opts['port'] = port
#    conn = SFTPConnection()
#    conn.whenSftp = defer.Deferred()
#    auth = SSHUserAuthClient(user, opts, conn)
#    connect(host, port, opts, verifyHostKey, auth)
#    return conn.whenSftp
#
#def transfer(client):
#    

def sendHtmlToWeb(res, outhtmls):
    printout(vtype.kDebug, "sendHtmlToWeb thread fork")
    printout(vtype.kDebug, "outhtmls={0}".format(outhtmls))
    return threads.deferToThread(doSendHtmlToWeb, outhtmls)

def doSendHtmlToWeb(outhtmls):
    # TODO: use twisted to do this
    printout(vtype.kDebug, "doSendHtemlToWeb")
    printout(vtype.kDebug, "outhtmls={0}".format(outhtmls))
    if (outhtmls != None):
        for htfn in outhtmls:
            cmd = "scp {0} {1}:{2}/".format(htfn, htmlServ, htmlDir)
            printout(vtype.kDebug,cmd)
            proc = subprocess.Popen(cmd,
                                    stdout=subprocess.PIPE, shell=True)
            (out, err) = proc.communicate()


def getMacYearMonFromFilename(fn):
    # assumes filename has the form
    #  status.MACADR.YYYYmmdd-HHMMSS.dat
    toks = fn.split('.')
    return toks[1], toks[2][0:4], toks[2][4:6]

def getHtmlFilen(stn):
    return '{0}/{1}{2}.{3}'.format(statOutDir,statOutPrf,stn,statOutExt)

def getHtmlFrom(stat, stn, power):
    printout(vtype.kDebug, "getHtmlFrom({0},{1},{2})".format(stat,stn,power))
    h = None
    if ((stat!=None) and (stn!=None)):
        printout(vtype.kDebug, "start html")
        h  = '<html><meta http-equiv="refresh" content="{0}"><body>'.format(
            checkStatusPeriod)
        printout(vtype.kDebug, "bef time")
        h += "<h3>Station {0}: update found {1} (UTC)</h3>".format(
            stn, datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S"))
        printout(vtype.kDebug, "time")
        h += "<p>Mac Adr {0}</p>".format(stat.GetMacAdrAsStr())
        h += "<p>Config label [{0}]</p>".format(stat.GetConfLabel())
        h += "<p>Station time {0}</p>".format(stat.GetStatusTime().AsString("s"))
        h += "<p>Current run {0}, seq {1}</p>".format(stat.GetRunNum(),
                                                      stat.GetSeqNum())
        printout(vtype.kDebug, "up to run")
        h += "<p>Therm trig rate {0:.3f}, event save rate {1:.3f}</p>".format(
            stat.GetThmRate(), stat.GetEvtRate())
        printout(vtype.kDebug, "rates")
        #kb = stat.GetTotKB() * 1e3 / 1024.
        #h += "<p>Num files {0}, total kB {1:.1f} ({2:.2f} MB)</p>".format(
        #    stat.GetNfiles(), kb, kb/1024.)
        #printout(vtype.kDebug, "nfiles")
        h += "<p>Free space on SD card {0:.3f} MB</p>".format(stat.GetFreeMB())
        if (power!=None):
            h += "<p>Voltage ~{0:2.2f} V</p>".format(power.GetCalVolt())
            h += "<p>Current ~{0:2.2f} A</p>".format(power.GetCalCurr())
        printout(vtype.kDebug, "power")
        h += '<p><img src="charts.{0}.now.gif"></p>'.format(stat.GetMacAdrAsStr())
        h += '<p><img src="charts.{0}.gif"></p>'.format(stat.GetMacAdrAsStr())
        h += "</body></html>"
        printout(vtype.kDebug, "close")
    printout(vtype.kDebug, "return h={0}".format(h))
    return h

def doStatusCheck(outhtmls, macs):
    printout(vtype.kDebug, "Doing status check.")
    # output: list of html files to send
    toupdate = {}
    deflist = []
    x  = "ls -larth {0}/status* ".format(rstatDir)
    x += "| awk '{ print $NF }'"
    proc = subprocess.Popen(x,
                            stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    files = out.split('\n')
    for fn in files:
        try:
            plainfn = fn[fn.rindex('/')+1:]
            if ( plainfn.startswith("status.") ):
                
                d = processFile(fn, plainfn, outhtmls, toupdate, macs)
                deflist.append(d)
                d.addBoth(moveStatusFile, fn, plainfn)

        except:
            pass

    dl = defer.DeferredList(deflist)
    dl.addCallback(doCloseGraphFiles, toupdate)
    dl.addErrback(uponFail,"allFilesProcd")
    return dl

def checkStatus():
    printout(vtype.kDebug, "checkStatus")
    outhtmls = []
    macs = []
    d = doStatusCheck(outhtmls, macs)
    d.addCallback(doMakeStripCharts, macs)
    d.addCallback(sendHtmlToWeb, outhtmls)
    d.addErrback(uponFail, 'checkStatus')
    return d

def doMakeStripCharts(outhtmls, macs):
    printout(vtype.kDebug, "doMakeStripCharts {0}".format(macs))
    return makeStripCharts(macs)

def getOrMakeGraph(graphs, gname, gtitle):
    printout(vtype.kDebug, "getOrMakeGraph({0})".format(gname))
    gr = None
    if (gname not in graphs):
        printout(vtype.kDebug,
                 "Graph [{0}] not in graphs. Making.".format(gname))
        printout(vtype.kDebug,"graphs={0}".format(graphs))
        gr = ROOT.TGraph()
        gr.SetName(gname)
        gr.SetTitle(gtitle)
        graphs[gname] = gr
    else:
        gr = graphs[gname]
    return gr

def processFile(fn, plainfn, outhtmls, toupdate, macs):
    printout(vtype.kDebug,"processFile reading {0}".format(fn))
    with open(fn,"rb") as inf:
        br, stat, power, evt, ssvers, prv, erv = readStatus(inf.read())
        printout(vtype.kDebug,"br = {0}".format(br))
        printout(vtype.kDebug, "stat={0}".format(stat))
        if (stat!=None):
            stat.Print()
            if (power!=None):
                power.Print()
            if (evt!=None):
                evt.Print()
            macadr = stat.GetMacAdrAsStr()
            printout(vtype.kDebug, "macadr={0}".format(macadr))
            printout(vtype.kDebug, "macs={0}".format(macs))
            printout(vtype.kDebug, "macadr not in macs = {0}".format(
                    macadr not in macs))
            if (macadr not in macs):
                printout(vtype.kDebug, "new mac {0}".format(macadr))
                macs.append(macadr)
            printout(vtype.kDebug, "getting stn")
            stn=0
            try:
                stn = getStnFromMacAdr(macadr)
                printout(vtype.kDebug, "station={0}".format(stn))
            except:
                printout(vtype.kDebug, "could not get stn number "
                         "for mac {0}".format(macadr))
            if (stn>0):
                html = getHtmlFrom(stat, stn, power)
                printout(vtype.kDebug,"html={0}".format(html))
                outfn = getHtmlFilen(stn)
                try:
                    with open(outfn,"w") as outf:
                        outf.write(html)
                    if outfn not in outhtmls:
                        outhtmls.append(outfn)
                    return addToGraphFromStatus(stat, power,
                                                chartsBaseDir,
                                                "update",
                                                toupdate)
                except:
                    printout(vtype.kError, "Could not open "\
                                 "output file {0}".format(
                            outfn))
    return defer.fail("processFile")

def moveStatusFile(res, fn, plainfn):
    printout(vtype.kDebug, "moveStatusFile {0}".format(fn))
    try:
        macadr,year,month = getMacYearMonFromFilename(plainfn)
        odir = "{0}/{1}/{2}/{3}".format(storeBaseDir,macadr,year,month)
        checkDirExists(odir)
        os.system("mv {0} {1}/".format(fn, odir))
    except:
        pass

def getOutputFile(outfn, opt, toupdate):
    printout(vtype.kDebug, "getOutputFile {0}".format(outfn))
    # see if file is already opened
    for of in toupdate.iterkeys():
        if (of.GetName()==outfn):
            printout(vtype.kDebug, "already open")
            return defer.succeed(of)
    # if we made it here, file needs opening
    return openTFileWhenUnlocked(outfn, opt, toupdate)

def openTFileWhenAble(outfn, opt, toupdate):
    printout(vtype.kDebug, "openTFileWhenAble {0}".format(outfn))
    return getOutputFile(outfn, opt, toupdate)

def openTFile(res, outfn, opt, toupdate, filelock):
    printout(vtype.kDebug,"Opening {0} ({1})".format(outfn,opt))
    outf = ROOT.TFile(outfn, opt)
    graphs = {}
    toupdate[outf] = [graphs, filelock]
    global ALL_GNAMES
    for gn in ALL_GNAMES:
        gg = outf.Get(gn)
        if (gg!=None):
            graphs[gn] = gg
            printout(vtype.kDebug,"Loaded {0} from {1}".format(gn,outfn))
    return outf

def openTFileWhenUnlocked(outfn, opt, toupdate, timeout=300):
    printout(vtype.kDebug, "openTFileWhenUnlocked {0}".format(outfn))
    filelock = defer.DeferredFilesystemLock("{0}.lock".format(outfn))
    d = filelock.deferUntilLocked(timeout)
    d.addCallback(openTFile, outfn, opt, toupdate, filelock)
    d.addErrback(uponFail,"waitForOutputFile")
    return d

def addToGraphFileWhenOpen(outfn, opt, status, power, toupdate):
    printout(vtype.kDebug, "addToGraphFileWhenOpen {0}".format(outfn))
    d = openTFileWhenAble(outfn, opt, toupdate)
    d.addCallback(getGraphsForFile, toupdate)
    d.addCallback(addToGraphs, status, power)
    d.addErrback(uponFail,"addToGraphFileWhenOpen")
    return d

def getGraphsForFile(outf, toupdate):
    printout(vtype.kDebug, "getGraphsForFile")
    graphs = None
    if (outf!=None):
        graphs = (toupdate[outf])[0]
    return graphs

def addToGraphs(graphs, status, power):
    printout(vtype.kDebug, "addToGraphs")
    if (status!=None):
        stime = status.GetStatusTime().GetSec()
        if ( (stime>statChartMinT) and
             (stime<statChartMaxT) ):
            printout(vtype.kDebug,"Adding to graphs")
            gervt = getOrMakeGraph(graphs, GRNAME_ERvsT,
                                   "Event Rate vs Time"
                                   ";Date, time (UTC);Event Rate (Hz)")
            gervt.SetPoint(gervt.GetN(), stime, status.GetThmRate())
            if (power!=None):
                gvvt = getOrMakeGraph(graphs, GRNAME_VvsT,
                                      "Voltage vs Time; Date,time (UTC); V_in (V)")
                gcvt = getOrMakeGraph(graphs, GRNAME_CvsT,
                                      "Current vs Time; Date,time (UTC); I_in (A)")
                gvvt.SetPoint(gvvt.GetN(), stime, power.GetCalVolt())
                gcvt.SetPoint(gcvt.GetN(), stime, power.GetCalCurr())

def addToGraphFromStatus(status, power, odir, opt, toupdate):
    printout(vtype.kDebug, "addToGraphsFromStatus")
    if (status!=None):
        macadr = status.GetMacAdrAsStr()
        outdir = "{0}/{1}".format(odir,macadr)
        checkDirExists(outdir)
        outfn = "{0}/{1}.{2}.root".format(
            outdir,statChartPrf,macadr)
        return addToGraphFileWhenOpen(outfn, opt,
                                      status, power, toupdate)

def chartsFromArchives(cdir=chartsBaseDir):
    # collect data from archived status updates
    # and store graph files in cdir
    # NOTE: this imports & run a reactor!
    from twisted.internet import reactor
    toupdate = {}
    for path, dirs, files in os.walk(storeBaseDir):
        print "checking {0}".format(path)
        for infn in files:
            if infn.startswith("status"):
                fulfn = "{0}/{1}".format(path,infn)
                with open(fulfn,"rb") as inf:
                    br, status, power, evt, ssvers, prv, erv =\
                        readStatus(inf.read())
                    addToGraphFromStatus(status, power,
                                         cdir, "recreate",
                                         toupdate)
    closeGraphFiles(toupdate)

def closeGraphFiles(toupdate):
    printout(vtype.kDebug, "closeGraphFiles")
    for outf,graphslock in toupdate.iteritems():
        if (outf!=None):
            outf.cd()
            graphs = graphslock[0]
            for gr in graphs.itervalues():
                printout(vtype.kDebug,
                         "Writing {0} to {1}".format(gr.GetName(), 
                                                     outf.GetName()))
                gr.Write(gr.GetName(), ROOT.TObject.kOverwrite)
            outf.Close()
            lock = graphslock[1]
            if (lock!=None):
                lock.unlock()
    toupdate.clear()

def doCloseGraphFiles(res, toupdate):
    printout(vtype.kDebug, "doCloseGraphFiles")
    closeGraphFiles(toupdate)

def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)

    from twisted.internet import reactor

    loopy = task.LoopingCall(checkStatus)
    loopy.start(checkStatusPeriod)
    
    reactor.callWhenRunning(startupMsg)
    reactor.run()
    
if __name__=="__main__":
    main()


