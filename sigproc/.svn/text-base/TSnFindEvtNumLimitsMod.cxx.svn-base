#include "TSnFindEvtNumLimitsMod.h"

#include <TList.h>

#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnFindEvtNumLimitsMod);

TSnFindEvtNumLimitsMod::TSnFindEvtNumLimitsMod(const Char_t* name) :
   TAModule(name, "find min/max event numbers"),
   fMinName("MinEvtNum"), fMaxName("MaxEvtNum"),
   fMinEvtNum(0), fMaxEvtNum(0), fHdr(0), fFirst(kTRUE) {
}

TSnFindEvtNumLimitsMod::~TSnFindEvtNumLimitsMod() {
   delete fMinEvtNum;
   delete fMaxEvtNum;
}

void TSnFindEvtNumLimitsMod::SlaveBegin() {
   fMinEvtNum = new TParameter<UInt_t>(GetMinEvtNumName(), 0);
   fMaxEvtNum = new TParameter<UInt_t>(GetMaxEvtNumName(), 0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,0)
   fMinEvtNum->SetMergeMode('m'); // min
   fMaxEvtNum->SetMergeMode('M'); // max
#endif
   AddOutput(fMinEvtNum);
   AddOutput(fMaxEvtNum);
   fFirst = kTRUE;
   
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnFindEvtNumLimitsMod::Process() {
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (0!=fHdr) {
      const UInt_t en = fHdr->GetEvtNum();
      if (fFirst) {
         fMinEvtNum->SetVal(en);
         fMaxEvtNum->SetVal(en);
      } else {
         if (en < fMinEvtNum->GetVal()) {
            fMinEvtNum->SetVal(en);
         }
         if (en > fMaxEvtNum->GetVal()) {
            fMaxEvtNum->SetVal(en);
         }
      }
      fFirst = kFALSE;
   } else {
      SendError(kAbortModule, "Process",
                "Coudl not get event header");
   }
}

void TSnFindEvtNumLimitsMod::Terminate() {
   // in case of running in proof, we need to "merge" the results properly
   
// if old root, we have to do the merging ourselves
#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,0)
   TAMOutput* output = GetModOutput();
   if (output!=0) {
      TList* outlist = output->GetOutputList();
      TIter nextOut(outlist);
      TObject* obj=0;
      TParameter<UInt_t>* t=0;
      // if min/max are non-zero, don't delete them because
      // they point to objects in the output list (either because
      // we're not in Proof or because TAM automatically assigned
      // the pointers while looking through Proof's output)
      fMinTime=0;
      fMaxTime=0;
      while ( (obj=nextOut())!=0 ) {
         if (fMinName.CompareTo(obj->GetName())==0) {
            t = dynamic_cast< TParameter<UInt_t> >(obj);
            if (fMinEvtNum==0) {
               fMinEvtNum = new TParameter<UInt_t>(*t);
            } else if (t->GetVal() < fMinEvtNum->GetVal()) {
               fMinEvtNum->SetVal(t->GetVal());
            }
            delete outlist->Remove(obj); // this was safe in simple TList tests
         } else if (fMaxName.CompareTo(obj->GetName())==0) {
            t = dynamic_cast< TParameter<UInt_t> >(obj);
            if (fMaxEvtNum==0) {
               fMaxEvtNum = new TParameter<UInt_t>(*t);
            } else if (t->GetVal() > fMaxEvtNum->GetVal()) {
               fMaxEvtNum->SetVal(t->GetVal());
            }
            delete outlist->Remove(obj); // this was safe in simple TList tests
         }
      }
      AddOutput(fMinEvtNum);
      AddOutput(fMaxEvtNum);
   } else {
      SendError(kStopModule, "Terminate",
                "Could not get TAMOutput for [%s].",
                GetName());
   }
#else
   TAModule::Terminate();
#endif // version < 5.34.0
   
}
