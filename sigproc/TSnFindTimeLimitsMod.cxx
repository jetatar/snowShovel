#include "TSnFindTimeLimitsMod.h"

#include <TTimeStamp.h>
#include <TList.h>

#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnFindTimeLimitsMod);

TSnFindTimeLimitsMod::TSnFindTimeLimitsMod(const Char_t* name) :
   TAModule(name, "determine the earliest and latest event times"),
   fAETimeNm("AbsEvtTime"),
   fMinName("MinEvtTime"),
   fMaxName("MaxEvtTime"),
   fMinTime(0), fMaxTime(0), fHdr(0), fFirst(kTRUE) {
}

TSnFindTimeLimitsMod::~TSnFindTimeLimitsMod() {
   delete fMinTime;
   delete fMaxTime;
}

void TSnFindTimeLimitsMod::SlaveBegin() {
   fMinTime = new TParameter<Double_t>(GetMinTimeName(), 0);
   fMaxTime = new TParameter<Double_t>(GetMaxTimeName(), 0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,0)
   fMinTime->SetMergeMode('m'); // min
   fMaxTime->SetMergeMode('M'); // max
#endif
   AddOutput(fMinTime);
   AddOutput(fMaxTime);
   fFirst   = kTRUE;
   
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnFindTimeLimitsMod::Process() {
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (0!=fHdr) {

      Double_t at = fHdr->GetTime().AsDouble();

      // use absolute time if available
      const TParameter<Double_t>* evttime = 
         dynamic_cast< const TParameter<Double_t>* >(
            FindObjThisEvt( GetAbsEvtTimeName() ) );
      if (evttime!=0) {
         at = evttime->GetVal();
      }

      if (fFirst) {
         fMinTime->SetVal(at);
         fMaxTime->SetVal(at);
      } else {
         if (at < fMinTime->GetVal()) {
            fMinTime->SetVal(at);
         }
         if (at > fMaxTime->GetVal()) {
            fMaxTime->SetVal(at);
         }
      }
      fFirst = kFALSE;

   } else {
      SendError(kAbortModule, "Process",
                "Coudl not get event header");
   }

}

void TSnFindTimeLimitsMod::Terminate() {
   // in case of running in proof, we need to "merge" the results properly
   
// if old root, we have to do the merging ourselves
#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,0)
   TAMOutput* output = GetModOutput();
   if (output!=0) {
      TList* outlist = output->GetOutputList();
      TIter nextOut(outlist);
      TObject* obj=0;
      TParameter<Double_t>* t=0;
      // if min/max time are non-zero, don't delete them because
      // they point to objects in the output list (either because
      // we're not in Proof or because TAM automatically assigned
      // the pointers while looking through Proof's output)
      fMinTime=0;
      fMaxTime=0;
      while ( (obj=nextOut())!=0 ) {
         if (fMinName.CompareTo(obj->GetName())==0) {
            t = dynamic_cast< TParameter<Double_t> >(obj);
            if (fMinTime==0) {
               fMinTime = new TParameter<Double_t>(*t);
            } else if (t->GetVal() < fMinTime->GetVal()) {
               fMinTime->SetVal(t->GetVal());
            }
            delete outlist->Remove(obj); // this was safe in simple TList tests
         } else if (fMaxName.CompareTo(obj->GetName())==0) {
            t = dynamic_cast< TParameter<Double_t> >(obj);
            if (fMaxTime==0) {
               fMaxTime = new TParameter<Double_t>(*t);
            } else if (t->GetVal() > fMaxTime->GetVal()) {
               fMaxTime->SetVal(t->GetVal());
            }
            delete outlist->Remove(obj); // this was safe in simple TList tests
         }
      }
      AddOutput(fMinTime);
      AddOutput(fMaxTime);
   } else {
      SendError(kStopModule, "Terminate",
                "Could not get TAMOutput for [%s].",
                GetName());
   }
#else
   TAModule::Terminate();
#endif // version < 5.34.0
   
}


