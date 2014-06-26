#include "TSnCommStatusPack.h"

#include <TString.h>
#include <TROOT.h>
#include <TClass.h>

#include "TSnStatusUpdate.h"
#include "TSnPowerReading.h"
#include "TSnIOStatusFrame.h"
#include "TSnIOPowerFrame.h"
#include "TSnIOBuffer.h"

#include <stdexcept>
#include <iostream>

ClassImp(TSnCommStatusPack);
////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommStatusPack
//
// Class describing a complete status update sent by a station.
// It is composed of a TSnStatusUpdate, a TSnPowerReading and a TTimeStamp.
//
// This container owns its config components.
//
////////////////////////////////////////////////////////////////////////////////

TSnCommStatusPack::TSnCommStatusPack(const TSnStatusUpdate& s,
                                     const TSnPowerReading& p,
                                     const TTimeStamp& t)
  : fStatus(new TSnStatusUpdate(s)), fPower(new TSnPowerReading(p)),
    fTime(t), fStIOvers(-1), fPwIOvers(-1) {
}

TSnCommStatusPack::TSnCommStatusPack(const TSnCommStatusPack& s) :
   TSnCommObject(s),
   fStatus(0), fPower(0), 
   fStIOvers(s.GetStIOvers()), fPwIOvers(s.GetPwIOvers()) {
   const TSnStatusUpdate* u = s.GetStatus();
   if (u!=0) {
      fStatus = new TSnStatusUpdate(*u);
   }
   const TSnPowerReading* p = s.GetPowerReading();
   if (p!=0) {
      fPower = new TSnPowerReading(*p);
   }
   SetTime(s.GetTime());
}
   
TSnCommStatusPack::~TSnCommStatusPack() {
   delete fStatus;
   delete fPower;
}

void TSnCommStatusPack::ReadStatusFrom(const TSnIOBuffer& b) {
   delete fStatus;
   fStatus = new TSnStatusUpdate;
   fStIOvers = TSnIOStatusFrame::ReadFrom(b, *fStatus);
}

void TSnCommStatusPack::ReadPowerFrom(const TSnIOBuffer& b) {
   delete fPower;
   fPower = new TSnPowerReading;
   fPwIOvers = TSnIOPowerFrame::ReadFrom(b, *fPower);
}

void TSnCommStatusPack::SetTime(const TTimeStamp& s) {
   fTime = s;
}

UInt_t TSnCommStatusPack::GetIOByteArraySize(const Bool_t withHeader) const {
   UInt_t bs(0);
   if (withHeader) {
      if (fStatus!=0) {
         bs += TSnIOStatusFrame::SizeOf(GetStatusIOvers(),
                                        fStatus->GetConfLabelLen(),
                                        fStatus->GetWvLoseLSB(),
                                        fStatus->GetWvLoseMSB());
      }
      if (fPower!=0) {
         bs += TSnIOPowerFrame::SizeOf(GetPowerIOvers());
      }
      // time not written
   } else {
      throw std::runtime_error("<TSnCommStatusPack::GetIOByteArraySize>: "
                               "IO byte array for status pack always has "
                               "headers.");
   }
   return bs;
}

UInt_t TSnCommStatusPack::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   const UInt_t ob = b.GetPosVal();
   if (fStatus!=0) {
      TSnIOStatusFrame::WriteTo(b, *fStatus, GetStatusIOvers());
   }
   if (fPower!=0) {
      TSnIOPowerFrame::WriteTo(b, *fPower, GetPowerIOvers());
   }
   return (b.GetPosVal() - ob);
}

UChar_t TSnCommStatusPack::GetStatusIOvers() const {
   return (fStIOvers<0) ? TSnIOStatusFrame::kIOvers
      : static_cast<UChar_t>(fStIOvers);
}

UChar_t TSnCommStatusPack::GetPowerIOvers() const {
   return (fPwIOvers<0) ? TSnIOPowerFrame::kIOvers
      : static_cast<UChar_t>(fPwIOvers);
}

Bool_t TSnCommStatusPack::IsEqual(const TObject* obj) const {
   Bool_t ok=kFALSE;
   if (obj!=0) {
      if (obj->IsA() == TSnCommStatusPack::Class()) {
         const TSnCommStatusPack* s =
            static_cast<const TSnCommStatusPack*>(obj);
         ok = (fTime.GetSec() == s->fTime.GetSec());
         ok &= (GetStatusIOvers() == s->GetStatusIOvers());
         ok &= (GetPowerIOvers() == s->GetPowerIOvers());
         if (ok && (fPower!=0) && (s->fPower!=0)) {
            ok &= (fPower->GetAveV1() == s->fPower->GetAveV1());
            ok &= (fPower->GetAveV2() == s->fPower->GetAveV2());
            ok &= (fPower->GetRmsV1() == s->fPower->GetRmsV1());
            ok &= (fPower->GetRmsV2() == s->fPower->GetRmsV2());
            ok &= (fPower->GetTime()  == s->fPower->GetTime() );
         }
         if (ok && (fStatus!=0) && (s->fStatus!=0)) {
            ok &= (fStatus->GetMacAddress() == s->fStatus->GetMacAddress());
            ok &= fStatus->GetConfLabelStr().CompareTo(
               s->fStatus->GetConfLabelStr())==0;
            ok &= (fStatus->GetRunNum() == s->fStatus->GetRunNum());
            ok &= (fStatus->GetStatusTime().GetSec()
                   == s->fStatus->GetStatusTime().GetSec());
            ok &= (fStatus->GetWvLoseLSB() == s->fStatus->GetWvLoseLSB());
            ok &= (fStatus->GetWvLoseMSB() == s->fStatus->GetWvLoseMSB());
            ok &= (fStatus->GetWvBaseline() == s->fStatus->GetWvBaseline());
            ok &= (fStatus->GetSeqNum() == s->fStatus->GetSeqNum());
            ok &= (fStatus->GetThmRate() == s->fStatus->GetThmRate());
            ok &= (fStatus->GetEvtRate() == s->fStatus->GetEvtRate());
            ok &= (fStatus->GetFreeMB() == s->fStatus->GetFreeMB());
         }
      }
   }
   return ok;
}

Bool_t TSnCommStatusPack::IsOkForStatusVersion(const Int_t sv) const {
   return (sv == GetStatusIOvers());
}

void TSnCommStatusPack::Print(Option_t* option) const {
   Printf("TSnCommStatusPack:");
   TString t(fTime.AsString());
   Printf("  time = %s",t.Data());
   if (fStatus!=0) {
      Printf("  status:");
      fStatus->Print();
   } else {
      Printf("  status = N/A");
   }
   if (fPower!=0) {
      Printf("  power reading:");
      fPower->Print();
   } else {
      Printf("  power reading = N/A");
   }
}

ULong_t TSnCommStatusPack::Hash() const {
   TString x;
   x += static_cast<UInt_t>(fTime.GetSec());
   if (fStatus!=0) {
      x += fStatus->GetConfLabelStr();
      x += static_cast<UInt_t>(
         fStatus->GetStatusTime().GetSec());
   }
   if (fPower!=0) {
      x += fPower->GetAveV1();
      x += fPower->GetAveV2();
   }
   return x.Hash();
}

TString TSnCommStatusPack::GetLabel() const {
   TString l(Form("Status_%u", static_cast<UInt_t>(fTime.GetSec())));
   if (fStatus!=0) {
      l += "_";
      l += fStatus->GetMacAdrAsStr();
   }
   return l;
}
