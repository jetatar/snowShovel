#include "TSnCommIncStatus.h"

#include <TString.h>

#include "TSnPowerReading.h"

ClassImp(TSnCommIncStatus);

TSnCommIncStatus::TSnCommIncStatus(const TTimeStamp& t, 
                                   const TSnStatusUpdate& s,
                                   const TSnPowerReading& p,
                                   const Int_t stvers,
                                   const Int_t pwvers) :
   TSnCommIncoming(t, s, stvers), fPower(new TSnPowerReading(p)),
   fPwIOvers(pwvers) {
}

TSnCommIncStatus::~TSnCommIncStatus() {
   delete fPower;
}

void TSnCommIncStatus::SetPower(const TSnPowerReading& p) {
   delete fPower;
   fPower = new TSnPowerReading(p);
}

void TSnCommIncStatus::Print(Option_t* option) const {
   TSnCommIncoming::Print(option);
   Printf("PowerReading (%p)", static_cast<const void*>(fPower));
   if (fPower!=0) {
      fPower->Print(option);
   }
   Printf("Power IO version:  %d", fPwIOvers);
}

