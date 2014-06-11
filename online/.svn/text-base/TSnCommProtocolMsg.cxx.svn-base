#include "TSnCommProtocolMsg.h"

#include <TString.h>
#include <TClass.h>

#include "TSnIOBuffer.h"
#include "TSnIOHeaderFrame.h"
#include "TSnIOStatusFrame.h"

#include <cstring>
#include <stdexcept>

ClassImp(TSnCommProtocolMsg);

UInt_t TSnCommProtocolMsg::GetIOByteArraySize(const Bool_t withHeader) const {
   if (withHeader==kFALSE) {
      throw std::runtime_error("<TSnCommProtocolMsg::GetIOByteArraySize>: "
                               "IO byte array for protocol message always has "
                               "headers.");
   }
   return TSnIOHeaderFrame::SizeOf();
}

UInt_t TSnCommProtocolMsg::WriteIOByteArrayTo(TSnIOBuffer& b) const {
   const UInt_t ob = b.GetPosVal();
   TSnIOHeaderFrame::WriteTo(b, fMsgCode, fMsgPayl);
   return (b.GetPosVal() - ob);
}

Bool_t  TSnCommProtocolMsg::IsEqual(const TObject* obj) const {
   Bool_t ok=kFALSE;
   if (obj!=0) {
      if (obj->IsA() == TSnCommProtocolMsg::Class()) {
         const TSnCommProtocolMsg* p =
            static_cast<const TSnCommProtocolMsg*>(obj);
         ok  = (fMsgCode == p->fMsgCode);
         ok &= (fMsgPayl == p->fMsgPayl);
      }
   }
   return ok;
}

ULong_t TSnCommProtocolMsg::Hash() const { 
   return GetLabel().Hash(); 
}

TString TSnCommProtocolMsg::GetLabel() const {
   TString l(Form("ProtocolMsg_%02x_%u",fMsgCode,fMsgPayl));
   return l;
}
   
void   TSnCommProtocolMsg::Print(Option_t* option) const {
   Printf("TSnCommProtocolMsg:");
   TString mc(TSnIOHeaderFrame::GetCodeAsString(fMsgCode));
   Printf("  code    = %02x (%s)",fMsgCode,mc.Data());
   Printf("  payload = %x (%u)",fMsgPayl,fMsgPayl);
}

Bool_t TSnCommProtocolMsg::IsOkForStatusVersion(const Int_t sv) const {
   
   /* a season stack goes here
    * if (sv > 2018to2019IOvers) {
    * } else if (sv > 2017to2018IOvers) {
    *  ...
    * } else if (sv > 2012to2013IOvers) {
    * } else if (sv == 2012to2013IOvers) {
    * } else {
    *    prior to any season
    */
   if (sv > TSnIOStatusFrame::k2012to2013IOVers) {
      return kTRUE;
   } else {
      // some protocols weren't added yet
      // (here we don't distinguish between 2012-2013 and earlier)
      switch (fMsgCode) {
         case TSnIOHeaderFrame::kFileTrgStrtCode:
         case TSnIOHeaderFrame::kFileTrgStopCode:
         case TSnIOHeaderFrame::kHnShOkReqRnCode:
         case TSnIOHeaderFrame::kHnShDemandCode:
         case TSnIOHeaderFrame::kHnShNoReplyCode:
         case TSnIOHeaderFrame::kSignalStrCode:
            return kFALSE;
         default:
            return kTRUE;
      };
   }
}

