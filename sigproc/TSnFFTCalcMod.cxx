#include "TSnFFTCalcMod.h"

#include "TSnCalWvData.h"
#include "TSnCalFFTData.h"

ClassImp(TSnFFTCalcMod);


void TSnFFTCalcMod::SlaveBegin() {
   ReqCalData(fDatBrNm.Data(), fDat);   
}


void TSnFFTCalcMod::Process() {
   LoadCalData(fDatBrNm.Data());
   if (fDat!=0) {
      
      TSnCalFFTData* fft = new TSnCalFFTData(fFftNm.Data(),
                                             Form("FFT of %s", fDat->GetName()),
                                             *fDat);
      AddObjThisEvt(fft);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not load data [%s]",
                fDatBrNm.Data());
   }

}
