#include "TSnRejectEarlyEvtsMod.h"

#include <TTimeStamp.h>

#include "TSnClockSetInfo.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"

ClassImp(TSnRejectEarlyEvtsMod);

void TSnRejectEarlyEvtsMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kCSrBrNm, fStart);
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnRejectEarlyEvtsMod::Process() {
   LoadBranch(TSnRawTreeMaker::kCSrBrNm);
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (fHdr!=0) {
      if (fStart!=0) { // don't bother if we couldn't get the trig start
         
         // abs event time
         TTimeStamp abstime = TSnClockSetInfo::CalcAbsTime( *fStart, *fHdr );
         const TTimeStamp& starttm = fStart->CalcAbsCurrTime();
         if ( (abstime.AsDouble() - starttm.AsDouble()) < fEarlyTime ) {
            SkipEvent();
         }
         
      } else {
         SendError(kAbortAnalysis, "Process",
                   "Could not get trigger start clock frame! "
                   "Was TSnConfigTreeLoader added to the selector?");
      }
   } else {
      SendError(kAbortAnalysis, "Process",
                "Could not get event header!");
   }
   
}
