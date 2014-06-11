import os,sys,ROOT

# position = (rho, phi, err rho, err phi)
posns = { 11: [(0./3.28),
               0.,
               (3./3.28),
               360.],
          12: [(218./3.28),
               (270.),
               (7./3.28),
               2.],
          13: [(218./3.28),
               (0.),
               (7./3.28),
               2.],
          17: [(515./3.28),
               (315.),
               (10./3.28),
               5.],
          18: [(515./3.28),
               (315.),
               (10./3.28),
               5.],
          19: [(515./3.28),
               (315.),
               (10./3.28),
               5.]
          }

for r,psn in posns.iteritems():
    print "run {0}:".format(r)
    print "x = {0}".format(psn[0]*ROOT.TMath.Cos(psn[1]*ROOT.TMath.DegToRad()))
    print "y = {0}".format(psn[0]*ROOT.TMath.Sin(psn[1]*ROOT.TMath.DegToRad()))

