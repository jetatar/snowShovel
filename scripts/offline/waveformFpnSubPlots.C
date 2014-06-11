#include "waveformPlots.C"

void waveformFpnSubPlots(const Char_t* infn,
                         const Char_t* outdir,
                         const Bool_t  useThermals=kTRUE,
                         const Bool_t  useForced=kTRUE,
                         const Bool_t  rejectAdcToMB=kFALSE,
                         const Bool_t  checkCRC=kFALSE,
                         const UInt_t  verbosity=11) {
   waveformPlots(infn, outdir, TSnSaveCalibDataMod::kFpnEvtTreeNm,
                 kTRUE, kTRUE, kFALSE,
                 useThermals, useForced, rejectAdcToMB, checkCRC,
                 verbosity);
}
                   
