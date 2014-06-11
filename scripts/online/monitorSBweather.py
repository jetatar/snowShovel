import os, sys
import pytz
import gzip
import gc

from twisted.internet import defer, task, protocol, utils
from twisted.web.client import Agent, ResponseDone
from twisted.web.http_headers import Headers

from AriUtils import *
from AriDataOrganizer import sendChartsToWeb

# switch: base directory in which to store weather html files
sbwBaseDir="/data/2013_InSitu/SBweather"
# switch: base directory in which to store weather plot file
sbwPlotDir="/data/2013_InSitu/stripCharts/SBWeather"
# switch: name of weather plot file
sbwPlotFnm="SBWGraphsFromApr2013.root"
# switch: how often to check the weather web page (seconds)
checkPagePeriod=300
# switch: whether to compress the htmls files
gzipHtmlFiles=True
# switch: chart plots file prefix
sbwChartPrf="SBWcharts"
# switch: how long to wait for the web page to download (seconds)
dlTimeout=120

setVerbosity(vtype.kInfo)
#setVerbosity(vtype.kDebug)


def uponFail(msg, loc):
    printout(vtype.kError,'FAIL <{0}>: {1}'.format(loc, msg))

class ScottBaseWeatherGrapher(object):
    GRNAME_CTEMP="gCTemp"
    GRNAME_MAXTEMP="gMaxTemp"
    GRNAME_MINTEMP="gMinTemp"
    GRNAME_AVEWIND="gAveWind"
    GRNAME_MAXGUST="gMaxGust"
    ONEDAY=datetime.timedelta(days=1)

    def __init__(self, parser):
        self.ALL_GNAMES=[self.GRNAME_CTEMP, self.GRNAME_MAXTEMP,
                         self.GRNAME_MINTEMP,
                         self.GRNAME_AVEWIND, self.GRNAME_MAXGUST]
        self.outf = None
        self.graphs = None
        self.buildGraphTitleMap()
        self.setParser(parser)

    def buildGraphTitleMap(self):
        self.graphNmTitleMap = {}
        self.graphNmTitleMap[self.GRNAME_CTEMP] = \
            "Scott Base Temperature;date, time (UTC);temp (C)"
        self.graphNmTitleMap[self.GRNAME_MAXTEMP] = \
            "Scott Base High Temperature;date, time (UTC);high temp (C)"
        self.graphNmTitleMap[self.GRNAME_MINTEMP] = \
            "Scott Base Low Temperature;date, time (UTC);low temp (C)"
        self.graphNmTitleMap[self.GRNAME_AVEWIND] = \
            "Scott Base Average Wind;date, time (UTC);ave wind (knots)"
        self.graphNmTitleMap[self.GRNAME_MAXGUST] = \
            "Scott Base Maximum Wind Gust;date, time (UTC);max wind gust (knots)"

    def setParser(self, parser):
        self.parser = parser
        self.buildLineGraphNmMap()
        if (self.outf is None):
            self.openTFile()

    def buildLineGraphNmMap(self):
        self.lineGraphNmMap = {}
        self.lineGraphNmMap[self.parser.lineTypes.kCTemp]    = self.GRNAME_CTEMP
        self.lineGraphNmMap[self.parser.lineTypes.kMaxTemp]  = self.GRNAME_MAXTEMP
        self.lineGraphNmMap[self.parser.lineTypes.kMinTemp]  = self.GRNAME_MINTEMP
        self.lineGraphNmMap[self.parser.lineTypes.kAveWind]  = self.GRNAME_AVEWIND
        self.lineGraphNmMap[self.parser.lineTypes.kLastHour] = self.GRNAME_MAXGUST

    def openTFile(self):
        global sbwPlotDir
        global sbwPlotFnm
        outfn = '{0}/{1}'.format(sbwPlotDir, sbwPlotFnm)
        checkDirExists(sbwPlotDir)
        opt = 'update'
        if (self.outf!=None):
            printout(vtype.kDebug,"outf not None. closing.")
            self.closeTFile()
        printout(vtype.kDebug,"Opening {0} ({1})".format(outfn,opt))
        self.outf = ROOT.TFile(outfn, opt)
        if (self.graphs!=None):
            printout(vtype.kDebug,"graphs not None. clearing.")
            self.graphs.clear()
        self.graphs = {}
        for gn in self.ALL_GNAMES:
            gg = self.outf.Get(gn)
            if (gg!=None):
                self.graphs[gn] = gg
                printout(vtype.kDebug,"Loaded {0} from {1}".format(gn,outfn))


    def closeTFile(self):
        # for some reason, 
        if (self.graphs!=None):
            for gr in self.graphs.itervalues():
                printout(vtype.kDebug,
                         "Writing {0} to {1}".format(gr.GetName(), 
                                                     self.outf.GetName()))
                gr.Write(gr.GetName(), ROOT.TObject.kOverwrite)
            printout(vtype.kDebug, "Clearing graphs list")
            # for some reason, self.graphs.clear() won't cut it
            for k in self.graphs.keys():
                x = self.graphs[k]
                x.IsA().Destructor(x)
            self.graphs.clear()
            self.graphs = None
        if (self.outf!=None):
            printout(vtype.kDebug, "Closing graph file")
            self.outf.Close()
            # for seom reason, deleting the python ref won't cut it
            self.outf.IsA().Destructor(self.outf)
            self.outf = None
        gc.collect()

    def makeGraphIfNeeded(self, gname, gtitle):
        if (gname not in self.graphs):
            printout(vtype.kDebug,
                     "Graph [{0}] not in graphs. Making.".format(gname))
            self.graphs[gname] = ROOT.TGraph()
            self.graphs[gname].SetName(gname)
            self.graphs[gname].SetTitle(gtitle)
            printout(vtype.kDebug,"graphs={0}".format(self.graphs))

    def addToGraphs(self):
        rtime = self.parser.props[self.parser.lineTypes.kTime]
        utime = getUnixTimeFrom(rtime)
        printout(vtype.kDebug,"Got Unix time {0} for UTC {1}".format(
                utime, getDateStrFrom(rtime)))
        for k,v in self.parser.props.iteritems():
            if k in self.lineGraphNmMap:
                utm = utime
                if ( (k==self.parser.lineTypes.kLastHour) and
                     (self.parser.lineTypes.kLocTime in self.parser.props) ):
                    ltime = self.parser.props[self.parser.lineTypes.kLocTime]
                    try:
                        gtime = datetime.datetime.strptime(
                            '{0}-{1}-{2} {3}'.format(
                                ltime.year,ltime.month,ltime.day, v[1]),
                            '%Y-%m-%d %I:%M%p')
                    except:
                        gtime = datetime.datetime.strptime(
                            '{0}-{1}-{2} {3}'.format(
                                ltime.year,ltime.month,ltime.day, v[1]),
                            '%Y-%m-%d %I:%M %p')
                    gtime = self.parser.webpageTZ.localize(gtime)
                    # gtime should be earlier. if not, it's from the previous day
                    if (gtime>ltime):
                        gtime -= self.ONEDAY
                    utm = getUnixTimeFrom(gtime)
                    printout(vtype.kDebug,"Got Unix time {0} for NZT {1}".format(
                            utm, getDateStrFrom(gtime)))
                    v = v[0] # plot the speed
                grname = self.lineGraphNmMap[k]
                printout(vtype.kDebug,"Updating graph {0}".format(grname))
                printout(vtype.kDebug,"k={0}, v={1}, utm={2}".format(k, v, utm))
                self.makeGraphIfNeeded(grname,self.graphNmTitleMap[grname])
                self.graphs[grname].SetPoint(self.graphs[grname].GetN(), utm, v)
        outfn = self.outf.GetName()
        return outfn

