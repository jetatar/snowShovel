#include "TSnTrgInfo.h"

#include <TString.h>

#include "TSnMath.h"

ClassImp(TSnTrgInfo);

void TSnTrgInfo::Print(Option_t* option) const {
   Printf("TSnTrgInfo:");
   Printf("  trigger bits  = %hu", fTrgBits);
}

void TSnTrgInfo::PrintBits(const Bool_t endLine) const {
   TSnMath::PrintBits(fTrgBits, endLine);
}

TString TSnTrgInfo::GetBitsAsString(const UShort_t b) {
   return TSnMath::GetBitsAsString(b);
}

