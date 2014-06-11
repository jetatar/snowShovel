#include "TSnSignalStrengthInfo.h"

#include <TClass.h>

#include "TSnDAQConfig.h"

ClassImp(TSnSignalStrengthInfo);

void TSnSignalStrengthInfo::Copy(TObject& object) const {
   // provided for use from python
   if (object.IsA()->InheritsFrom(TSnSignalStrengthInfo::Class())) {
      dynamic_cast<TSnSignalStrengthInfo&>(object) = *this;
   } else {
      object = *this;
   }
}

void TSnSignalStrengthInfo::Print(Option_t* option) const {
   Printf("TSnSignalStrengthInfo:");
   Printf("  comm type         = %s",
          TSnDAQConfig::GetDatPackBitName(
             static_cast<TSnDAQConfig::EDatPackBit>(fCommType)).Data());
   Printf("  signal strength   = %g",fSigStr);
   Printf("  sig str meas time = %s",fSSTime.AsString("c"));
}
