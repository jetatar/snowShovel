import os, re, subprocess


def ReadInData(data, dest, bin):
    
    ls = subprocess.Popen(['ls',data], stdout=subprocess.PIPE)
    ls1 =  ls.stdout.read()
    DiffRuns = ls1.split()
    
    Exclude = ["run930","run931","run932","run933"]

    for x in Exclude:
        DiffRuns.remove(x)
    
    Nums = []
    FileListNum = []
 
   
    if bin == 0:
        
        for elem in DiffRuns:
            if elem.startswith("run"):
                a = re.findall(r'\d+',elem)
                FileListNum.append(a[0])

    elif bin == 2:
        FloatNum = []
        for elem in DiffRuns:
            if elem.startswith("run"):
                a = re.findall(r'\d+',elem)
                Nums.append(a[0])
        for x in Nums:
            FloatNum.append(int(x))
        a = max(FloatNum)
        FileListNum.append(a)
        
    else:
        FloatNum = []
        for elem in DiffRuns:
            if elem.startswith("run"):
                a = re.findall(r'\d+',elem)
                Nums.append(a[0])
        for x in Nums:
            FloatNum.append(int(x))
        for x in range(40):
            a = max(FloatNum)
            FileListNum.append(a)
            FloatNum.remove(a)
        
    print "Making symbolic links, please bare with me for a few minutes"
    
    for i in FileListNum:
        dat = "{0}/run{1}/hk/".format(data,i)
        x = subprocess.Popen(['ls',dat], stdout=subprocess.PIPE)
        file = x.stdout.read()
        hks = file.split()
        for f in hks:
            if f.startswith('hk'):
                SymLink = "ln -s {0} {1}".format(dat+f, dest)
                link = os.system(SymLink)
       

def MakePlots(linkdir, CodeDir, bin):
    
    if os.path.exists(linkdir+'allhk.dat.gz') == True:
        cmd0 = 'rm ' + linkdir + 'allhk.dat.gz'
        rm1 = os.system(cmd0)
    if os.listdir(linkdir) != []:
        cmd1 = 'cat ' + linkdir + '*.gz > ' + linkdir + 'allhk.dat.gz'
        cmd2 = 'rm ' + linkdir + 'hk*'
        a = os.system(cmd1)
        b = os.system(cmd2)



        runHK = CodeDir + 'runHkMkNtuples.py'
        runHK10 = CodeDir + 'runHkMkNtuples10.py'
        rhk = CodeDir + 'rhkPlot.C'
        jd = CodeDir + 'jdrawHkPlots.C'

    if bin == 0:

        a = os.system('python ' + runHK)
        b = os.system('root -b -l -q '+ rhk +'+\(\\\"{0}\\\",\\\"{1}\\\"\)'.format(linkdir + 'nt.allhk.dat.gz.root',linkdir + 'rhk_allhk.root'))
        c = os.system('root -b -l -q ' + jd + '\(\\\"{0}\\\",\\\"{1}\\\",\\\"0\\\"\)'.format(linkdir + 'rhk_allhk.root',linkdir + 'jdraw_allhk.root'))
        
    else:

        a = os.system('python ' + runHK10)
        b = os.system('root -b -l -q '+ rhk +'+\(\\\"{0}\\\",\\\"{1}\\\"\)'.format(linkdir + 'nt.allhk.dat.gz.root',linkdir + 'rhk_allhk.root'))
        c = os.system('root -b -l -q ' + jd + '\(\\\"{0}\\\",\\\"{1}\\\",\\\"0\\\"\)'.format(linkdir + 'rhk_allhk.root',linkdir + 'jdraw_allhk.root'))



"""Publish Plots"""

