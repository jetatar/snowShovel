#include "TSnAmpOnSet.h"

#include <TString.h>

#include <stdexcept>

ClassImp(TSnAmpOnSet);


void TSnAmpOnSet::SetAmpOn(const UInt_t ch,
                           const Bool_t on) {
   if (fAmpsOn.size()<=ch) {
      fAmpsOn.resize(ch+1);
   }
   fAmpsOn.at(ch) = on;
}

Bool_t TSnAmpOnSet::IsValidForDb() const {
   return fAmpsOn.size()>0;
}

void TSnAmpOnSet::Print(Option_t* option) const {
   const UInt_t naos = fAmpsOn.size();
   for (UInt_t ch=0; ch<naos; ch++) {
      Printf("    ch%u: %s", ch, (fAmpsOn.at(ch) ? "on" : "off"));
   }
}

void TSnAmpOnSet::DoSizeError(const UInt_t s) const {
   throw std::runtime_error(Form("<TSnAmpOnSet::GetAmpOnBitword>: "
      "Too many amps [%u] to cast to type with [%u] bits.",
      static_cast < UInt_t >(kBitsPerByte*fAmpsOn.size()), s));
}
