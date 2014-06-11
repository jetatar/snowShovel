#include "TSnCompMbed.h"

#include <cstdio>

#include <stdexcept>

ClassImp(TSnCompMbed);

const Char_t* TSnCompMbed::kCompNm  = "mbed";

Bool_t TSnCompMbed::IsEqual(const TObject* obj) const {
   if (obj!=0) {
      if (obj->IsA() == TSnCompMbed::Class()) {
         const TSnCompMbed* mb = static_cast<const TSnCompMbed*>(obj);
         return (0==fMac.CompareTo(mb->GetMAC()));
      }
   }
   return kFALSE;
}

ULong64_t TSnCompMbed::GetMACValFromStr(const TString macstr) {
   const Int_t len = macstr.Length();
   ULong64_t m=0;
   if (len==12) {
      sscanf(macstr.Data(),"%012llX", &m);
      m<<=16;
   } else {
      throw std::runtime_error(Form("<TSnCompMbed::GetMACVal>: "
         "Unexpected length (%d) of mac address string [%s].",
         len, macstr.Data()));
   }
   return m;
}

const Char_t* TSnCompMbed::GetMACFromVal(const ULong64_t m) {
   // Note: uses a static character array, so don't call this
   // function more than once in the same line
   static Char_t ma[20];
   sprintf(ma,"%012llX",m>>16);
   return ma;
}

