#include "TSnRejectBadClocksMod.h"

#include <TTimeStamp.h>

#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnRejectBadClocksMod);

void TSnRejectBadClocksMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnRejectBadClocksMod::CheckTime(const Double_t t) 
{
    if ( ((t<fMinTime) || (t>fMaxTime)) && bInclude ) 
    {
        if (GetVerbosity()>10) 
        {
            TTimeStamp min(static_cast<UInt_t>(fMinTime),
                        fMinTime-static_cast<UInt_t>(fMinTime));
            TTimeStamp max(static_cast<UInt_t>(fMaxTime),
                        fMaxTime-static_cast<UInt_t>(fMaxTime));
            TTimeStamp ts(static_cast<UInt_t>(t),
                       t - static_cast<UInt_t>(t));
            Warning("CheckTime","Time [%s] out of range "
                     "(%s, %s).",
                     ts.AsString("s"),
                     min.AsString("s"),
                     max.AsString("s"));
        }
      return SkipEvent();
    }
    if( ((t >= fMinTime) && (t <= fMaxTime)) && !bInclude )
    {
        if (GetVerbosity()>10) 
        {
            TTimeStamp min(static_cast<UInt_t>(fMinTime),
                        fMinTime-static_cast<UInt_t>(fMinTime));
            TTimeStamp max(static_cast<UInt_t>(fMaxTime),
                        fMaxTime-static_cast<UInt_t>(fMaxTime));
            TTimeStamp ts(static_cast<UInt_t>(t),
                       t - static_cast<UInt_t>(t));
            Warning("CheckTime","Time [%s] out of range "
                     "(%s, %s).",
                     ts.AsString("s"),
                     min.AsString("s"),
                     max.AsString("s"));
        }

        return SkipEvent( );
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

