def enum(**enums):
    return type('Enum', (), enums)

stnMap = {
    10: [117, 112, 124, 132],
    11: [118, 121, 104, 113],
    12: [119, 120, 123, 122]
    }

vToAdc = 4095. / 2.5

threshTypes = enum(kHi=0, kLo=1)

dcThrshCals = {
    104: [ [0.774, -2137], [0.777, -2233] ],
    112: [ [0.760, -2109], [0.765, -2200] ],
    113: [ [0.761, -2105], [0.765, -2197] ],
    117: [ [0.764, -2118], [0.764, -2186] ],
    118: [ [0.772, -2140], [0.776, -2221] ],
    119: [ [0.768, -2109], [0.741, -2115] ],
    120: [ [0.767, -2103], [0.739, -2111] ],
    121: [ [0.756, -2075], [0.760, -2167] ],
    122: [ [0.783, -2171], [0.756, -2166] ],
    123: [ [0.769, -2100], [0.739, -2110] ],
    124: [ [0.756, -2089], [0.760, -2169] ],
    132: [ [0.760, -2099], [0.766, -2184] ],
    134: [ [0.758, -2101], [0.763, -2181] ],
    135: [ [0.755, -2068], [0.761, -2159] ],
    137: [ [0.757, -2084], [0.761, -2171] ]
    }

dcWaveCals = {
    104: [ [0.963,  2.2], [0.973,  0.3] ],
    112: [ [0.918,  1.0], [0.912, -0.3] ],
    113: [ [0.954,  0.5], [0.940, -1.3] ],
    117: [ [0.919,  1.4], [0.911, -0.1] ],
    118: [ [0.969,  0.4], [0.965,  0.3] ],
    119: [ [0.920, -0.2], [0.896, -2.2] ],
    120: [ [0.924, -0.3], [0.898, -1.9] ],
    121: [ [0.911,  7.5], [0.930, -2.3] ],
    122: [ [0.927,  1.3], [0.924, -0.7] ],
    123: [ [0.927, -0.4], [0.896, -2.0] ],
    124: [ [0.917,  1.3], [0.906, -0.8] ],
    132: [ [0.910,  0.9], [0.947, 11.2] ]
    }

thrDiffs = {
    104: [110, -115],
    112: [102, -100],
    113: [98,  -124],
    117: [94,  -86],
    118: [100, -110],
    119: [100, -130],
    120: [130, -120],
    121: [100, -104],
    122: [ 90, -100],
    123: [120, -114],
    124: [98,  -88],
    132: [82,  -98]
    }

def getDac(dCard, Vth):
    if (abs(Vth)<1.0):
        Vth *= 1000. # V to mV
    hilo = None
    if (Vth>0):
        hilo = threshTypes.kHi
    else:
        hilo = threshTypes.kLo
    cpars = dcThrshCals[dCard][hilo]
    d  = (Vth - cpars[1]) / cpars[0]
    d  = int(round(d))
    b  = d + thrDiffs[dCard][hilo]
    return (min(d,b),max(d,b))

def getVth(dCard, dac, hilo):
    cpars = dcThrshCals[dCard][hilo]
    v = (cpars[0] * dac) + cpars[1]
    return v

def getAdc(dCard, vin):
    if (abs(vin)<1.0):
        vin *= 1000. # V to mV
    hilo = None
    if (vin>0):
        hilo = threshTypes.kHi
    else:
        hilo = threshTypes.kLo
    cpars = dcWaveCals[dCard][hilo]
    a = (vin - cpars[1]) / cpars[0]
    return int(round(a))

def getVin(dCard, adc, hilo):
    cpars = dcWaveCals[dCard][hilo]
    v = (cpars[0] * adc) + cpars[1]
    return v

def doCalc(fcn, stn, xvals, hilo=None):
    y = []
    for dCard, x in zip(stnMap[stn],xvals):
        if (hilo!=None):
            y.append( fcn(dCard, x, hilo) )
        else:
            y.append( fcn(dCard, x) )
    return y

def dacs(stn, Vths):
    return doCalc(getDac, stn, Vths)

def vth(stn, dacs, hilo):
    return doCalc(getVth, stn, dacs, hilo)

def vin(stn, adcs, hilo):
    return doCalc(getVin, stn, adcs, hilo)

def adcs(stn, Vins):
    return doCalc(getAdc, stn, Vins)

