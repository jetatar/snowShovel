#include "TSnCalcAbsTimeMod.h"

#include <TParameter.h>

#include "TSnClockSetInfo.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"

ClassImp(TSnCalcAbsTimeMod);

TSnCalcAbsTimeMod::TSnCalcAbsTimeMod(const Char_t* name) :
   TAModule(name, "add the absolute time of the event to the obj events"),
   fAETimeNm("AbsEvtTime"),
   fStart(0), fHdr(0), fPrevStart(new TSnClockSetInfo) {
   
   // set to something crazy so it gets reset on the first event
   fPrevStart->SetCurrTime( TTimeStamp(0) );
}

TSnCalcAbsTimeMod::~TSnCalcAbsTimeMod() {
   delete fPrevStart;
}

void TSnCalcAbsTimeMod::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kCSrBrNm, fStart);
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnCalcAbsTimeMod::Process() {
   LoadBranch(TSnRawTreeMaker::kCSrBrNm);
   if (fStart!=0) {
      if (fStart->GetCurrTime().GetSec() != fPrevStart->GetCurrTime().GetSec()) {
         // load the new trigger start
         fPrev      =  fStart->CalcAbsCurrTime();
        *fPrevStart = *fStart;
      } // else it's the same, don't need to do anything
   } else {
      if (GetVerbosity()>10) {
         SendError(kWarning, "Process",
                   "Could not get trigger start clock.");
      }
   }
   
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (fHdr!=0) {
      if (fStart!=0) { // don't bother if we couldn't get the trig start
         
         // wrap the double in a TParameter so we can add it to the object
         // list of this event
         TTimeStamp abstime = TSnClockSetInfo::CalcAbsTime( fPrev, *fHdr );
         TParameter<Double_t>* at = 
            new TParameter<Double_t>(GetAbsEvtTimeName(),
                                     abstime.AsDouble());
         AddObjThisEvt(at); // the selector will delete it for us
         
         fPrev = abstime;
      } // else we cannot calculate the absolute time, so don't pretend that
        // we can. modules will have to deal with the missing object if
        // they are able.
   } else {
      SendError(kAbortModule, "Process",
                "Could not get event header");
   }
}
