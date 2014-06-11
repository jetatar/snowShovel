#include "TSnPowerReading.h"

#include <TClass.h>
#include <TString.h>

#include <stdexcept>

ClassImp(TSnPowerReading);
ClassImp(TSnPowerReading::TObsoletePars);

void TSnPowerReading::Copy(TObject& object) const {
   // provided for use from python
   if (object.IsA()->InheritsFrom("TSnPowerReading")) {
      dynamic_cast<TSnPowerReading&>(object) = *this;
   } else {
      object = *this;
   }
}

Float_t TSnPowerReading::GetCalVolt(const Float_t vgain) const {
   return GetAveV1() * vgain;
}

Float_t TSnPowerReading::GetCalCurr(const Float_t cped,
                                    const Float_t cgain) const {
   return (cped + ( (GetAveV2()-GetAveV1())*cgain ) );
}

void TSnPowerReading::Print(Option_t* option) const {
   Printf("TSnPowerReading: ");
   Printf("  ave V1 = %g", GetAveV1());
   Printf("  ave V2 = %g", GetAveV2());
   Printf("  rms V1 = %g", GetRmsV1());
   Printf("  rms V2 = %g", GetRmsV2());
   Printf("  est. V ~ %g", GetCalVolt());
   Printf("  est. C ~ %g", GetCalCurr());
   Printf("  time   = %s", GetTimeStamp().AsString());
   if (HasObsParams()) {
      Printf("  (Obsolete) Vadc1 = %hu", GetObsPars().fVadc1);
      Printf("  (Obsolete) Vadc2 = %hu", GetObsPars().fVadc2);
   }
}


const TSnPowerReading::TObsoletePars& TSnPowerReading::GetObsPars() const {
   if (HasObsParams()==kFALSE) {
      throw std::runtime_error("<TSnPowerReading::GetObsPars>: "
                               "No obsolete parameters available.");
   }
   return fObs;
}

UShort_t TSnPowerReading::GetObsVadc1() const {
   // returns obsolete Vadc if available, otherwise just
   // casts the AveV to a UShort and returns that
   
   if (HasObsParams()) {
      return GetObsPars().fVadc1;
   } else {
      return static_cast<UShort_t>(GetAveV1());
   }
}

UShort_t TSnPowerReading::GetObsVadc2() const {
   // returns obsolete Vadc if available, otherwise just
   // casts the AveV to a UShort and returns that
   
   if (HasObsParams()) {
      return GetObsPars().fVadc2;
   } else {
      return static_cast<UShort_t>(GetAveV2());
   }
}

void TSnPowerReading::SetObsVadc1(const UShort_t v) {
   // sets the obsolete Vadc parameter.
   // also sets the corresponding AveV parameter and makes the
   // RmsV be 0.
   fHasObs = kTRUE;
   fObs.fVadc1 = v;
   SetAveV1(v);
   SetRmsV1(0);
}

void TSnPowerReading::SetObsVadc2(const UShort_t v) {
   // sets the obsolete Vadc parameter.
   // also sets the corresponding AveV parameter and makes the
   // RmsV be 0.
   fHasObs = kTRUE;
   fObs.fVadc2 = v;
   SetAveV2(v);
   SetRmsV2(0);
}

