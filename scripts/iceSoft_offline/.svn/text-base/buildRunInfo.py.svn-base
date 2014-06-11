#!/usr/bin/env python                                                                                                            

import os, sys, pickle, glob, time

# to compile in python:
#
# >>> import py_compile
# >>> py_compile.compile("buildRunInfo.py")
#
# After running
# > python buildRunInfo.py
#
# The output file can be read by:
# >>> import buildRunInfo
# >>> configs = buildRunInfo.loadConfigs()
# 
# or in Root:
# TPython::Exec("import buildRunInfo");
# TPython::Exec("configs = buildRunInfo.loadConfigs()");
# const Int_t ampsOnBitWord = 
#    TPython::Eval(Form("buildRunInfo.getAmpsStateBitWord(%u)",evtTime));
#


# switches
confdir = '/data/2012/icicle2/CONFIG_ARCHIVE'
outfn   = '/home/cjreed/work/AriannaData/icicle/CONFIG_INFO/icicleConf2012.dat'


# the interesting keys to be stored
ckeys = {'Hkd':['hkPeriod','atwdState','afarState','iridiumState',\
                'amp1State','amp2State','amp3State','amp4State',\
                'trimbleGpsState','heartBeatPulserState'],
         'Acqd':['softTrigPeriod','numCardsMajLogic','enableThermTrig',\
                 'dac0','dac1','dac2','dac3','plaHiThrsh','plaLoThrsh']
         }

# the big dictionary of configurations
configs={}

# class to store the config info
# should have:
# ctype = "Acqd" or "Hkd" or whatever
# utcdate = integer unix date
# strdate = utc date in string format
# [key] = [formatted value]
#    for exampleL: softTrigPeriod with value 67
# possible keys are specified in 'ckeys'
class conf:
   def __repr__(self):
      return self.ctype+"."+str(self.utcdate)
   
   def __str__(self):
      r = "\n".join( "%-25s : %s" % (k, self.__dict__[k]) \
         for k in sorted(self.__dict__) )
      return r
   
   def __getattr__(self, name):
      if name in self.__dict__:
         return self.__dict__[name]
      else:
         raise AttributeError
   
   def __eq__(self, other):
      return ( (self.ctype==other.ctype) and (self.utcdate==other.utcdate) )
   
   def __lt__(self, other):
      return ( (self.ctype < other.ctype) and (self.utcdate < other.utcdate) )
   
   def __gt__(self, other):
      return ( (self.ctype > other.ctype) and (self.utcdate > other.utcdate) )

# functions
def loadConfigs(inf="/home/cjreed/work/AriannaData/icicle/CONFIG_INFO/"\
                    "icicleConf2012.dat"):
   f = open(inf, 'rb')
   configs = pickle.load(f)
   return configs

def findMostRecentEntry(ctype, utcdate):
   if utcdate in configs[ctype]:
      return configs[ctype][utcdate]
   else:
      return configs[ctype][max(d for d in configs[ctype].keys() if d<utcdate)]

def getAmpsState(utcdate):
   ent = findMostRecentEntry('Hkd', utcdate)
   return [ent.amp1State, ent.amp2State, ent.amp3State, ent.amp4State]

def getAmpsStateBitWord(utcdate):
   amps = getAmpsState(utcdate)
   a=0
   l = len(amps)
   for i in range(0, l):
      a += amps[i] << i
   return a

def getUTCDateFromFilename(inf):
   # /data/2012/icicle2/CONFIG_ARCHIVE/Hkd.config.1328239710
   toks = inf.split('.')
   return toks[-1]

def getDateStringFromUTC(utc):
   return time.asctime(time.gmtime(utc))
   
def getValues(line):
   ktv  = line.split('#')
   tv   = (ktv[-1]).split('=')
   tn   = tv[0]
   vals = tv[1].rstrip().rstrip(';').split(',')
   typ  = tn[0]
   if (typ=='I'):
      return [int(x) for x in vals]
   elif (typ=='F'):
      return [float(x) for x in vals]
   elif (typ=='S'):
      return vals
   else:
      raise NotImplementedError("Unknown config value type '{0}'".format(typ))


# the main thing -- build the config database
for ctype in ckeys.keys():
   if ctype not in configs:
      configs[ctype] = {}
   cfg = configs[ctype]
   for infn in glob.glob(confdir+'/'+ctype+'*'):
      cf = conf()
      cf.ctype = ctype
      cf.utcdate  = int(getUTCDateFromFilename(infn))
      cf.strdate  = getDateStringFromUTC(cf.utcdate)
      
      inf = open(infn, 'r')
      for line in inf:
         for ck in ckeys[ctype]:
            if (line.startswith(ck)):
               vals = getValues(line)
               if (len(vals)==1):
                  cf.__dict__[ck] = vals[0]
               else:
                  cf.__dict__[ck] = vals
      
      cfg[cf.utcdate] = cf


f = open(outfn, 'wb')
pickle.dump(configs, f)
f.close()