class ScottBaseWeatherParser(object):
    webpageTZ = pytz.timezone('Pacific/Auckland')
    lineTypes = enum(kTime='LAST READING AT TIME: ',
                     kDate='DATE: ',
                     kCTemp='Current Temperature',
                     kMaxTemp='Maximum Temperature',
                     kMinTemp='Minimum Temperature',
                     kAveWind='Average windspeed',
                     kMaxGust='Maximum Gust',
                     kLastHour='last hour',
                     kGustTime='at: ',
                     kKnots='kts',
                     kDegree='&deg;',
                     kCloseBr='>',
                     kOpenBr='<',
                     kLocTime='LocalNZTime'
                     )

    def __init__(self, web):
        self.props = {}
        self.parseSBweather(web)

    def getProps(self):
        return self.props

    def readTimeFrom(self, line):
        # expect format like:
        # <TR><TH colspan=4><B><FONT SIZE=+1 color="#0">LAST READING AT TIME: 4:20 PM  DATE: 17 April 2013, time of next update: 4:30 p.m.</FONT></B></TH></TR>
        ltstart = line.index(self.lineTypes.kTime)
        ldstart = line.index(self.lineTypes.kDate)
        ltimestr = line[ltstart+len(self.lineTypes.kTime):ldstart].strip()
        ldatestr = line[ldstart+len(self.lineTypes.kDate):]
        ldatestr = ldatestr[:ldatestr.index(',')]
        try:
            nzdt = datetime.datetime.strptime('{0} {1}'.format(ldatestr,ltimestr),
                                              '%d %B %Y %I:%M %p')
        except:
            # gah, they keep changing their fucking web page
            # who uses a 24 hour clock and also AM/PM, anyway?!
            try:
                nzdt = datetime.datetime.strptime('{0} {1}'.format(ldatestr,
                                                                   ltimestr),
                                                  '%d %B %Y %I:%M%p')
            except:
                nzdt = datetime.datetime.strptime('{0} {1}'.format(ldatestr,
                                                                   ltimestr),
                                                  '%d %B %Y %H:%M %p')
        nzdt = self.webpageTZ.localize(nzdt)
        utcdt = nzdt.astimezone(pytz.timezone('UTC'))
        printout(vtype.kDebug,"** read time NZT {0}, UTC {1}".format(
                getDateStrFrom(nzdt), getDateStrFrom(utcdt)))
        return nzdt, utcdt

    def readTempFrom(self, line, tag):
        # expect format like:
        # <td><font color="#009900"><FONT SIZE=+1>Current Temperature </td><td><font color="#009900"><b>-33.9&deg;C (-29.1&deg;F)</b>,  Apparent temp -43.1&deg;C</FONT></font></td></tr>
        tmp = line[line.index(tag)+len(tag):]
        degr = tmp.index(self.lineTypes.kDegree)
        tmp = tmp[tmp.rfind(self.lineTypes.kCloseBr,0,degr)+1:degr].strip()
        printout(vtype.kDebug, '** read temp {0}'.format(float(tmp)))
        return float(tmp)

    def readWindFrom(self, line):
        # expect format like:
        # <tr><td><FONT SIZE=+1>Average windspeed<FONT SIZE=+0> (ten minute)</td><td>14.5 kts (26.8 kmh)</FONT></td>
        kts = line.index(self.lineTypes.kKnots)
        wnd = line[line.rfind(self.lineTypes.kCloseBr,0,kts)+1:kts].strip()
        printout(vtype.kDebug,'** read wind {0}'.format(float(wnd)))
        return float(wnd)

    def readGustFrom(self, line):
        # return both the gust speed and the time it occurred
        # expect format like:
        # <td><FONT SIZE=+1><font color="#009900">Maximum Gust </font><FONT SIZE=+0> (last hour)</td><td><font color="#009900">15.5 kts (28.6 kmh) at: 4:11PM</font></FONT></td></tr>
        wnd = self.readWindFrom(line)
        gti = line.index(self.lineTypes.kGustTime)+len(self.lineTypes.kGustTime)
        gtm = line[gti:line.find(self.lineTypes.kOpenBr,gti)].strip()
        printout(vtype.kDebug,'** read gust {0} at {1}'.format(wnd,gtm))
        return wnd,gtm

    def parseSBweather(self, web):
        lines = web.splitlines()
        lt = self.lineTypes
        self.props = {} # clear for this page
        for line in lines:
            printout(vtype.kDebugData,'line: {0}'.format(line))
            if lt.kTime in line:
                nzdt, utcdt = self.readTimeFrom(line)
                self.props[lt.kLocTime] = nzdt
                self.props[lt.kTime] = utcdt
            elif lt.kCTemp in line:
                self.props[lt.kCTemp] = \
                    self.readTempFrom(line, lt.kCTemp)
            elif lt.kMaxTemp in line:
                self.props[lt.kMaxTemp] = \
                    self.readTempFrom(line, lt.kMaxTemp)
            elif lt.kMinTemp in line:
                self.props[lt.kMinTemp] = \
                    self.readTempFrom(line, lt.kMinTemp)
            elif lt.kAveWind in line:
                self.props[lt.kAveWind] = self.readWindFrom(line)
            elif lt.kMaxGust in line:
                if lt.kLastHour in line:
                    self.props[lt.kLastHour] = self.readGustFrom(line)
                # could in principal store the "since 9am" too

