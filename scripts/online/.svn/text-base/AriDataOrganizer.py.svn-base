import os,sys

from twisted.internet import defer, task, threads

from AriUtils import *
from AriFiles import ensureNoOverwrite, AriAsyncFile
from AriStationProtocol import binDir, uponFail
from AriSubproc import *

# switch: base directory in which to store raw files
rawBaseDir="/data/InSitu/raw"
# switch: base directory in which to store tree files
treeBaseDir="/data/InSitu/tree"
# switch: base directory in which to store rate/volt plot files
plotsBaseDir="/data/InSitu/statusPlots"
# switch: base directory in which to store strip charts
chartsBaseDir="/data/InSitu/stripCharts"
# switch: base directory in which to store duplicate files
dupeBaseDir="/data/InSitu/duplicates"
# switch: html server
htmlServ="arianna.ps.uci.edu"
# switch: directory on html server
htmlDir="/home/arianna/www/status"
# switch: prefix of strip charts
chartPrf="charts"
# switch: prefix of status strip charts
statChartPrf="statusCharts"
# switch: prefix of thermal rate graph file
thmRatePrf="thmrates"
# switch: prefix of voltage graph file
voltPrf="volts"
# switch: if true, update status charts. if false, don't calc rates/volts
appendChartPoints=True
# switch: time bin in which to calculate rates
rateTimeWin=900
# switch: length of time between checks for new data
checkDataPeriod=900 # seconds

#setVerbosity(vtype.kInfo)
setVerbosity(vtype.kDebug)


# constant: raw data file prefix
DATF_PREFIX="SnEvts"
# constant: name of rate vs time graph in file
RATEGRNM="gERvsT"
# constant: name of volt vs time graph in file
VOLTGRNM="gVolt"
# constant: name of current vs time graph in file
CURRGRNM="gCurr"

def startupMsg():
    global binDir, checkDataPeriod
    printout(vtype.kInfo, "Checking for raw data files in {0} "
             "every {1} seconds.".format(binDir, checkDataPeriod))

@defer.inlineCallbacks
def storeFileNoOverwrite(fn, odir, plainfn, sdir):
    printout(vtype.kDebug,"storeFileNoOverwrite")
    checkDirExists(odir)
    ofn = "{0}/{1}".format(odir,plainfn)
    ufn = ensureNoOverwrite(ofn)
    if (ufn!=ofn):
        # put in the duplicates
        ofn = ensureNoOverwrite("{0}/{1}/{2}".format(dupeBaseDir,sdir,plainfn))
        checkDirOfFileExists(ofn)
    printout(vtype.kDebug, "call: mv {0} {1}".format(fn, ofn))
    try:
        yield async_check_output(["mv",fn,"{0}".format(ofn)])
    except Exception as e:
        uponFail(e,"mv")

def appendGraphPoints(gfrom, gto):
    printout(vtype.kDebug,"appendGraphPoints")
    if ((gfrom!=None) and (gto!=None)):
        for i in xrange(0, gfrom.GetN()):
            j = gto.GetN()
            gto.SetPoint(j,
                         (gfrom.GetX())[i],
                         (gfrom.GetY())[i])
            if (gfrom.InheritsFrom("TGraphErrors") and
                gto.InheritsFrom("TGraphErrors")):
                gto.SetPointError(j,
                                  (gfrom.GetEX())[i],
                                  (gfrom.GetEY())[i])

def appendGraphFiles(fnfrom, fnto, gname):
    printout(vtype.kDebug,"appendGraphFiles. fnfrom={0}, to={1}, gname={2}"\
                 .format(fnfrom, fnto, gname))
    if (fnfrom!=None and fnto!=None):
        ffrom = ROOT.TFile(fnfrom)
        fto   = ROOT.TFile(fnto, "update")
        gfrom = ffrom.Get(gname)
        gto   = fto.Get(gname)
        if (gfrom!=None):
            ROOT.SetOwnership(gfrom, True)
            if (gto!=None):
                ROOT.SetOwnership(gto, True)
            else:
                printout(vtype.kDebug,"make new graph from old one ({0})"\
                             .format(gfrom.GetName()))
                gto = ROOT.TGraph(gfrom)
            appendGraphPoints(gfrom, gto)
            printout(vtype.kDebug,"Writing {0} to {1}".format(gto.GetName(),
                                                              fto.GetName()))
            fto.cd()
            gto.Write(gname, ROOT.TObject.kOverwrite)
        fto.Close()
        ffrom.Close()

