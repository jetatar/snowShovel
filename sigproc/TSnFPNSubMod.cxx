#include "TSnFPNSubMod.h"

#include "NSnConstants.h"
#include "TSnFPNCalibSet.h"
#include "TSnCalWvData.h"
#include "TSnRawWaveform.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnFPNSubMod);


void TSnFPNSubMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kEWvBrNm, fRaw);

   fFpnSet = 
      dynamic_cast<const TSnFPNCalibSet*>(FindPublicObj(fFpnSetName.Data()));
   if (fFpnSet==0) {
      SendError(kAbortAnalysis, "SlaveBegin",
                "Could not get FPN set named [%s] from published objects.",
                fFpnSetName.Data());
   }
}

void TSnFPNSubMod::Process() {
   LoadBranch(TSnRawTreeMaker::kEWvBrNm);
   
   if (fRaw!=0) {

      TSnCalWvData* cdat = new TSnCalWvData(fCalDatNm.Data(),
                                            "FPN subtracted waveform");
      const UShort_t* d   = fRaw->GetWvData();
      Float_t*        c   = cdat->GetData();
      Float_t*        e   = cdat->GetError();
      for (Int_t s=0; s<NSnConstants::kTotSamps; ++s, ++d, ++c, ++e) {
         fFpnSet->CalibrateWithErr(s, *d, 0, *c, *e);
      }
   
      AddObjThisEvt(cdat);

   } else {
      SendError(kAbortModule, "Process",
                "Could not get raw waveform data.");
   }
   
}