class ScottBaseWeatherHtmlProtocol(protocol.Protocol):
    def __init__(self, whenBodyRcvd):
        self.alldone = whenBodyRcvd
        self.buff    = []

    def dataReceived(self, stuff):
        printout(vtype.kDebug, 'dataReceived ({0})'.format(len(stuff)))
        printHex(vtype.kDebugData, bytearray(stuff))
        self.buff.append(stuff)

    def getPage(self):
        return ''.join(self.buff)
        
    def connectionLost(self, reason):
        printout(vtype.kDebug, 'Lost connection: '.format(reason.
                                                          getErrorMessage()))
        if (reason.check(ResponseDone)):
            self.alldone.callback(self.getPage())
        else:
            self.alldone.errback(reason)

class ScottBaseWeatherDownloader(Agent):
    sbwMthd = 'GET'
    sbwURL  = 'http://antarcticanz.govt.nz/weather/SBweather/sbweather.html'
    #sbwURL  = 'http://www.antarcticanz.govt.nz/weather/SBweather/sbweather.html'
    sbwHdrs = Headers({'User-Agent': ['ARIANNA Project']})

    def updateCharts(self, outfn):
        printout(vtype.kDebug,"updateCharts: {0}".format(outfn))
        if (outfn!=None):
            global sbwPlotDir
            global sbwChartPrf
            ctag = '{0}/{1}'.format(sbwPlotDir,sbwChartPrf)
            printout(vtype.kDebug,'Updating charts {0}'.format(ctag))
            cmd = ROOT.TString(
                '$SNS/scripts/offline/makeSBWstripCharts.C("{0}","{1}")'.
                format(outfn, ctag))
            ROOT.gSystem.ExpandPathName(cmd)
            printout(vtype.kDebug,cmd.Data())
            args = ('-b', '-q', cmd.Data())
            plots = ['{0}.gif'.format(ctag), '{0}.now.gif'.format(ctag)]
            d = utils.getProcessValue('root.exe', args, os.environ)
            d.addCallback( self.doSendChartsToWeb, plots )
            d.addErrback(uponFail, 'updateCharts')
            return d
        else:
            return defer.succeed("no outfn")
        
    def doSendChartsToWeb(self, res, plots):
        sendChartsToWeb(plots)
    
    def uponPageDLd(self, page, regraph=False):
        printout(vtype.kDebug, 'uponPageDLd')
        printout(vtype.kDebugData, page)
        parser = ScottBaseWeatherParser(page)
        printout(vtype.kDebug, 'props: {0}'.format(parser.props))
        rtime = parser.props[parser.lineTypes.kTime]
        sbwhfn = self.getSBWHtmlFilenameFor(rtime)
        newpage = self.isPageNew(sbwhfn)
        if (newpage or regraph):
            if (newpage):
                printout(vtype.kDebug, 'new page {0}! parsing...'.format(sbwhfn))
                self.saveHtmlFile(page, sbwhfn)
            printout(vtype.kDebug, 'making new grapher')
            grapher = ScottBaseWeatherGrapher(parser)
            outfn = grapher.addToGraphs()
            grapher.closeTFile()
            printout(vtype.kDebug, "del grapher")
            del grapher
            return outfn
        else:
            printout(vtype.kInfo, 'page {0} exists. skipping...'.format(sbwhfn))
        return None

    def saveHtmlFile(self, page, sbwhfn):
        global gzipHtmlFiles
        try:
            opnr = open
            if (gzipHtmlFiles):
                opnr = gzip.open
            printout(vtype.kInfo, 'saving html to {0}'.format(sbwhfn))
            checkDirExists(sbwhfn[:sbwhfn.rfind('/')].strip())
            with opnr(sbwhfn,"w") as outf:
                outf.write(page)
        except IOError as e:
            uponFail(e, 'ScottBaseWeatherDownloader.saveHtmlFile')

    def isPageNew(self, htmlfn):
        return not os.path.isfile(htmlfn)
        
    def getSBWHtmlFilenameFor(self, utctime, tag='SBWeather'):
        global sbwBaseDir
        hdir = '{0}/{1}/{2:02d}/{3:02d}'.format(sbwBaseDir,
                                                utctime.year,
                                                utctime.month,
                                                utctime.day)
        ofn = '{0}/{1}_{2}.html'.format(hdir, tag,
                                        getDateStrFrom(utctime))
        if (gzipHtmlFiles):
            ofn += ".gz"
        return ofn

    def saveUnparsed(self, res, pageGetter):
        ofn = self.getSBWHtmlFilenameFor( datetime.datetime.utcnow(),
                                          'Unparsed_SBWeather' )
        self.saveHtmlFile( pageGetter.getPage(), ofn )
        return res

    def uponHeadersDLd(self, response):
        # got the headers. listen for the body
        printout(vtype.kDebug,"Got headers")
        whenBodyRcvd = defer.Deferred()
        pageGetter = ScottBaseWeatherHtmlProtocol(whenBodyRcvd)
        response.deliverBody(pageGetter)
        whenBodyRcvd.addCallback(self.uponPageDLd)
        whenBodyRcvd.addCallback(self.updateCharts)
        whenBodyRcvd.addErrback(self.saveUnparsed, pageGetter)
        whenBodyRcvd.addErrback(uponFail, 'uponHeadersDLd')
        return whenBodyRcvd

    def downloadPage(self):
        # ask for the web page
        ROOT.gROOT.CloseFiles()
        printout(vtype.kInfo,"Requesting weather web page...")
        d = self.request(self.sbwMthd, self.sbwURL, self.sbwHdrs, None)
        d.addCallback(self.uponHeadersDLd)
        d.addErrback(uponFail, 'downloadPage')
        timeoutCall = self._reactor.callLater(dlTimeout, d.cancel)
        def comp(p):
            if (timeoutCall.active()):
                timeoutCall.cancel()
            return p
        d.addBoth(comp)
        return d

