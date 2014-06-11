#include "TSnRunInfo.h"

#include "TSnDAQConfig.h"
#include "NSnHashUtils.h"

#include <stdexcept>

ClassImp(TSnRunInfo);
ClassImp(TSnRunInfo::TObsoletePars);

void TSnRunInfo::ResetName() {
  TString lbl(GetConfLabel());
  lbl.ReplaceAll(" ", "_");
  TString sl(GetStationLabel());
  sl.ReplaceAll(" ", "_");
  SetName(Form("Run_%s_%s_%u",lbl.Data(),sl.Data(),GetRun()));
}

Bool_t TSnRunInfo::IsValidForDb() const {
   Bool_t
   ok  = fConfLabel.Length()>0;
   ok &= fConfLabel.Length()<TSnDAQConfig::kMaxLblLen;
   ok &= fStationLabel.Length()>0;
   return ok;
}

Bool_t TSnRunInfo::IsEqual(const TObject* obj) const {
   if (obj!=0) {
      if (obj->IsA() == TSnRunInfo::Class()) {
         const TSnRunInfo* r = static_cast<const TSnRunInfo*>(obj);
         Bool_t same = (0==fConfLabel.CompareTo(r->GetConfLabel()));
         if (same) {
            same = (0==fStationLabel.CompareTo(r->GetStationLabel()));
            if (same) {
               return (fRun==r->GetRun());
            }
         }
      }
   }
   return kFALSE;
}

ULong_t TSnRunInfo::Hash() const {
   ULong_t h = fConfLabel.Hash();
   NSnHashUtils::HashCombine(h, fStationLabel);
   NSnHashUtils::HashCombine(h, TString::Hash(&fRun, sizeof(fRun)));
   return h;
}

void TSnRunInfo::Print(Option_t* option) const {
   
   Printf("TSnRunInfo:");
   Printf("   Run        = %u", fRun);
   Printf("   First Seq  = %hu", fFirstSeq);
   Printf("   Evts / Seq = %u", fEvtsPerSeq);
   Printf("   Station    = %s", fStationLabel.Data());
   Printf("   Config     = %s", fConfLabel.Data());
   if (HasObsParams()) {
      Printf("   (Obsolete) First Evt = %u", GetObsPars().fFirstEvt);
   }
}

const TSnRunInfo::TObsoletePars& TSnRunInfo::GetObsPars() const {
   if (HasObsParams()==kFALSE) {
      throw std::runtime_error("<TSnRunInfo::GetObsPars>: "
                               "No obsolete parameters available.");
   }
   return fObs;
}

void TSnRunInfo::SetObsFirstEvt(const UInt_t f) {
   // sets both the obsolete parameter "FirstEvt" and sets
   // the corresponding FirstSeq by calculating the sequence that
   // FirstEvt would fall into
   
   fHasObs = kTRUE;
   fObs.fFirstEvt = f;
   fFirstSeq = static_cast<UShort_t>(static_cast<Float_t>(f)
                                     / static_cast<Float_t>(fEvtsPerSeq));
}
