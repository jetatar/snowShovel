#include "TSnRejectBadClocksMod.h"

#include <TTimeStamp.h>

#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnRejectBadClocksMod);

const UInt_t TSnRejectBadClocksMod::kPrintRejected = 10;

void TSnRejectBadClocksMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnRejectBadClocksMod::CheckTime(const Double_t t) {
   
   Bool_t outsideAll = kTRUE;
   
   VecTimeWin_t::const_iterator w, badw, end=fTimeWins.end();
   for (w=fTimeWins.begin(); (w!=end) && outsideAll; ++w) {
      
      const Double_t& min = w->first,
                    & max = w->second;
      const Bool_t isOutside = (min>t) || (t>max);
      outsideAll &= isOutside;
      
   }

   // it was inside at least one time window.
   if ( ((outsideAll==kFALSE) && IsRejectingInsideAnyTimeWin()) ||
        ( outsideAll          && IsRejectingOutsideAllTimeWins()) ) {

      if (GetVerbosity()>=kPrintRejected) {
         TTimeStamp tmin(static_cast<UInt_t>(badw->first),
                         badw->first - static_cast<UInt_t>(badw->first));
         TTimeStamp tmax(static_cast<UInt_t>(badw->second),
                         badw->second - static_cast<UInt_t>(badw->second));
         TTimeStamp ts(static_cast<UInt_t>(t),
                       t - static_cast<UInt_t>(t));
         Warning("CheckTime","Time [%s] out of range "
                 "(%s, %s).",
                 ts.AsString("s"),
                 tmin.AsString("s"),
                 tmax.AsString("s"));
      }
      SkipEvent();

   }
}

void TSnRejectBadClocksMod::Process() {
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (0!=fHdr) {
      CheckTime(fHdr->GetTime().AsDouble());
   } else {
      SendError(kAbortModule, "Process",
                "Could not load event header.");
   }

}

void TSnRejectBadClocksMod::Print(Option_t* option) const {
   Printf("TSnRejectBadClocksMod:");
   Printf("  Rejecting events [%s]",
          fRejOutliers ? "outside" : "inside");
   Printf("  these [%u] time windows:", GetNumTimeWins());
   VecTimeWin_t::const_iterator w, end=fTimeWins.end();
   for (w=fTimeWins.begin(); w!=end; ++w) {
      Printf("    (%13.3f, %13.3f)", w->first, w->second);
   }
}

