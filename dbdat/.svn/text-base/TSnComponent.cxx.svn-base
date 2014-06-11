#include "TSnComponent.h"

#include <TTimeStamp.h>

ClassImp(TSnComponent);

TSnComponent::TSnComponent(const TTimeStamp& inst,
                           const TTimeStamp& remv) :
   fInstallDate(new TTimeStamp(inst)),
   fRemovedDate(new TTimeStamp(remv)) {   
}

TSnComponent::~TSnComponent() {
   delete fInstallDate;
   delete fRemovedDate;
}

void TSnComponent::SetInstallDate(const TTimeStamp& inst) {
   delete fInstallDate;
   fInstallDate = new TTimeStamp(inst);
}

void TSnComponent::SetRemovedDate(const TTimeStamp& remv) {
   delete fRemovedDate;
   fRemovedDate = new TTimeStamp(remv);
}

void TSnComponent::SetInstallDate(const UInt_t tloc,
                                  const Bool_t isUTC,
                                  const Int_t secOffset,
                                  const Bool_t dosDate) {
   delete fInstallDate;
   fInstallDate = new TTimeStamp(tloc, isUTC, secOffset, dosDate);
}

void TSnComponent::SetRemovedDate(const UInt_t tloc,
                                  const Bool_t isUTC,
                                  const Int_t secOffset,
                                  const Bool_t dosDate) {
   delete fRemovedDate;
   fRemovedDate = new TTimeStamp(tloc, isUTC, secOffset, dosDate);
}
