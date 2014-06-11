import statusMonitor

sdir=None
toff=None

def chartsFromArchWithOffset(cdir=statusMonitor.chartsBaseDir):
    # collect data from archived status updates
    # and store graph files in cdir
    # NOTE: this imports & run a reactor!
    from twisted.internet import reactor
    toupdate = {}
    for path, dirs, files in statusMonitor.os.walk(sdir):
        print "checking {0}".format(path)
        for infn in files:
            if infn.startswith("status"):
                fulfn = "{0}/{1}".format(path,infn)
                with open(fulfn,"rb") as inf:
                    br, status, power, evt = statusMonitor.readStatus(
                        inf.read())
                    status.SetStatusTime(status.GetStatusTime().GetSec()
                                         + toff)
                    print "New status time = {0}".format(
                        status.GetStatusTime().AsString("s"))
                    statusMonitor.addToGraphFromStatus(status, power,
                                         cdir, "update",
                                         toupdate)
    statusMonitor.closeGraphFiles(toupdate)


def main():
    if (len(statusMonitor.sys.argv)<3):
        print "Usage: python addStatusToChartsWithOffset.py [raw status dir] [time offset secs]"
        statusMonitor.sys.exit()
        
    global sdir,toff
    sdir = statusMonitor.sys.argv[1]
    toff = int(statusMonitor.sys.argv[2])
        
    chartsFromArchWithOffset()


if __name__=="__main__":
    main()