@defer.inlineCallbacks
def doRawDataCheck():
    # this will be run in a thread (see checkRawData)
    printout(vtype.kDebug, "Doing raw data check.")
    macsAndGrfiles = {}
    files = []
    try:
        fls = os.listdir(binDir)
        for fn in fls:
            if (fn.startswith(DATF_PREFIX)):
                if (files==None):
                    files = []
                files.append("{0}/{1}".format(binDir,fn))
    except Exception as e:
        printout(vtype.kWarn,"No bin files, or problem listing {0}/{1}*"\
                     .format(binDir,DATF_PREFIX))
    for fn in files:
        try:
            plainfn = fn
            if '/' in fn:
                plainfn = fn[fn.rindex('/')+1:]
            if ( plainfn.startswith(DATF_PREFIX) ):
                printout(vtype.kDebug,"processing {0}".format(fn))
                mac,run,seq = parseSnEvtsFilename(plainfn)
                sdir = getSubDirFor(mac,run,seq)
                # make runtree
                tdir = "{0}/{1}".format(treeBaseDir,sdir)
                checkDirExists(tdir)
                tfn = "{0}/{1}{2}".format(tdir, 
                                          ROOT.TSnRawTreeMaker.kRTFprf,
                                          plainfn)
                tfn = tfn.replace(".dat",".root")

                try:
                    yield async_check_output(["root.exe","-b","-q",
                                  '$SNS/scripts/offline/makeRawTree.C+('\
                                      '"{0}","{1}")'\
                                      .format(fn, tdir)])
                except Exception as e:
                    uponFail(e,"makeRawTree.C")
                if mac not in macsAndGrfiles:
                    macsAndGrfiles[mac] = {}
                # make rates/volts file
                if (appendChartPoints):
                    pdir = "{0}/{1}".format(plotsBaseDir,sdir)
                    printout(vtype.kDebug,"pdir={0}".format(pdir))
                    checkDirExists(pdir)
                    rfn = "{0}/{1}.{2}".format(pdir,thmRatePrf,plainfn)
                    vfn = "{0}/{1}.{2}".format(pdir,voltPrf,plainfn)
                    rfn = rfn.replace(".dat",".root")
                    vfn = vfn.replace(".dat",".root")
                    grfiles = macsAndGrfiles[mac]
                    if (thmRatePrf not in grfiles):
                        grfiles[thmRatePrf] = []
                    if (voltPrf not in grfiles):
                        grfiles[voltPrf] = []
                    grfiles[thmRatePrf].append(rfn)
                    grfiles[voltPrf].append(vfn)
                    try:
                        yield async_check_output(["root.exe","-b","-q",
                                      '$SNS/scripts/offline/eventRates.C+('\
                                          '"{0}","{1}",{2})'\
                                          .format(tfn, rfn,rateTimeWin)])
                    except Exception as e:
                        uponFail(e,"eventRates.C")
                    try:
                        yield async_check_output(["root.exe","-b","-q",
                                      '$SNS/scripts/offline/voltPlots.C+('\
                                      '"{0}","{1}")'\
                                          .format(tfn, vfn)])
                    except Exception as e:
                        uponFail(e,"voltPlots.C")
                # move file to storage
                odir = "{0}/{1}/r{2:05d}/{3}".format(rawBaseDir,mac,run,
                                                     getDirForSeq(seq))
                checkDirExists(odir)
                yield storeFileNoOverwrite(fn, odir, plainfn, sdir)
        except Exception as e:
            printout(vtype.kError,"Error: {0}".format(sys.exc_info()[0]))
            uponFail(e, "raw file loop")
    defer.returnValue(macsAndGrfiles)

@defer.inlineCallbacks
def sendChartsToWeb(charts):
    # TODO: use twisted to do this
    printout(vtype.kDebug, "sendChartsToWeb({0})".format(charts))
    if (charts != None):
        for c in charts:
            x = "{1}:{2}/".format(c, htmlServ, htmlDir)
            cmd = "scp {0} {1}".format(c, x)
            printout(vtype.kDebug,cmd)
            try:
                yield async_check_output(["scp",c, x])
            except Exception as e:
                uponFail(e,"scp")