def runscripts(Dir, SymLinkDest, dest, CodeDir, bin):
        
    ReadInData(Dir,SymLinkDest,bin)
    MakePlots(SymLinkDest,CodeDir,bin)
        
        
    plots = []
        

    jdraw =  ROOT.TFile(SymLinkDest + "jdraw_allhk.root")
    wind = jdraw.Get("gWindV")
    solcur = jdraw.Get("gSol")
    windcur = jdraw.Get("gWind")
    windspeed = jdraw.Get("gAnem")
    bothcur = jdraw.Get("gWindSol")
    stnvol = jdraw.Get("gStationVn")
    temp1 = jdraw.Get("gTemp1")
    temp2 = jdraw.Get("gTemp2")
    
    wind.SetMarkerColor(ROOT.kMagenta+2)
    solcur.SetMarkerColor(ROOT.kMagenta)
    windcur.SetMarkerColor(ROOT.kViolet)
    temp1.SetMarkerColor(ROOT.kCyan+2)
    temp2.SetMarkerColor(ROOT.kRed)
    windspeed.SetMarkerColor(ROOT.kBlue)
    bothcur.SetMarkerColor(ROOT.kGreen-4)
    stnvol.SetMarkerColor(ROOT.kPink-7)
    wind.SetLineColor(ROOT.kMagenta+2)
    solcur.SetLineColor(ROOT.kMagenta)
    windcur.SetLineColor(ROOT.kViolet)
    temp1.SetLineColor(ROOT.kCyan+2)
    temp2.SetLineColor(ROOT.kRed)
    windspeed.SetLineColor(ROOT.kBlue)
    bothcur.SetLineColor(ROOT.kGreen-4)
    stnvol.SetLineColor(ROOT.kPink-7)
    
    temp1.SetLineWidth(1)
    temp2.SetLineWidth(1)

    wind.GetXaxis().SetNdivisions(10,5,0)
    solcur.GetXaxis().SetNdivisions(10,5,0)
    windcur.GetXaxis().SetNdivisions(10,5,0)
    temp1.GetXaxis().SetNdivisions(10,5,0)
    temp2.GetXaxis().SetNdivisions(10,5,0)
    windspeed.GetXaxis().SetNdivisions(10,5,0)
    bothcur.GetXaxis().SetNdivisions(10,5,0)
    stnvol.GetXaxis().SetNdivisions(10,5,0)
    
    text1 = ROOT.TText(30,1360.61e6,"Temp1")
    text2 = ROOT.TText(25,1360.61e6,"Temp2")
    text1.SetTextColor(ROOT.kCyan+2)
    text2.SetTextColor(ROOT.kRed)
    

    plots.append(wind)
    plots.append(solcur)
    plots.append(windcur)
    plots.append(windspeed)
    plots.append(bothcur)
    plots.append(stnvol)
    
    plotnames = ["wind","solcur","windcur","windspeed","bothcur","stnvol"]
    
    a = 0
    
    for i in plots:
        c = ROOT.TCanvas()
        i.Draw("APL")
        ROOT.gSystem.ProcessEvents()
        img = ROOT.TImage.Create()
        img.FromPad(c)
        img.WriteImage(dest+"{0}.png".format(plotnames[a]))
        a = a + 1
        c.Close()
    c = ROOT.TCanvas()
    temp1.Draw("APL")
    temp2.Draw("PL")
    text1.Draw("same")
    text2.Draw("same")
    ROOT.gSystem.ProcessEvents()
    img = ROOT.TImage.Create()
    img.FromPad(c)
    img.WriteImage(dest+"alltemp.png")
    c.Close()
    

        



import ROOT, time
    
while 1:
    
    #12 hours between cycles
    period = 43200 
    
    
    Dir = "/data/2013/icicle2/"
    SymLinkDest = "/home/arianna/online/2013/icicle2/alltime/"
    SymLinkDest10 = "/home/arianna/online/2013/icicle2/last10/"
    dest = "/home/arianna/www/online/2013/icicle2/alltime/"
    dest10 = "/home/arianna/www/online/2013/icicle2/last10/"
    CodeDir = "/home/arianna/snowShovel/scripts/iceSoft_offline/"

    runscripts(Dir, SymLinkDest, dest, CodeDir, 0)
    runscripts(Dir, SymLinkDest10, dest10, CodeDir, 1)

    print 'Completed! I will now sleep for {0} seconds and run again'.format(period)

    time.sleep(period)
