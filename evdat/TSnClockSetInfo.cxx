#include "TSnClockSetInfo.h"

#include <TClass.h>
#include <TMath.h>

#include "TSnEventHeader.h"

#include <stdexcept>
#include <algorithm>

ClassImp(TSnClockSetInfo);

void TSnClockSetInfo::Swap(TSnClockSetInfo& a, TSnClockSetInfo& b) {
   std::swap(a.fPrevTime, b.fPrevTime);
   std::swap(a.fSetTime,  b.fSetTime);
   std::swap(a.fCurrTime, b.fCurrTime);
   std::swap(a.fUSsinceSet, b.fUSsinceSet);
   std::swap(a.fAbsCache, b.fAbsCache);
}

void TSnClockSetInfo::Copy(TObject& object) const {
   // provided for use from python
   if (object.IsA()->InheritsFrom(TSnClockSetInfo::Class())) {
      dynamic_cast<TSnClockSetInfo&>(object) = *this;
   } else {
      object = *this;
   }
}

void TSnClockSetInfo::Print(Option_t* option) const {
   Printf("TSnClockSetInfo: ");
   Printf("  prev time    = %s", fPrevTime.AsString("c"));
   Printf("  set time     = %s", fSetTime.AsString("c"));
   Printf("  curr time    = %s", fCurrTime.AsString("c"));
   Printf("  us since set = %d (%uu)",fUSsinceSet,
          GetUSsinceSetU());
   Printf("  abs time     = %s", (fAbsCache!=0) ? fAbsCache->AsString("c")
                                                : "(nil)");
}

TTimeStamp TSnClockSetInfo::CalcAbsTime(const TTimeStamp& ref,
                                        const TSnEventHeader& hdr) {
   return CalcAbsTime( ref,
                       hdr.GetTime(),
                       static_cast<UInt_t>((hdr.GetDTms())*1000) );
}

TTimeStamp TSnClockSetInfo::CalcAbsTime(const TTimeStamp& ref,
                                        const TTimeStamp& cur,
                                        const UInt_t      dtus) {
   // given a reference time 'ref', a current time 'cur' and a counter
   // measured in microseconds 'dtus', calculate the number of times
   // the us counter would have rolled over between ref and cur, and
   // add the total us's to cur's time. cur must be later than ref.
   //
   // returns a time stamp with the same seconds as cur, but with
   // nanoseconds reflecting the us counter
   // 
   // the us timer on the mbed doesn't rollover at 0x7FFFFFFF, but at
   // 0xFFFFFFFF -- as if it were an unsigned int, which is what this
   // function accepts. the mbed uses a signed int to be consistent
   // with the Ticker interrupts, where having a sign is useful
   // (negative times => should have already called back).
   //
   // if 'ref' is equal to the epoch, that is ref.GetSec()==0,
   // then the current time is simply returned
   
   TTimeStamp nt(cur);
   if (ref.GetSec()!=0) {
      if (cur.GetSec() < ref.GetSec()) {
         throw std::runtime_error(Form("Current time (%s) must be later "
                                       "than reference time (%s)",
                                       cur.AsString(), ref.AsString()));
         
      } else {
         const Double_t rt     = ref.AsDouble();
         const Double_t ct     = cur.AsDouble();
         const Double_t dt     = (ct - rt) * 1.00e6;
         // truncate nrolls
         const Double_t nrolls = (dt<0) ? TMath::Ceil(  dt / kMaxUInt )
            : TMath::Floor( dt / kMaxUInt );
         const Double_t ns     = (((nrolls * kMaxUInt) + dtus) - dt) * 1.00e3;
         nt.SetNanoSec(ns);
      }
   }
   
   return nt;
}

