#include "TSnBasicEvtSelMod.h"

#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"
#include "TSnRawWaveform.h"

ClassImp(TSnBasicEvtSelMod);

TSnBasicEvtSelMod::TSnBasicEvtSelMod(const Char_t* name,
                                     const UShort_t bits,
                                     const UShort_t rejBits) :
   TAModule(name, "Basic event selection module"),
   fHdrBrNm(TSnRawTreeMaker::kEHdBrNm),
   fEWvBrNm(TSnRawTreeMaker::kEWvBrNm),
   fTbits(bits), fRbits(rejBits), fDoCRC(kTRUE), fHdr(0), fRaw(0) {
   // normal ctor
}

void TSnBasicEvtSelMod::SlaveBegin() {
   ReqBranch(fHdrBrNm.Data(), fHdr);
   ReqBranch(fEWvBrNm.Data(), fRaw);
}

void TSnBasicEvtSelMod::Process() {
   
   // first check trigger bits
   LoadBranch(fHdrBrNm.Data());
   if (0==fHdr) {
      if (GetVerbosity()>10) {
         SendError(kWarning, "Process",
                   "Could not get event header.");
      }
      return SkipEvent();
   } else if (fHdr->GetTrgInfo().IsTrig(fTbits.GetTrgBits())==kFALSE) {
      if (GetVerbosity()>10) {
         TString b = fTbits.GetBitsAsString();
         TString t = fHdr->GetTrgInfo().GetBitsAsString();
         SendError(kWarning, "Process",
                   "Trigger bits not set: "
                   "want (%s), evt has (%s)",
                   b.Data(), t.Data());
      }
      return SkipEvent();
   } else if (fHdr->GetTrgInfo().IsTrig(fRbits.GetTrgBits())) {
      if (GetVerbosity()>10) {
         TString b = fRbits.GetBitsAsString();
         TString t = fHdr->GetTrgInfo().GetBitsAsString();
         SendError(kWarning, "Process",
                   "Reject trigger bits set: "
                   "not allowing (%s), evt has (%s)",
                   b.Data(), t.Data());
      }
      return SkipEvent();
   }
   
   if (fDoCRC) {
      // trg bits ok; check CRC
      LoadBranch(fEWvBrNm.Data());
      
      if (fRaw==0) {
         if (GetVerbosity()>10) {
            SendError(kWarning, "Process",
                      "Could not get raw waveform data.");
         }
         return SkipEvent();
      } else if ( fRaw->GetStationCRC() != fRaw->GetCRC() ) {
         if (GetVerbosity()>10) {
            SendError(kWarning, "Process",
                      "Could not raw data CRC mismatch: "
                      "station=%u, local=%u",
                      fRaw->GetStationCRC(),
                      fRaw->GetCRC());
         }
         return SkipEvent(); // failed event selection
      }
   }

   // otherwise ok to process
}
