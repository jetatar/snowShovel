#include "TSnSmoothDataLstSqrCnvMod.h"

#include <TObjArray.h>

#include "TSnInterp1DLstSqConvo.h"
#include "NSnConstants.h"
#include "TSnCalWvData.h"

ClassImp(TSnSmoothDataLstSqrCnvMod);


void TSnSmoothDataLstSqrCnvMod::SlaveBegin() {
   ReqCalData(fDatBrNm.Data(), fData);
}

void TSnSmoothDataLstSqrCnvMod::Process() {
   LoadCalData(fDatBrNm.Data());
   if (fData!=0) {
      
      TObjArray* alls = new TObjArray(NSnConstants::kNchans);
      alls->SetName(fIntpNm.Data());
      alls->SetOwner(kTRUE); // prevent memory leaks!
      
      TString sn;
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         sn = Form("%sCh%d",fIntpNm.Data(),ch);
         TSnInterp1DLstSqConvo* s =
            new TSnInterp1DLstSqConvo(sn.Data(),
                                      "LeastSquareConvolution",
                                      fN, fM, fS, *fData, ch, fVsTime);
         alls->AddAt(s, ch);
      }
      
      AddObjThisEvt(alls);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not load data [%s]", fDatBrNm.Data());
   }

}
