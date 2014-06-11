#include "TSnShiftCalWvDataMod.h"

#include "TSnCalWvData.h"
#include "TSnStopPosnSet.h"

ClassImp(TSnShiftCalWvDataMod);

void TSnShiftCalWvDataMod::SlaveBegin() {
   ReqBranch(fDatBrNm.Data(), fDat);
   
   if (fShiftNm.IsNull()) {
      SendError(kAbortAnalysis, "SlaveBegin",
                "Cannot make shifted waveform data without a name for "
                "the resulting TSnCalWvData object!");
   }
}

void TSnShiftCalWvDataMod::Process() {
   LoadBranch(fDatBrNm.Data());
   if (fDat!=0) {
      const TSnStopPosnSet* stops = dynamic_cast<const TSnStopPosnSet*>(
         FindObjThisEvt(fStopNm.Data()));
      if (stops!=0) {
         
         TSnCalWvData* shifted = 
            stops->NewShiftedData(*fDat, fShiftNm.Data(),
                                  "From TSnShiftCalWvDataMod");
         AddObjThisEvt(shifted);
         
      } else {
         SendError(kAbortModule, "Process",
                   "Could not get stop positions [%s]",
                   fStopNm.Data());
      }
   } else {
      SendError(kAbortModule, "Process",
                "Could not get calib wvfm data [%s]",
                fDatBrNm.Data());
   }
}
