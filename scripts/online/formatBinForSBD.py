import sys, os, array
import ROOT

ROOT.gSystem.Load("$SNS/lib/libonline")

infn = sys.argv[1]
infspl = infn.rsplit('.',1)
outfn = '{0}.sbd'.format(infspl[0])

if (len(sys.argv)<2):
    print 'No file specified.'
    sys.exit()

if (len(sys.argv)>2):
    print 'Expect only one file.'
    sys.exit()

fsiz = os.path.getsize(infn)
# SBD msg = 270 bytes max. but we need to prepend the header
if (fsiz>(270-ROOT.TSnIOHeaderFrame.SizeOf())):
    print 'Binary message in file [{0}] is too large ({1})'\
        .format(infn, fsiz)
elif (fsiz==0):
    print 'Null file or file [{0}] not found.'.format(infn)
else:
    print 'Writing output file: {0}'.format(outfn)
    with open(outfn,'wb') as outf:
        mm = array.array('c', '0'*ROOT.TSnIOHeaderFrame.SizeOf())
        ROOT.TSnIOHeaderFrame.WriteTo(mm,
                                      ROOT.TSnIOHeaderFrame.kSbdMsgCode,
                                      ROOT.TSnIOHeaderFrame.GetSbdHdLenFrom(1,
                                                                            1))
        outf.write(mm)
        with open(infn,'rb') as inf:
            outf.write(inf.read(fsiz))

            inf.close()
        
        outf.close()

