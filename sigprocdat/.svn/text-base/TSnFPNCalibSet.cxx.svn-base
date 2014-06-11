#include "TSnFPNCalibSet.h"

#include <TObjArray.h>

#include "TSnFPNCalibVal.h"

ClassImp(TSnFPNCalibSet);

Float_t TSnFPNCalibSet::GetFPNError(const Int_t smp) const {
   const TSnFPNCalibVal& c = 
      dynamic_cast<const TSnFPNCalibVal&>(GetCalibFor(smp));
   return c.GetFPNError();
}

Float_t TSnFPNCalibSet::GetRMSError(const Int_t smp) const {
   const TSnFPNCalibVal& c = 
      dynamic_cast<const TSnFPNCalibVal&>(GetCalibFor(smp));
   return c.GetRMSError();
}

void TSnFPNCalibSet::ChangeCalibOf(const Int_t smp, 
                                   const Float_t fpn,
                                   const Float_t rms,
                                   const Float_t n) {
   TSnFPNCalibVal& c = dynamic_cast<TSnFPNCalibVal&>(GetCalibFor(smp));
   c.SetFPN(fpn);
   c.SetRMS(rms);
   c.SetN(n);
}

void TSnFPNCalibSet::AddCalibAt(const Int_t smp, 
                                const Float_t fpn,
                                const Float_t rms,
                                const Float_t n) {
   if (HasCalibsFor(smp)) {
      ChangeCalibOf(smp, fpn, rms, n);
   } else {
      PassCalibTo(smp, new TSnFPNCalibVal(fpn, rms, n));
   }
}

