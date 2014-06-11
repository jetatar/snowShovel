#include "TSnDAQConfig.h"

#include "TSnMath.h"

#include <sstream>
#include <stdexcept>

ClassImp(TSnDAQConfig);
ClassImp(TSnDAQConfig::TObsoletePars);

const Int_t    TSnDAQConfig::kMaxLblLen      = 64;

void TSnDAQConfig::ResetName() {
   TString lbl(GetLabel());
   lbl.ReplaceAll(" ", "_");
   SetName(Form("Config_%s",lbl.Data()));
}

Bool_t TSnDAQConfig::IsEqual(const TObject* obj) const {
   // just check that the (unique!) labels are the same
   if (obj!=0) {
      if (obj->IsA() == TSnDAQConfig::Class()) {
         const TSnDAQConfig* dq = static_cast<const TSnDAQConfig*>(obj);
         return (0==fLabel.CompareTo(dq->GetLabel()));
      }
   }
   return kFALSE;
}

Bool_t TSnDAQConfig::IsValidForDb() const {
   // try to verify that all non-null parameters have been set
   // and other values are within range
   
   Bool_t
   ok  = (fLabel.Length()>0) && (fLabel.Length()<kMaxLblLen);
   ok &= fUsage.Length()>0;
   ok &= fUser.Length()>0;
   ok &= fDesc.Length()>0;
   ok &= fDacs.IsValidForDb();
   ok &= fPlas.IsValidForDb();
   ok &= fTrigSet.IsValidForDb();
   ok &= fAmpsOn.IsValidForDb();
   ok &= fAmpsOn.GetNchans() == fDacs.GetNchans();
   return ok;
}

void TSnDAQConfig::Print(Option_t* option) const {
   Printf("TSnDAQConfig: %s", fLabel.Data());
   Printf("  usage         = %s", fUsage.Data());
   Printf("  user          = %s", fUser.Data());
   Printf("  description   = %s", fDesc.Data());
   Printf("  date built    = %s", fBuilt.AsString());
   Printf("  dacs:");
   fDacs.Print(option);
   Printf("  plas:");
   fPlas.Print(option);
   Printf("  trigger setup:");
   fTrigSet.Print();
   Printf("  run mode bits = ");
   Printf("    %s", (IsSingleSeqRunMode() ? "stopping after one file" :
                     "continuous data taking"));
   Printf("    counting %s", (IsCountingPowerReadings() ? "power readings"
                              : "events"));
   //Printf("  amps on:");
   //fAmpsOn.Print(option);
   Printf("  heartbeat per = %hu", fHrtBt);
   Printf("  stream hi lo  = %s", (fStreamHiLoPlas ? "true" : "false"));
   Printf("  wv lose LSB   = %hhu", fWvLoseLSB);
   Printf("  wv lose MSB   = %hhu", fWvLoseMSB);
   Printf("  wv baseline   = %hu", fWvBaseline);
   printf("  dat pack bits : "); TSnMath::PrintBits(fDatPackType, kTRUE);
   Printf("  pow mode bits = (%hhu)", fPowMode);
   Printf("    amps data take %s", (IsPoweredFor(kAmpsDatTak) ? "ON" : "off"));
   Printf("    card data take %s", (IsPoweredFor(kCardDatTak) ? "ON" : "off"));
   Printf("    irid data take %s", (IsPoweredFor(kIridDatTak) ? "ON" : "off"));
   Printf("    afar data take %s", (IsPoweredFor(kAfarDatTak) ? "ON" : "off"));
   Printf("    amps comm wind %s", (IsPoweredFor(kAmpsComWin) ? "ON" : "off"));
   Printf("    card comm wind %s", (IsPoweredFor(kCardComWin) ? "ON" : "off"));
   Printf("    irid comm wind %s", (IsPoweredFor(kIridComWin) ? "ON" : "off"));
   Printf("    afar comm wind %s", (IsPoweredFor(kAfarComWin) ? "ON" : "off"));
   Printf("  bat volt  TO  low pwr = %hd", fBatVoltLowPwr);
   Printf("  bat volt FROM low pwr = %hd", fBatVoltFromLowPwr);
   Printf("  volt chk per  = %hu", fVoltCheckPer);
   fComWin.Print(option);
   Printf("  com dur lowp  = %u", fLPComWin.GetDuration());
   Printf("  com per lowp  = %u", fLPComWin.GetPeriod());
   Printf("  watchdog time = %u", fWchDogPer);
   if (HasObsParams()) {
      Printf("  (Obsolete) force per           = %hu", GetObsPars().fForced);
      Printf("  (Obsolete) bat volt TO low pwr = %hd", 
             GetObsPars().fBatVoltLowPwr);
   }
}

void TSnDAQConfig::SetDateBuilt(const UInt_t tloc,
                                const Bool_t isUTC,
                                const Int_t secOffset,
                                const Bool_t dosDate) {
   fBuilt.Set(tloc, isUTC, secOffset, dosDate);
}

TString TSnDAQConfig::GetDatPackBitName(const EDatPackBit x) {
   switch (x) {
      case kSDcard:
         return "SDcard";
      case kIrid:
         return "Iridium";
      case kAfar:
         return "Afar";
      case kUSB:
         return "USB";
      default:
         throw std::runtime_error(Form("<TSnDAQConfig::GetDatPackBitName>: "
                                       "Unknown data pack bit (%#02x)",x));
   };
}


const TSnDAQConfig::TObsoletePars& TSnDAQConfig::GetObsPars() const {
   if (HasObsParams()==kFALSE) {
      throw std::runtime_error("<TSnDAQConfig::GetObsPars>: "
                               "No obsolete parameters available.");
   }
   return fObs;
}

void TSnDAQConfig::SetObsForcedPeriodUShort(const UShort_t f) {
   // set the forced period using an obsolete integral value.
   // sets both the obsolete parameter as well as the forced period
   // of this config
   
   fHasObs = kTRUE;
   fObs.fForced = f;
   SetForcedPeriod(static_cast<Float_t>(f));
}

void TSnDAQConfig::SetObsBatVoltToLowPwrShort(const Short_t v) {
   // set the v_in at which to switch to low power using the
   // obsolete Short_t type (it's now UShort_t)
   //
   // sets both the obsolete parameter as well as the BatVoltToLowPwr
   
   fHasObs = kTRUE;
   fObs.fBatVoltLowPwr = v;
   SetBatVoltToLowPwr(v);
}
