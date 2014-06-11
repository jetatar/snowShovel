#include "TSnTriggerSetup.h"

#include <TObjString.h>
#include <TString.h>
#include <TParameter.h>
#include <TMap.h>

#include <stdexcept>
#include <algorithm>

ClassImp(TSnTriggerSetup);

const Char_t*  TSnTriggerSetup::kMajLogTrgType  = "majority logic";

TSnTriggerSetup::TSnTriggerSetup(const TSnTriggerSetup& t) :
   fTrgLog(0), fThrotl(t.GetThrottlePeriodms()), 
   fForced(t.GetForcedPeriod()), fThmOn(t.IsThermalTriggering()) {
   // copy ctor
   // copy the contents of the map
   const TMap* m = t.GetTrigLogic();
   if (m!=0) {
      TIter ni(m->MakeIterator());
      const TObject* k=0;
      const TParameter<UShort_t>* p;
      while ( (k=ni())!=0 ) {
         p = dynamic_cast<const TParameter<UShort_t>*>(m->GetValue(k));
         SetTrigLogic(k->GetName(), p->GetVal());
      }
   }
}

void TSnTriggerSetup::Swap(TSnTriggerSetup& a, TSnTriggerSetup& b) {
   std::swap(a.fTrgLog, b.fTrgLog);
   std::swap(a.fThrotl, b.fThrotl);
   std::swap(a.fForced, b.fForced);
   std::swap(a.fThmOn,  b.fThmOn);
}

TSnTriggerSetup::~TSnTriggerSetup() {
   // dtor
   // delete everything in the trigger logic map, and delete the map
   if (fTrgLog!=0) {
      fTrgLog->DeleteAll();
   }
   delete fTrgLog;
}

Bool_t TSnTriggerSetup::IsValidForDb() const {
   Bool_t
   ok  = (fTrgLog!=0) && (fTrgLog->IsEmpty()!=kTRUE);
   ok &= (fForced>=0.0);
   return ok;
}

UChar_t TSnTriggerSetup::GetNumCardsMajLog() const {
   const UShort_t n = GetTrigLogic(kMajLogTrgType);
   if (n>=kMaxUChar) {
      throw std::out_of_range(Form("<TSnIOConfigFrame::WriteTo>: "
         "Trigger logic value [%hu] too big for majority logic.",
         n));
   }
   return static_cast<UChar_t>(n);
}

UShort_t TSnTriggerSetup::GetTrigLogic(const Char_t* type) const {
   
   const TParameter<UShort_t>* val = 0;
   if (fTrgLog!=0) {
      val = 
         dynamic_cast< const TParameter<UShort_t>* >(fTrgLog->GetValue(type));
   }
   
   if (val!=0) {
      return val->GetVal();
   } else {
      throw std::out_of_range(Form("<TSnDAQConfig::GetTrigLogic>: "
         "No trigger logic of type [%s] in logic collection.",
         type));
   }
   return ~0; // all bits 1 should be recognizably bad (0 might be ok)
   
}


void TSnTriggerSetup::SetTrigLogic(const Char_t* type, const UShort_t val) {
   
   // using a TMap instead of just a THashTable because TParameter does
   // not (currently) supply a Hash function
   if (fTrgLog==0) {
      fTrgLog = new TMap;
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
      fTrgLog->SetOwner();
#else
      fTrgLog->SetOwnerKeyValue();
#endif
   }
   
   TParameter<UShort_t>* pv =
      dynamic_cast< TParameter<UShort_t>* >(fTrgLog->GetValue(type));
   if (pv!=0) {
      // already in map
      pv->SetVal(val);
   } else {
      // add to map
      pv = new TParameter<UShort_t>(type, val);
      fTrgLog->Add(new TObjString(type), pv);
   }
}


void TSnTriggerSetup::Print(Option_t* option) const {
   TIter tl(fTrgLog);
   const TObject* tlk;
   Printf(     "  trigger logic:");
   while ( (tlk = tl())!=0 ) {
     Printf("    %s = %hu", tlk->GetName(),
         dynamic_cast< const TParameter<UShort_t>* >(
	    fTrgLog->GetValue(tlk))->GetVal());
   }
   Printf("  trig throttle = %hu", fThrotl);
   Printf("  forced period = %g", fForced);
   Printf("  thermal trigs = %s", (fThmOn ? "true" : "false"));
}

