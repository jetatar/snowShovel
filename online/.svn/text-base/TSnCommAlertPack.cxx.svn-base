#include "TSnCommAlertPack.h"

#include <TROOT.h>
#include <TClass.h>

#include "TSnIOObject.h"
#include "TSnIOBuffer.h"

#include <iostream>
#include <algorithm>

ClassImp(TSnCommAlertPack);

void TSnCommAlertPack::Swap(TSnCommAlertPack& a, TSnCommAlertPack& b) {
   std::swap(a.fLevel,   b.fLevel);
   std::swap(a.fComm,    b.fComm);
   std::swap(a.fSource,  b.fSource);
   std::swap(a.fSubject, b.fSubject);
   std::swap(a.fMessage, b.fMessage);
}

void TSnCommAlertPack::Print(Option_t *option) const {
   Printf("Source:  [%s]",fSource.Data());
   Printf("Subject: [%s]",fSubject.Data());
   Printf("Message: [%s]",fMessage.Data());
   if (fComm!=0) {
      fComm->Print(option);
   } else {
      Printf("No comm object.");
   }
}

UInt_t TSnCommAlertPack::GetIOByteArraySize(const Bool_t withHeader) const {
   if (fComm!=0) {
      return fComm->GetIOByteArraySize(withHeader);
   } else {
      throw std::runtime_error(Form("<TSnCommAlertPack::GetIOByteArraySize>: "
                                    "No comm object."));
   }
   return 0;
}

UInt_t TSnCommAlertPack::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   if (fComm!=0) {
      return fComm->WriteIOByteArrayTo(b);
   } else {
      throw std::runtime_error(Form("<TSnCommAlertPack::WriteIOByteArrayTo>: "
                                    "No comm object."));
   }
   return 0;
}

TSnIOBuffer TSnCommAlertPack::GetIOByteArray() const {
   if (fComm!=0) {
      return fComm->GetIOByteArray();
   } else {
      throw std::runtime_error(Form("<TSnCommAlertPack::GetIOByteArray>: "
                                    "No comm object."));
   }
   TSnIOBuffer b;
   return b;
}
   
Bool_t  TSnCommAlertPack::IsEqual(const TObject* obj) const {
   Bool_t same(kFALSE);
   if (obj!=0) {
      if (obj->IsA() == TSnCommAlertPack::Class()) {
         const TSnCommAlertPack* cf = static_cast<const TSnCommAlertPack*>(obj);
         same  =  (fSource.CompareTo(cf->fSource)==0);
         same &= (fSubject.CompareTo(cf->fSubject)==0);
         same &= (fMessage.CompareTo(cf->fMessage)==0);
         if (fComm!=0) {
            same &= fComm->IsEqual(cf->fComm);
         } else {
            same &= ((cf->fComm)==0);
         }
      }
   }
   return same;
}

Bool_t TSnCommAlertPack::IsOkForStatusVersion(const Int_t sv) const {
   if (fComm!=0) {
      return fComm->IsOkForStatusVersion(sv);
   } else {
      return kTRUE;
   }
}