def getSCdirsFiles(mac):
    tdir = "{0}/{1}".format(treeBaseDir, mac)
    cdir = "{0}/{1}".format(chartsBaseDir, mac)
    checkDirExists(cdir)
    rfn = "{0}/{1}.{2}.root".format(cdir, thmRatePrf, mac)
    vfn = "{0}/{1}.{2}.root".format(cdir, voltPrf, mac)
    return tdir, cdir, rfn, vfn

def makeRateVoltPlots(macsAndGrfiles):
    printout(vtype.kDebug,"makeRateVoltPlots")
    for mac,grfiles in macsAndGrfiles.iteritems():
        tdir, cdir, rfn, vfn = getSCdirsFiles(mac)
        if (appendChartPoints):
            printout(vtype.kDebug,"grfiles={0}".format(grfiles))
            thisrfns = grfiles[thmRatePrf]
            for thisrfn in thisrfns:
                appendGraphFiles(thisrfn, rfn, RATEGRNM)
            thisvfns = grfiles[voltPrf]
            for thisvfn in thisvfns:
                appendGraphFiles(thisvfn, vfn, VOLTGRNM)
                appendGraphFiles(thisvfn, vfn, CURRGRNM)

@defer.inlineCallbacks
def makeStripChart(mac):
    printout(vtype.kDebug,"makeStripChart ({0})".format(mac))
    a = AriAsyncFile("stripchart.{0}.lock".format(mac))
    try:
        yield a.DeferUntilAvailable() # wait for lock
    except Exception as e:
        uponFail(e,"makeStripChart({0}) wait for lock".format(mac))
    tdir, cdir, rfn, vfn = getSCdirsFiles(mac)
    stg = "{0}/{1}.{2}".format(cdir, chartPrf, mac)
    stfn = "{0}/{1}.{2}.root".format(cdir, statChartPrf, mac)
    checkDirExists(cdir)
    printout(vtype.kDebug, "stg={0}, stfn={1}".format(stg,stfn))
    printout(vtype.kDebug, "rfn={0} ({1}), vfn={2} ({3})".format(
            rfn, os.path.exists(rfn), vfn, os.path.exists(vfn)))
    #if (os.path.exists(rfn) and
    #    os.path.exists(vfn)):
    printout(vtype.kDebug,"calling makeStripCharts")
    try:
        yield async_check_output(["root.exe","-b","-q",
                                 '$SNS/scripts/offline/makeStripCharts.C+('\
                                      '"{0}","{1}","{2}","{3}")'\
                                      .format(rfn, vfn, stg, stfn)])
    except Exception as e:
        uponFail(e,"call makeStripCharts.C({0})".format(mac))
    charts = []
    charts.append("{0}.gif".format(stg))
    charts.append("{0}.now.gif".format(stg))
    defer.returnValue(charts)

@defer.inlineCallbacks
def makeStripCharts(macs):
    # update strip charts
    printout(vtype.kDebug,"update stations: {0}".format(macs))
    charts = []
    deflist = []
    for mac in macs:
        printout(vtype.kDebug, "caling make strip chart {0}".format(mac))
        try:
            ch = yield makeStripChart(mac)
            if (ch!=None):
                charts += ch
            else:
                printout(vtype.kInfo, "no strip charts for {0}".format(mac))
        except Exception as e:
            uponFail(e,"makeStripChart({0}) thread ({1})".format(mac,ch))
    printout(vtype.kDebug,"msc: charts={0}".format(charts))
    try:
        yield sendChartsToWeb(charts)
    except Exception as ex:
        uponFail(ex,'runSendChartsToWeb')
    printout(vtype.kDebug,"makeStripCharts done")

@defer.inlineCallbacks
def checkRawData():
    printout(vtype.kDebug,"checkRawData")
    try:
        macsAndGrfiles = yield doRawDataCheck()
        try:
            makeRateVoltPlots(macsAndGrfiles)
            try:
                yield makeStripCharts(macsAndGrfiles.keys())
            except Exception as exe:
                uponFail(exe, "makeStripCharts dtt")
        except Exception as ex:
            uponFail(ex, "makeRateVoltPlots")
    except Exception as e:
        uponFail(e,"doRawDataCheck")

def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)

    from twisted.internet import reactor

    loopy = task.LoopingCall(checkRawData)
    loopy.start(checkDataPeriod,True)
    
    reactor.callWhenRunning(startupMsg)
    reactor.run()

if __name__=="__main__":
    main()
