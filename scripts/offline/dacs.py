
def enum(**enums):
    return type('Enum', (), enums)

#dcals={
#       101:[2792.75,1299.75],
#       102:[2781.16,1316.95],
#       103:[2802.13,1297.3],
#       105:[2796.97,1298.93],
#       106:[2813.51,1292.38],
#       107:[2812.45,1294.02],
#       108:[2812.28,1300.57],
#       110:[2801.39,1302.21],
#       111:[2806.3,1351.35],
#       114:[2803.6,1303.85],
#       115:[2782.43,1319.41],
#       116:[2813.67,1285.83],
#       117:[2812.,1275.18]}
#
#vcals={
#    101:[-0.0009606,0.000834],
#    102:[0.0007927,0.00083305],
#    103:[-0.0068827,0.00083424],
#    105:[-0.00701868,0.000839544],
#    106:[-0.00875654,0.000848892],
#    107:[0.0201667,0.000844574],
#    108:[-0.0002033,0.000846332],
#    110:[-0.011199,0.00083064],
#    111:[0.005654,0.00080304],
#    114:[-0.007406,0.0008447],
#    115:[0.0025884,0.00081584],
#    116:[-0.0164615,0.000846715],
#    117:[-0.00149602,0.000845044]
#    }

threshTypes = enum(kHi=0, kLo=1)
calParams   = enum(kg1=0, kg2=1, kSt=2, kSa=3)

# dCard : [ [g1, g2, St, Sa] HIGH,       [g1, g2, St, Sa] LOW ]
dCardCals = {
     6: [ [0.689, 0.948, 1.05, -0.55], [0.716, 0.888, 1.009, -0.554] ],
   101: [ [0.781, 0.922, 1.04, -0.60], [0.806, 0.923, 1.021, -0.643] ],
   102: [ [0.797, 0.919, 1.01, -0.61], [0.811, 0.904, 1.019, -0.637] ],
   103: [ [0.773, 0.921, 1.05, -0.59], [0.811, 0.927, 1.025, -0.644] ],
   104: [ [0.748, 0.932, 1.09, -0.59], [0.799, 0.923, 1.042, -0.647] ],
   105: [ [0.793, 0.915, 1.05, -0.58], [0.793, 0.919, 1.017, -0.646] ],
   106: [ [0.777, 0.893, 1.06, -0.56], [0.801, 0.930, 1.034, -0.650] ],
   107: [ [0.777, 0.915, 1.06, -0.58], [0.803, 0.915, 1.031, -0.678] ],
   108: [ [0.777, 0.917, 1.06, -0.58], [0.811, 0.900, 1.024, -0.646] ],
   110: [ [0.781, 0.918, 1.04, -0.59], [0.809, 0.931, 1.029, -0.643] ],
   111: [ [0.805, 0.937, 1.03, -0.63], [0.845, 0.906, 0.994, -0.671] ],
   112: [ [0.759, 0.909, 1.08, -0.56], [0.803, 0.914, 1.041, -0.641] ],
   113: [ [0.752, 0.934, 1.09, -0.58], [0.795, 0.940, 1.049, -0.646] ],
   114: [ [0.781, 0.906, 1.04, -0.58], [0.811, 0.910, 1.030, -0.638] ],
   115: [ [0.816, 0.928, 1.00, -0.64], [0.795, 0.930, 1.028, -0.636] ],
   116: [ [0.761, 0.914, 1.07, -0.56], [0.809, 0.923, 1.031, -0.642] ],
   117: [ [0.767, 0.926, 1.06, -0.59], [0.790, 0.930, 1.050, -0.636] ],
   118: [ [0.756, 0.925, 1.08, -0.57], [1.209, 0.953, 0.812, -0.864] ],
   119: [ [0.787, 0.942, 1.03, -0.62], [0.833, 0.965, 1.013, -0.695] ],
   120: [ [0.783, 0.942, 1.04, -0.61], [0.981, 0.931, 0.899, -0.770] ],
   121: [ [0.783, 0.910, 1.04, -0.58], [0.801, 0.920, 1.021, -0.645] ],
   122: [ [0.785, 0.906, 1.04, -0.58], [0.806, 0.914, 1.024, -0.644] ],
   123: [ [0.785, 0.901, 1.05, -0.58], [0.813, 0.908, 1.023, -0.647] ],
   124: [ [0.781, 0.909, 1.05, -0.58], [0.814, 0.919, 1.020, -0.650] ]
    }

