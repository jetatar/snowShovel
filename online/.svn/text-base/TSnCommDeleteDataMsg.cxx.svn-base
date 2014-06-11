#include "TSnCommDeleteDataMsg.h"

#include <TClass.h>

#include "TSnIOBuffer.h"
#include "TSnIOObject.h"

#include <stdexcept>

ClassImp(TSnCommDeleteDataMsg);

const Char_t* TSnCommDeleteDataMsg::kMagicString = 
   "fj2io32FIJ(#jd;;.O@++/]ewavk2[49ojv";

UInt_t TSnCommDeleteDataMsg::GetIOByteArraySize(const Bool_t withHeader) const {
   if (withHeader==kFALSE) {
      throw std::runtime_error("<TSnCommDeleteDataMsg::GetIOByteArraySize>: "
                               "IO byte array for protocol message always has "
                               "headers.");
   }
   const UInt_t s = TSnCommProtocolMsg::GetIOByteArraySize(withHeader);
   static const size_t msl = strlen(kMagicString);
   return (s+msl);
}

UInt_t TSnCommDeleteDataMsg::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   const UInt_t ob = b.GetPosVal();
   TSnCommProtocolMsg::WriteIOByteArrayTo(b);
   static const size_t msl = strlen(kMagicString);
   TSnIOObject::WriteStrTo(b, kMagicString, msl);
   return (b.GetPosVal() - ob);
}

Bool_t TSnCommDeleteDataMsg::IsEqual(const TObject* obj) const {
   Bool_t ok=kFALSE;
   if (obj!=0) {
      if (obj->IsA() == TSnCommDeleteDataMsg::Class()) {
         const TSnCommDeleteDataMsg* p =
            static_cast<const TSnCommDeleteDataMsg*>(obj);
         ok  = TSnCommProtocolMsg::IsEqual(p);
      }
   }
   return ok;
}

