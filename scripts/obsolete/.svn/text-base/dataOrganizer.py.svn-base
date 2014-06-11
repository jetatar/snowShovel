import os,sys
import subprocess

from twisted.internet import defer, task, threads

from AriUtils import *
from twistedIO import binDir, uponFail, HEADER_LEN

# switch: base directory in which to store raw data files
storeBaseDir="/data/2013_InSitu/rawData"
# switch: base directory in which to store ntuples
ntupleBaseDir="/data/2013_InSitu/runtrees"
# switch: base directory in which to store makeWaves output files
wavesBaseDir="/data/2013_InSitu/histFiles"
# switch: base directory in which to store makeWaves output files
plotsBaseDir="/data/2013_InSitu/plotFiles"
# switch: base directory in which to store strip charts
chartsBaseDir="/data/2013_InSitu/stripCharts"
# switch: base directory in which to store duplicate files
dupeBaseDir="/data/2013_InSitu/duplicates"
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
# switch: temp directory
tmpDir="/tmp"
# switch: whether or not to run makeWaves
doMakeWave=False
# switch: whether to append strip chart points file by file (True)
#         or to recreate the plots from the entire data set each time (False)
appendChartPoints=True
# switch: length of time between checks for new data
checkDataPeriod=1800 # seconds


setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebug)

# constant: bit word for thermal trigs
THMTRIG=1
# constant: raw data file prefix
DATF_PREFIX="SnEvts"
# constant: sequences per directory (match MBED)
SEQ_PER_DIR=100
# constant: name of rate vs time graph in file
RATEGRNM="gERvsT"
# constant: name of volt vs time graph in file
VOLTGRNM="gVolt"

import ROOT
ROOT.gSystem.Load("$SNS/lib/libdbdat")
ROOT.gSystem.Load("$SNS/lib/libevdat")
ROOT.gSystem.Load("$SNS/lib/libonline")

def startupMsg():
    printout(vtype.kInfo, "Checking for raw data files in {0} "
             "every {1} seconds.".format(binDir, checkDataPeriod))

def parseFilename(fn):
    # assumes filename has the form
    #  SnEvtsM0002F7F0C445r00014s00369.dat
    rst = fn.index("r")
    sst = fn.rindex("s")
    mac = fn[fn.index("M")+1:rst]
    run = int(fn[rst+1:sst])
    seq = 0
    if (fn.rfind("_")!=-1):
        seq = int(fn[sst+1:fn.rindex("_")])
    else:
        seq = int(fn[sst+1:fn.index(".")])
    return mac,run,seq

def getDirForSeq(seq):
    sdr = 's{0:05d}'.format( (seq/SEQ_PER_DIR)*SEQ_PER_DIR )
    return sdr

def getSubDirFor(mac, run, seq):
    sdir = "{0}/r{1:05d}/{2}".format(mac,run,getDirForSeq(seq))
    return sdir

def storeFileNoOverwrite(fn, odir, plainfn, sdir):
    # prevent overwrites
    seq = 1
    ofn = "{0}/{1}".format(odir,plainfn)
    if (os.path.exists(ofn)):
        ddir = "{0}/{1}".format(dupeBaseDir, sdir)
        odir = ddir
        checkDirExists(odir)
        ofn = "{0}/{1}".format(odir,plainfn)
        while os.path.exists(ofn):
            #dot = ofn.rindex('.')
            #ofn = "{0}_{1}.{2}".format(ofn[:dot],seq,ofn[dot+1:])
            dot = plainfn.rindex('.')
            ofn = "{0}/{1}_{2}.{3}".format(odir,plainfn[:dot],
                                           seq,plainfn[dot+1:])
            seq += 1
    printout(vtype.kDebug,"mv {0} {1}".format(fn, ofn))
    subprocess.call(["mv",fn,"{0}".format(ofn)])

def appendGraphPoints(gfrom, gto):
    if (gfrom!=None):
        if (gto!=None):
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
        else:
            gto = ROOT.TGraph(gfrom)

def appendGraphFiles(fnfrom, fnto, gname):
    if (fnfrom!=None and fnto!=None):
        ffrom = ROOT.TFile(fnfrom)
        fto   = ROOT.TFile(fnto, "update")
        gfrom = ffrom.Get(gname)
        gto   = fto.Get(gname)
        appendGraphPoints(gfrom, gto)
        gto.Write(gname, ROOT.TObject.kOverwrite)
        fto.Close()
        ffrom.Close()