def startupMessage():
    global checkPagePeriod
    printout(vtype.kInfo,"Grabbing Scott Base weather every {0} seconds..."\
                 .format(checkPagePeriod))

def chartFromArchives(bdir=sbwBaseDir, tags=['SBWeather']):
    # loop through html files and (remake) charts
    # NOTE: this imports & runs a reactor

    from twisted.internet import reactor
    dummy = ScottBaseWeatherDownloader(reactor)
    obs = log.startLogging(sys.stdout)

    @defer.inlineCallbacks
    def loopFiles(dummy, reactor):
        printout(vtype.kInfo,"loopFiles")
        outfn = None
        for path, dirs, files in os.walk(bdir):
            printout(vtype.kInfo,'Checking {0}'.format(path))
            for fn in files:
                for t in tags:
                    if fn.startswith(t):
                        opnr = open
                        if (fn.endswith("gz")):
                            opnr = gzip.open
                        infn = "{0}/{1}".format(path,fn)
                        with opnr(infn,"r") as inf:
                            outfn = dummy.uponPageDLd(inf.read(),True)
        yield dummy.updateCharts(outfn)
        printout(vtype.kInfo,"Done! Stopping reactor.")
        reactor.stop()
    
    reactor.callWhenRunning(loopFiles, dummy, reactor)
    printout(vtype.kInfo,"Starting...")
    reactor.run()
    if (obs!=None):
        printout(vtype.kDebug,"Stopping logger")
        obs.stop()

def main():
    # start logging - this must come after AriUtils is imported
    log.startLogging(sys.stdout)
    
    from twisted.internet import reactor

    global checkPagePeriod

    dler = ScottBaseWeatherDownloader(reactor)
    loopy = task.LoopingCall(dler.downloadPage)

    loopy.start(checkPagePeriod)
    
    reactor.callWhenRunning(startupMessage)
    reactor.run()


if __name__=='__main__':
    main()

