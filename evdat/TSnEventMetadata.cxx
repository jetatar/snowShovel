#include "TSnEventMetadata.h"

#include <TClass.h>

#include <stdexcept>

ClassImp(TSnEventMetadata);

Bool_t TSnEventMetadata::IsEqual(const TObject* obj) const {
   if (obj!=0) {
      TClass* cls = obj->IsA();
      if (cls!=0) {
         if (cls->InheritsFrom(TSnEventMetadata::Class())) {
            // already checked type, so use static cast
            const TSnEventMetadata* em = 
               static_cast<const TSnEventMetadata*>(obj);
            const Bool_t ok = (fStnId.CompareTo(em->GetStationId())==0)
               && (fRun == em->GetRunNum())
               && (fSeq == em->GetSeqNum());
            return ok;
         }
      } else {
         throw std::runtime_error("<TSnEventMetadata::IsEqual>: "
                                  "Could not get class of obj.");
      }
   }
   return kFALSE;
}

void TSnEventMetadata::Print(Option_t* option) const {
   Printf("  station = %s", fStnId.Data());
   Printf("  run     = %u", fRun);
   Printf("  seq     = %hu", fSeq);
}