def doRawDataCheck():
    printout(vtype.kDebug, "Doing status check.")
    toupdate = []
    grfiles  = {}
    x  = "ls -larth {0}/{1}* ".format(binDir,DATF_PREFIX)
    x += "| awk '{ print $NF }'"
    proc = subprocess.Popen(x,
                            stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    files = out.split('\n')
    for fn in files:
        try:
            if '/' in fn:
                plainfn = fn[fn.rindex('/')+1:]
                if ( plainfn.startswith(DATF_PREFIX) ):
                    printout(vtype.kDebug,"processing {0}".format(fn))
                    mac,run,seq = parseFilename(plainfn)
                    sdir = getSubDirFor(mac,run,seq)
                    printout(vtype.kDebug,"sdir= {0}".format(sdir))
                    # make runtree
                    tdir = "{0}/{1}".format(ntupleBaseDir,sdir)
                    printout(vtype.kDebug,"tdir= {0}".format(tdir))
                    checkDirExists(tdir)
                    tfn = "{0}/{1}{2}".format(tdir, 
                                              ROOT.TSnRawTreeMaker.kRTFprf,
                                              plainfn)
                    tfn = tfn.replace(".dat",".root")
                    subprocess.call(["root.exe","-b","-q",
                                     '$SNS/scripts/offline/makeRawTree.C+('\
                                         '"{0}","{1}")'\
                                         .format(fn, tdir)])
                    if mac not in toupdate:
                        toupdate.append(mac)
                    # make hists file
                    if (doMakeWave):
                        hdir = "{0}/{1}".format(wavesBaseDir,sdir)
                        checkDirExists(hdir)
                        hfn = "{0}/hists.noFpn.{1}".format(hdir, plainfn)
                        hfn = hfn.replace(".dat",".root")
                        subprocess.call(["root.exe","-b","-q",
                                         '$SNS/scripts/offline/makeWaves.C+('\
                                             '"{0}","{1}")'\
                                             .format(tfn, hfn)])
                    # make rates/volts file
                    if (appendChartPoints):
                        pdir = "{0}/{1}".format(plotsBaseDir,sdir)
                        printout(vtype.kDebug,"pdir={0}".format(pdir))
                        checkDirExists(pdir)
                        rfn = "{0}/{1}.{2}".format(pdir,thmRatePrf,plainfn)
                        vfn = "{0}/{1}.{2}".format(pdir,voltPrf,plainfn)
                        rfn = rfn.replace(".dat",".root")
                        vfn = vfn.replace(".dat",".root")
                        grfiles[thmRatePrf] = rfn
                        grfiles[voltPrf] = vfn
                        subprocess.call(["root.exe","-b","-q",
                                         '$SNS/scripts/offline/evtRateOverTime.C+('\
                                             '"{0}","{1}",{2})'\
                                             .format(tfn, rfn, THMTRIG)])
                        subprocess.call(["root.exe","-b","-q",
                                         '$SNS/scripts/offline/plotVolts.C+('\
                                         '"{0}","{1}")'\
                                             .format(tfn, vfn)])
                    # move file to storage
                    odir = "{0}/{1}/r{2:05d}/{3}".format(storeBaseDir,mac,run,
                                                         getDirForSeq(seq))
                    checkDirExists(odir)
                    storeFileNoOverwrite(fn, odir, plainfn, sdir)
        except:
            printout(vtype.kError,"Error: {0}".format(sys.exc_info()[0]))
            pass
    return (toupdate, grfiles)

def sendChartsToWeb(charts):
    # TODO: use twisted to do this
    printout(vtype.kDebug, "charts={0}".format(charts))
    if (charts != None):
        for c in charts:
            x = "{1}:{2}/".format(c, htmlServ, htmlDir)
            cmd = "scp {0} {1}".format(c, x)
            printout(vtype.kDebug,cmd)
            subprocess.call(["scp",c, x])

def collectLinks(tdir):
    odir = "{0}/dataOrganizerLinks/".format(tmpDir)
    checkDirExists(odir)
    for tdir, dirs, files in os.walk(tdir):
        if ((len(files)>0) and (files[0].startswith("nt"))):
            for f in files:
                subprocess.call(["ln","-s","{0}/{1}".format(tdir,f),odir])
    return odir

def cleanLinks():
    odir = "{0}/dataOrganizerLinks".format(tmpDir)
    subprocess.call(["rm","-rf",odir])

def updateStripCharts(macsAndGrfiles):
    macs = macsAndGrfiles[0]
    grfiles = macsAndGrfiles[1]
    d = threads.deferToThread(makeRateVoltPlots, macs, grfiles)
    d.addCallback(makeStripCharts)
    d.addErrback(uponFail, 'updateStripCharts')
    return d

def runSendChartsToWeb(res, charts):
    d = threads.deferToThread(sendChartsToWeb, charts)
    d.addErrback(uponFail, 'runSendChartsToWeb')
    return d

def getSCdirsFiles(mac):
    tdir = "{0}/{1}".format(ntupleBaseDir, mac)
    cdir = "{0}/{1}".format(chartsBaseDir, mac)
    checkDirExists(cdir)
    rfn = "{0}/{1}.{2}.root".format(cdir, thmRatePrf, mac)
    vfn = "{0}/{1}.{2}.root".format(cdir, voltPrf, mac)
    return tdir, cdir, rfn, vfn

def makeRateVoltPlots(macs, grfiles):
    for mac in macs:
        tdir, cdir, rfn, vfn = getSCdirsFiles(mac)
        if (appendChartPoints):
            thisrfn = grfiles[thmRatePrf]
            thisvfn = grfiles[voltPrf]
            appendGraphFiles(thisrfn, rfn, RATEGRNM)
            appendGraphFiles(thisvfn, vfn, VOLTGRNM)
        else:
            ldir = collectLinks(tdir)
            subprocess.call(["root.exe","-b","-q",
                             '$SNS/scripts/offline/evtRateOverTime.C+('\
                             '"{0}/nt*root","{1}",1)'\
                             .format(ldir, rfn)])
            subprocess.call(["root.exe","-b","-q",
                             '$SNS/scripts/offline/plotVolts.C+('\
                             '"{0}/nt*root","{1}")'\
                                 .format(ldir, vfn)])
            cleanLinks()
    return macs

def removeFilelock(res, filelock):
    filelock.unlock()
    return res

def makeStripChartWhenUnlocked(mac, charts, timeout=600):
    filelock = defer.DeferredFilesystemLock("stripchart.{0}.lock".format(mac))
    d = filelock.deferUntilLocked(timeout)
    d.addCallback(runMakeStripChart, mac, charts)
    d.addCallback(removeFilelock, filelock)
    return d

def runMakeStripChart(res, mac, charts):
    return makeStripChart(mac, charts)

def makeStripCharts(macs):
    # update strip charts
    printout(vtype.kDebug,"update stations: {0}".format(macs))
    charts = []
    deflist = []
    for mac in macs:
        d = makeStripChartWhenUnlocked(mac, charts)
        deflist.append(d)
    dl = defer.DeferredList(deflist)
    dl.addCallback(runSendChartsToWeb, charts)
    dl.addErrback(uponFail, 'makeStripCharts')
    return dl

def makeStripChart(mac, charts):
    tdir, cdir, rfn, vfn = getSCdirsFiles(mac)
    stg = "{0}/{1}.{2}".format(cdir, chartPrf, mac)
    stfn = "{0}/{1}.{2}.root".format(cdir, statChartPrf, mac)
    checkDirExists(cdir)
    if (os.path.exists(rfn) and
        os.path.exists(vfn)):
        subprocess.call(["root.exe","-b","-q",
                         '$SNS/scripts/offline/makeStripCharts.C+('\
                         '"{0}","{1}","{2}","{3}")'\
                         .format(rfn, vfn, stg, stfn)])
        charts.append("{0}.gif".format(stg))
        charts.append("{0}.now.gif".format(stg))
    return charts

def checkRawData():
    d = threads.deferToThread(doRawDataCheck)
    d.addCallback(updateStripCharts)
    d.addErrback(uponFail, 'checkRawData')
    return d

def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)

    from twisted.internet import reactor

    loopy = task.LoopingCall(checkRawData)
    loopy.start(checkDataPeriod)
    
    reactor.callWhenRunning(startupMsg)
    reactor.run()

if __name__=="__main__":
    main()


