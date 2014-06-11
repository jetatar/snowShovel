#include "TSnNetAdrSet.h"

#include <TString.h>

#include <cstring>

ClassImp(TSnNetAdrSet);

const UChar_t TSnNetAdrSet::kIPLen;         // length of IP address strings
const Char_t* TSnNetAdrSet::kIPDEF = "DEF"; // default ip string

TSnNetAdrSet::TSnNetAdrSet() :
   fRport(0) {
   // default ctor. server, ip, mask and gate all get the default value
   // from kIPDEF. the port defaults to 0.
   SetRemoteServer(kIPDEF);
   SetLocalIP(kIPDEF);
   SetLocalMask(kIPDEF);
   SetLocalGateway(kIPDEF);
   ResetName();
}

TSnNetAdrSet::TSnNetAdrSet(const Char_t* rserv, const UShort_t rport,
                           const Char_t* locip, const Char_t* locmask,
                           const Char_t* locgate) :
   fRport(rport) {
   // normal ctor. copy the strings, but not more than kIPLen-1 (1 for
   // the termination char)
   SetRemoteServer(rserv);
   SetLocalIP(locip);
   SetLocalMask(locmask);
   SetLocalGateway(locgate);
   ResetName();
}

void TSnNetAdrSet::ResetName() {
   SetName(Form("NetAdrSet_%s",GetLocalIP()));
}

ULong_t TSnNetAdrSet::Hash() const { 
   return TString::Hash(fLocIp, kIPLen*sizeof(Char_t));
}


void TSnNetAdrSet::SetRemoteServer(const Char_t* rs) {
   memset(fRserv, 0, kIPLen*sizeof(Char_t));
   strncpy(fRserv, rs, kIPLen-1);
}

void  TSnNetAdrSet::SetLocalIP(const Char_t* s) {
   memset(fLocIp, 0, kIPLen*sizeof(Char_t));
   strncpy(fLocIp, s, kIPLen-1);
   ResetName();
}

void TSnNetAdrSet::SetLocalMask(const Char_t* s) {
   memset(fLocMask, 0, kIPLen*sizeof(Char_t));
   strncpy(fLocMask, s, kIPLen-1);
}

void TSnNetAdrSet::SetLocalGateway(const Char_t* s) {
   memset(fLocGate, 0, kIPLen*sizeof(Char_t));
   strncpy(fLocGate, s, kIPLen-1);
}

Bool_t TSnNetAdrSet::IsEqual(const TObject* obj) const {
   if (obj!=0) {
      if (obj->IsA() == TSnNetAdrSet::Class()) {
         const TSnNetAdrSet* na = static_cast<const TSnNetAdrSet*>(obj);
         if (fRport == na->GetRemotePort()) {
            Bool_t same = (0==strcmp(fRserv,na->GetRemoteServer()));
            if (same) {
               same = (0==strcmp(fLocIp,na->GetLocalIP()));
               if (same) {
                  same = (0==strcmp(fLocMask,na->GetLocalMask()));
                  if (same) {
                     return (0==strcmp(fLocGate,na->GetLocalGateway()));
                  }
               }
            }
         }
      }
   }
   return kFALSE;
}

void TSnNetAdrSet::Print(Option_t* option) const {
   Printf("TSnNetAdrSet:");
   Printf("  Remote Serv = %s", fRserv);
   Printf("  Remote Port = %hu", fRport);
   Printf("  Local IP    = %s", fLocIp);
   Printf("  Local Mask  = %s", fLocMask);
   Printf("  Local Gate  = %s", fLocGate);
}
