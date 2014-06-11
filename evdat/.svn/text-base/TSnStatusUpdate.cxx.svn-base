#include "TSnStatusUpdate.h"

#include <TClass.h>

#include <cstdio>
#include <stdexcept>

ClassImp(TSnStatusUpdate);
ClassImp(TSnStatusUpdate::TObsoletePars);

void TSnStatusUpdate::Copy(TObject& object) const {
   // provided for use from python
   if (object.IsA()->InheritsFrom("TSnStatusUpdate")) {
      dynamic_cast<TSnStatusUpdate&>(object) = *this;
   } else {
      object = *this;
   }
}

const Char_t* TSnStatusUpdate::GetMacAdrAsStr() const {
   // this function uses a static string, and so should not be
   // called multiple times in the same line (or in threads)
   
   static Char_t ss[32];
   sprintf(ss,"%012llX", fMacAdr >> 16); // only want 48-bit address
   return ss;
}

void TSnStatusUpdate::Print(Option_t* option) const {
  Printf("TSnStatusUpdate: ");
  Printf("  mac adr         = %llu (%s)", 
	 fMacAdr, GetMacAdrAsStr());
  Printf("  config label    = %s", fConfLabel.Data());
  Printf("  run             = %u", fRun);
  Printf("  seq             = %hu", fSeq);
  Printf("  time            = %s", fTime.AsString());
  Printf("  wv lose LSB     = %hhu", fWvLoseLSB);
  Printf("  wv lose MSB     = %hhu", fWvLoseMSB);
  Printf("  wv baseline     = %hu", fWvBaseline);
  Printf("  therm trig rate = %g", fThmRate);
  Printf("  save event rate = %g", fEvtRate);
  Printf("  free megabytes  = %g", fFreeMB);
  Printf("  watchdog reset  = %s", (fWDreset!=0)?"TRUE":"false");
  if (HasObsParams()) {
     Printf("   (Obsolete) Nfiles = %u", GetObsPars().fNfiles);
     Printf("   (Obsolete) TotKB  = %g", GetObsPars().fTotKB);
  }
}

const TSnStatusUpdate::TObsoletePars& TSnStatusUpdate::GetObsPars() const {
   if (HasObsParams()==kFALSE) {
      throw std::runtime_error("<TSnStatusUpdate::GetObsPars>: "
                               "No obsolete parameters available.");
   }
   return fObs;
}

void TSnStatusUpdate::SetObsNfiles(const UInt_t n) {
   fHasObs = kTRUE;
   fObs.fNfiles = n;
}

void TSnStatusUpdate::SetObsTotKB(const Float_t t) {
   fHasObs = kTRUE;
   fObs.fTotKB = t;
}