biasT = {
     6: 0.85,
   101: 0.85,
   102: 0.85,
   103: 0.85,
   104: 0.85,
   105: 0.85,
   106: 0.85,
   107: 0.85,
   108: 0.85,
   110: 0.85,
   111: 0.85,
   112: 0.85,
   113: 0.85,
   114: 0.85,
   115: 0.85,
   116: 0.85,
   117: 0.85,
   118: 0.85,
   119: 0.85,
   120: 0.85,
   121: 0.85,
   122: 0.85,
   123: 0.85,
   124: 0.85
   }

stnMap = {
   3: [102, 110, 101, 103],
   4: [105, 106, 108, 107],
   6: [111, 114, 115, 116]
   }

vToAdc = 4095. / 2.5

def getDac(dCard, Vth, hilo=threshTypes.kHi):
   cpars = dCardCals[dCard][hilo]
   if (Vth>1.0):
      Vth /= 1000. # mV to V
   d  = (Vth + biasT[dCard])*cpars[calParams.kg1]
   d += cpars[calParams.kSt]
   return int(round(d * vToAdc))

def getVth(dCard, dac, hilo=threshTypes.kHi):
   cpars = dCardCals[dCard][hilo]
   v  = dac / vToAdc
   v -= cpars[calParams.kSt]
   v /= cpars[calParams.kg1]
   v -= biasT[dCard]
   return v

def getAdc(dCard, vin, hilo=threshTypes.kHi):
   cpars = dCardCals[dCard][hilo]
   if (vin>1.0):
      vin /= 1000. # mV to V
   a  = (vin + biasT[dCard])
   a *= cpars[calParams.kg1]
   a *= cpars[calParams.kg2]
   a += cpars[calParams.kSa]
   return int(round(a * vToAdc))

def getVin(dCard, adc, hilo=threshTypes.kHi):
   cpars = dCardCals[dCard][hilo]
   v  = adc / vToAdc
   v -= cpars[calParams.kSa]
   v /= cpars[calParams.kg1]
   v /= cpars[calParams.kg2]
   v -= biasT[dCard]
   return v

def doCalc(fcn, stn, xvals, hilo=threshTypes.kHi):
   y = []
   for dCard, x in zip(stnMap[stn],xvals):
      y.append( fcn(dCard, x, hilo) )
   return y

def dacs(stn, Vths, hilo=threshTypes.kHi):
   return doCalc(getDac, stn, Vths, hilo)

def vth(stn, dacs, hilo=threshTypes.kHi):
   return doCalc(getVth, stn, dacs, hilo)

def vin(stn, adcs, hilo=threshTypes.kHi):
   return doCalc(getVin, stn, adcs, hilo)

def adcs(stn, Vins, hilo=threshTypes.kHi):
   return doCalc(getAdc, stn, Vins, hilo)


#def vin(card, adc, ped=0):
#    a = vcals[card][0]
#    b = vcals[card][1]
#    return a + (b*(adc-ped))
#
#def dacs(card, Vth):
#    return dac(card,Vth), dac(card,-Vth)
#
#def dac(card, Vth):
#    if (abs(Vth)>1):
#        Vth *= 1e-3
#    a = dcals[card][0]
#    b = dcals[card][1]
#    return int( a+(b*Vth) + 0.5)
#
#def vth(card, dac):
#    a = dcals[card][0]
#    b = dcals[card][1]
#    d = dac - a
#    d /= b
#    return d
