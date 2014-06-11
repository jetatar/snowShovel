#include "TSnCompDaughtercard.h"

#include <TTimeStamp.h>

#include "TSnCompFpga.h"
#include "TSnCompDigitizer.h"

#include <stdexcept>

ClassImp(TSnCompDaughtercard);

const Char_t* TSnCompDaughtercard::kCompNm = "daughtercard";

TSnCompDaughtercard::~TSnCompDaughtercard() {
  delete fDateCnstr;
}

Bool_t TSnCompDaughtercard::IsValidForDb() const {
   Bool_t ok = fNum>=0;
   ok &= fFpga.IsValid();
   ok &= fDigi.IsValid();
   if (ok) {
      ok &= GetFpga().IsValidForDb();
      ok &= GetDigitizer().IsValidForDb();
      ok &= fModel.Length()>0;
   }
   return ok;
}

void TSnCompDaughtercard::SetDateConstructed(const TTimeStamp& d) {
   delete fDateCnstr;
   fDateCnstr = new TTimeStamp(d);
}

const TSnCompFpga& TSnCompDaughtercard::GetFpga() const {
   const TSnCompFpga* o=0;
   if (fFpga.IsValid()) {
      o = dynamic_cast<const TSnCompFpga*>(fFpga.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompDaughtercard::GetFpga>: "
         "Request for empty FPGA object.");
   }
   return *o;
}

const TSnCompDigitizer& TSnCompDaughtercard::GetDigitizer() const {
   const TSnCompDigitizer* o=0;
   if (fDigi.IsValid()) {
      o = dynamic_cast<const TSnCompDigitizer*>(fDigi.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompDaughtercard::GetDigitizer>: "
         "Request for empty digitizer object.");
   }
   return *o;
}

TSnCompFpga& TSnCompDaughtercard::GetFpga() {
   TSnCompFpga* o=0;
   if (fFpga.IsValid()) {
      o = dynamic_cast<TSnCompFpga*>(fFpga.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompDaughtercard::GetFpga>: "
         "Request for empty FPGA object.");
   }
   return *o;
}

TSnCompDigitizer& TSnCompDaughtercard::GetDigitizer() {
   TSnCompDigitizer* o=0;
   if (fDigi.IsValid()) {
      o = dynamic_cast<TSnCompDigitizer*>(fDigi.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompDaughtercard::GetDigitizer>: "
         "Request for empty digitizer object.");
   }
   return *o;
}

void TSnCompDaughtercard::SetFpga(TSnCompFpga& fpga) {
   fFpga = &fpga;
}

void TSnCompDaughtercard::SetDigitizer(TSnCompDigitizer& d) {
   fDigi = &d;
}
