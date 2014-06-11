import os,sys
import itertools
import ROOT

kSampRate = 1.92e9 # Hz
kNsamps   = 128
kT        = kNsamps/kSampRate

def getSingleRate(thresh, sig, ped, cdf):
    prb = 1.0 - cdf(thresh,sig,ped)
    #print prb
    return kSampRate*prb

def getHiSingleRate(thresh, sig=1, ped=0):
    # return the trigger rate (Hz) for a given threshold
    # with sig=1 and ped=0, 'thresh' is in units of Vrms (=sigma)
    return getSingleRate(thresh, sig, ped, ROOT.Math.normal_cdf)

def getLoSingleRate(thresh, sig=1, ped=0):
    # return the trigger rate (Hz) for a given threshold
    # with sig=1 and ped=0, 'thresh' is in units of Vrms (=sigma)
    return getSingleRate(thresh, sig, ped, ROOT.Math.normal_cdf_c)

def getMajorityRate(minchans, hiThreshs, loThreshs=None):
    # minchans = minimum number of channels in the majority logic to trigger
    # hi/loThreshs = array of either:
    #       [ threshold chan 0, threshold chan 1, ... threshold chan N-1 ]
    #    or [ (thresh, sig, ped) ch0, (thresh, sig, ped) ch1, ... ]
    #    the length of the array must be >= minchans
    #
    # rates are for a single-sample pattern (i.e. HAAAAAAA and/or LAAAAAAA)
    #
    # for example:
    #    2 of 4 majority logic with high threshold at 5 sigma on all 4 chans:
    #      >>> getMajorityRate(2, [5]*4)
    #      0.12116330302277274
    #    3 of 4 majority logic with both high and low thresholds at 3 sigma:
    #      >>> getMajorityRate(3, [4]*4, [-4]*4)
    #      31.979113738944328

    nchans = len(hiThreshs)
    if loThreshs!=None:
        assert nchans == len(loThreshs)
    if (minchans<=nchans):
        rates = []
        for ch in range(0, nchans):
            hi = hiThreshs[ch]
            lo = None
            if loThreshs!=None:
                lo = loThreshs[ch]
                assert type(hi)==type(lo)
            def getRateFor(x, fcn):
                if (type(x) is tuple):
                    return fcn(x[0],x[1],x[2])
                else:
                    return fcn(x)
            cr = getRateFor(hi, getHiSingleRate)
            if (lo!=None):
                cr += getRateFor(lo, getLoSingleRate)
            rates.append(cr)
        #print rates
        p = itertools.combinations(rates, minchans)
        rate = 0
        for x in p:
            y = 1
            for z in x:
                y *= z
            rate += y
        rate *= (kT ** (minchans-1))
        return rate
    else:
        raise Exception('Require {0} channels, but only have {1} channels.'
                        .format(minchans, chanProps))


nck = {}
def nChooseK(n,k):
    if (n<k):
        raise Exception("n ({0}) should be bigger than k ({1})".format(n,k))
    global nck
    # need to be calculated?
    if (n,k) not in nck:
        # initialize "recursion" end points (no calculation necessary)
        for i in range(0, n+1):
            if (i,0) not in nck:
                nck[(i,0)] = 1
            if (i,i) not in nck:
                nck[(i,i)] = 1
        # fill in the array
        for i in range(1, n+1):
            for j in range(1, k-(n-i)+1):
                if (i,j) not in nck:
                    nck[(i,j)] = nChooseK(i-1,j-1) + nChooseK(i-1,j)
    return nck[(n,k)]
