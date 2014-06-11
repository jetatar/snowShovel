#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TSnRawTreeMaker.h"
#endif
#include "waveformPlots.C"

void waveformRawPlots(const Char_t* infn,
                      const Char_t* outdir,
                      const Bool_t  useThermals=kTRUE,
                      const Bool_t  useForced=kTRUE,
                      const Bool_t  rejectAdcToMB=kFALSE,
                      const Bool_t  checkCRC=kFALSE,
                      const UInt_t  verbosity=11) {
   waveformPlots(infn, outdir, TSnRawTreeMaker::kETname,
                 kTRUE, kFALSE, kFALSE,
                 useThermals, useForced, rejectAdcToMB, checkCRC,
                 verbosity);
}
