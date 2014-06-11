import os,sys

from twisted.internet import defer, task, threads

from AriUtils import *
from AriFiles import *
from readStatusPyRoot import *
from AriDataOrganizer import chartsBaseDir, statChartPrf, makeStripCharts
from AriStationProtocol import rstatDir, uponFail
from AriSubproc import *

# switch: base directory in which to store status files
storeBaseDir="/data/InSitu/rawStatus"
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
statChartMaxT=1477958400 # Nov 11, 2016
# switch: length of time between checks for new status updates
checkStatusPeriod=300 # seconds

# constants
GRNAME_ERvsT = "gStERvsT"
GRNAME_VvsT  = "gStVvsT"
GRNAME_CvsT  = "gStCvsT"
ALL_GNAMES   = [GRNAME_ERvsT, GRNAME_VvsT, GRNAME_CvsT]
RSTAT_PREFIX = "status"

#setVerbosity(vtype.kInfo)
setVerbosity(vtype.kDebug)

def startupMsg():
    printout(vtype.kInfo, "Checking for status updates in {0} "
             "every {1} seconds.".format(rstatDir, checkStatusPeriod))

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
        wdr = "was NOT"
        if (stat.DidWatchdogReset()):
            wdr = "<b>WAS</b>"
        h += "<p>Last reset {0} by watchdog.</p>".format(wdr)
        if (power!=None):
            h += "<p>Voltage ~{0:2.2f} V</p>".format(power.GetCalVolt())
            h += "<p>Current ~{0:2.2f} A</p>".format(power.GetCalCurr())
        printout(vtype.kDebug, "power")
        h += '<p><img src="charts.{0}.now.gif"></p>'.format(stat.GetMacAdrAsStr())
        h += '<p><img src="charts.{0}.gif"></p>'.format(stat.GetMacAdrAsStr())
        h += "</body></html>"
        printout(vtype.kDebug, "close")
    return h

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

@defer.inlineCallbacks
def addToGraphFromStatus(status, power, odir, opt):
    printout(vtype.kDebug, "addToGraphFromStatus")
    if (status!=None):
        printout(vtype.kDebug, "have status")
        macadr = status.GetMacAdrAsStr()
        outdir = "{0}/{1}".format(odir,macadr)
        checkDirExists(outdir)
        outfn = "{0}/{1}.{2}.root".format(
            outdir,statChartPrf,macadr)
        # open the file (when it's available)
        printout(vtype.kDebug,"make afoutf")
        afoutf = AriROOTFile(outfn, opt)
        outf = yield afoutf.DeferUntilOpen() # return immediate if already open
        if (outf!=None):
            # get any graphs from the file (getOrMakeGraph 
            graphs = {}
            global ALL_GNAMES
            for gn in ALL_GNAMES:
                gg = outf.Get(gn)
                if (gg!=None):
                    ROOT.SetOwnership(gg, True)
                    graphs[gn] = gg
                    printout(vtype.kDebug,
                             "Loaded {0} from {1}".format(gn,outfn))
            #printout(vtype.kDebug,"opening file. tu[a]={0}"\
            #    .format(toupdate[afoutf]))
            printout(vtype.kDebug,"outf={0}, graphs={1}".format(outf, graphs))
            addToGraphs(graphs, status, power)
            outf.cd()
            for gr in graphs.itervalues():
                printout(vtype.kDebug,
                         "Writing {0} to {1}".format(gr.GetName(), 
                                                     outf.GetName()))
                gr.Write(gr.GetName(), ROOT.TObject.kOverwrite)
        afoutf.Close()
        

def processFile(fn, plainfn, outhtmls, macs):
    printout(vtype.kDebug,"processFile reading {0}".format(fn))
    with open(fn,"rb") as inf:
        msg = ROOT.TSnIOBuffer(inf.read())
        br, stat, power, evt, ssvers, prv, erv  = readStatus(msg)
        printout(vtype.kDebug, "br = {0}".format(br))
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
                                                "update")
                except:
                    printout(vtype.kError, "Could not open "\
                                 "output file {0}".format(
                            outfn))
    return defer.fail("processFile")

def getMacYearMonFromFilename(fn):
    # assumes filename has the form
    #  status.MACADR.YYYYmmdd-HHMMSS.dat
    toks = fn.split('.')
    return toks[1], toks[2][0:4], toks[2][4:6]

def moveStatusFile(res, fn, plainfn):
    printout(vtype.kDebug, "moveStatusFile {0}".format(fn))
    try:
        macadr,year,month = getMacYearMonFromFilename(plainfn)
        odir = "{0}/{1}/{2}/{3}".format(storeBaseDir,macadr,year,month)
        checkDirExists(odir)
        os.system("mv {0} {1}/".format(fn, odir))
    except:
        pass

@defer.inlineCallbacks
def doStatusCheck(outhtmls, macs):
    printout(vtype.kDebug, "Doing status check.")
    # output: list of html files to send
    #toupdate = {}
    deflist = []
    files = []
    try:
        fls = os.listdir(rstatDir)
        for fn in fls:
            if (fn.startswith(RSTAT_PREFIX)):
                if (files==None):
                    files = []
                files.append("{0}/{1}".format(rstatDir,fn))
    except Exception as e:
        printout(vtype.kWarn,"No bin files, or problem listing {0}/{1}*"\
                     .format(rstatDir,RSTAT_PREFIX))
    for fn in files:
        try:
            plainfn = fn[fn.rindex('/')+1:]
            if ( plainfn.startswith("status.") ):
                
                d = processFile(fn, plainfn, outhtmls, macs)
                deflist.append(d)
                d.addCallback(moveStatusFile, fn, plainfn)
                d.addErrback(uponFail,"processFile")

        except Exception as e:
            uponFail(e, 'processFiles')

    try:
        yield defer.DeferredList(deflist)
    except Exception as e:
        uponFail(e, 'processFileList')
    #ROOT.gObjectTable.Print()
    #closeGraphFiles(toupdate)
    #ROOT.gObjectTable.Print()


@defer.inlineCallbacks
def checkStatus():
    printout(vtype.kDebug, "checkStatus")
    outhtmls = []
    macs = []
    yield doStatusCheck(outhtmls, macs)
    yield makeStripCharts(macs)
    try:
        global htmlServ
        global htmlDir
        for htfn in outhtmls:
            printout(vtype.kDebug,"send {0}".format(htfn))
            yield async_check_output(["scp",htfn,"{0}:{1}/".format(
                        htmlServ, htmlDir)])
    except Exception as e:
        printout(vtype.kError,"sendHtmlToWeb: {0}".format(e))

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
