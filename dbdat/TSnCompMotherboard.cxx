#include "TSnCompMotherboard.h"

#include <TTimeStamp.h>

#include "TSnCompFpga.h"

#include <stdexcept>

ClassImp(TSnCompMotherboard);

const Char_t* TSnCompMotherboard::kCompNm = "motherboard";

TSnCompMotherboard::~TSnCompMotherboard() {
   delete fDateCnstr;
}

Bool_t TSnCompMotherboard::IsValidForDb() const {
   Bool_t ok = fNum>=0;
   ok &= fFpga.IsValid();
   if (ok) {
      ok &= GetFpga().IsValidForDb();
      ok &= fModel.Length()>0;
   }
   return ok;
}

void TSnCompMotherboard::SetDateConstructed(const TTimeStamp& d) {
   delete fDateCnstr;
   fDateCnstr = new TTimeStamp(d);
}

const TSnCompFpga& TSnCompMotherboard::GetFpga() const {
   const TSnCompFpga* o=0;
   if (fFpga.IsValid()) {
      o = dynamic_cast<const TSnCompFpga*>(fFpga.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompMotherboard::GetFpga>: "
         "Request for empty FPGA object.");
   }
   return *o;
}

TSnCompFpga& TSnCompMotherboard::GetFpga() {
   TSnCompFpga* o=0;
   if (fFpga.IsValid()) {
      o = dynamic_cast<TSnCompFpga*>(fFpga.GetObject());
   }
   if (o==0) {
      throw std::runtime_error("<TSnCompMotherboard::GetFpga>: "
         "Request for empty FPGA object.");
   }
   return *o;
}

void TSnCompMotherboard::SetFpga(TSnCompFpga& fpga) {
   fFpga = &fpga;
}

