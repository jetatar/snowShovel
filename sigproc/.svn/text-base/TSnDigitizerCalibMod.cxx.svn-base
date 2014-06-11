#include "TSnDigitizerCalibMod.h"

#include "TSnDualGainCalibSet.h"
#include "TSnCalWvData.h"

ClassImp(TSnDigitizerCalibMod);


void TSnDigitizerCalibMod::SlaveBegin() {
   
   fCalSet = dynamic_cast<const TSnDualGainCalibSet*>(
      FindPublicObj(fCalSetNm.Data()));
   if (fCalSet==0) {
      SendError(kAbortAnalysis, "SlaveBegin",
                "Could not get calibration set [%s] from pub objs.",
                fCalSetNm.Data());
   }
   
   if (fFpnSubFromTree) {
      ReqBranch(fFpnSubNm.Data(), fFpnSub);
   }
   
}

void TSnDigitizerCalibMod::Process() {
   
   if (fFpnSubFromTree) {
      LoadBranch(fFpnSubNm.Data());
   } else {
      fFpnSub = dynamic_cast<TSnCalWvData*>(FindObjThisEvt(fFpnSubNm.Data()));
   }
   
   if (fFpnSub!=0) {
      
      TSnCalWvData* ampout = new TSnCalWvData(fCalDatNm.Data(),
                                              "Calibrated readout");
      const Float_t* f = fFpnSub->GetData();
      const Float_t* g = fFpnSub->GetError();
      Float_t*       c = ampout->GetData();
      Float_t*       e = ampout->GetError();
      for (Int_t s=0; s<NSnConstants::kTotSamps; ++s, ++f, ++g, ++c, ++e) {
         fCalSet->CalibrateWithErr(s, *f, *g, *c, *e );
      }
      
      // add to the event
      AddObjThisEvt(ampout);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get FPN subtracted data named [%s].",
                fFpnSubNm.Data());
   }
}

