#include "TSnRejectEarlyEvtsMod.h"

#include <TTimeStamp.h>

#include "TSnClockSetInfo.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnRunInfo.h"

ClassImp(TSnRejectEarlyEvtsMod);

void TSnRejectEarlyEvtsMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kCSrBrNm, fStart);
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   if ( IsRejectingOnlyFirstEvent() ) {
      ReqBranch(TSnRawTreeMaker::kCMtBrNm, fConfMeta);
      ReqBranch(TSnRawTreeMaker::kRunBrNm, fRunInfo);
   }
}

Bool_t TSnRejectEarlyEvtsMod::IsFirstEvtOfSeq() const {
   // check whether the current event is the first of the sequence
   //
   // this exploits the fact that the (current) MBED code sets
   // the first event number to (seq num * evts per file)
   // see ResetCountersClearEvt() in main.cpp of the MBED code.
   //
   // NOTE: if the MBED code changes, this function will become invalid!
   
   const UInt_t estart = fConfMeta->GetSeqNum() *
      fRunInfo->GetEvtsPerSeq();
   return (fHdr->GetEvtNum() == estart);

}

void TSnRejectEarlyEvtsMod::Process() 
{
    LoadBranch(TSnRawTreeMaker::kCSrBrNm);
    LoadBranch(TSnRawTreeMaker::kEHdBrNm);

    if (fHdr!=0) 
    {
        if (fStart!=0)   // don't bother if we couldn't get the trig start
        { 
         
            // abs event time
            TTimeStamp abstime = TSnClockSetInfo::CalcAbsTime( *fStart, *fHdr );
            const TTimeStamp& starttm = fStart->CalcAbsCurrTime();

            if ( (abstime.AsDouble() - starttm.AsDouble()) < fEarlyTime ) 
            {          
                if( IsRejectingOnlyFirstEvent() ) 
                {
                    LoadBranch(TSnRawTreeMaker::kRunBrNm);
                    LoadBranch(TSnRawTreeMaker::kCMtBrNm);

                    if ((fRunInfo!=0) && (fConfMeta!=0)) 
                    {
                        if ( IsFirstEvtOfSeq() ) 
                        {
                            SkipEvent();
                        }
                    } 
                    else 
                    {
                        SendError(kAbortAnalysis, "Process",
                            "Could not get run/config info for this sequence! "
                            "Was TSnConfigTreeLoader added to the selector?");
                    }
                } 

                else 
                {
                    SkipEvent();
                }
            }
        } 
        else 
        {
            SendError(kAbortAnalysis, "Process",
                   "Could not get trigger start clock frame! "
                   "Was TSnConfigTreeLoader added to the selector?");
        }
    } 
    else 
    {
        SendError(kAbortAnalysis, "Process",
                "Could not get event header!");
    }
}

