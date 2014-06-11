#include "TSnRejectBadVoltTimesMod.h"

#include <TTimeStamp.h>

#include "TSnRawTreeMaker.h"
#include "TSnPowerReading.h"

ClassImp(TSnRejectBadVoltTimesMod);

void TSnRejectBadVoltTimesMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kPwrBrNm, fPwr);   
}

void TSnRejectBadVoltTimesMod::Process() {
   LoadBranch(TSnRawTreeMaker::kPwrBrNm);
   if (0!=fPwr) {
      CheckTime(fPwr->GetTimeStamp().AsDouble());
   } else {
      SendError(kAbortModule, "Process",
                "Could not load power reading.");
   }
}

