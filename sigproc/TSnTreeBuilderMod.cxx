#include "TSnTreeBuilderMod.h"

#include <TIterator.h>


ClassImp(TSnTreeBuilderMod);


void TSnTreeBuilderMod::SlaveBegin() {

   // request the branches we need
   TIter nb(fBrNames.MakeIterator());
   TSnTreeBldrBranch* b(0);
   while ( (b=static_cast<TSnTreeBldrBranch*>(nb()))!=0 ) {
      if (b->fSrc == kBranch) {
         b->CallReqBranch(this);
      }
   }
   
   // make the tree
   delete fOt;
   fOt = new TTree(fOutTreeNm.Data(), fOutTreeTitle.Data());
   AddOutput(fOt);
   
   fMakeBranches = kTRUE;
}

void TSnTreeBuilderMod::Process() {
   
   
   TIter nb(fBrNames.MakeIterator());
   TSnTreeBldrBranch* b(0);
   while ( (b=static_cast<TSnTreeBldrBranch*>(nb()))!=0 ) {
      // copy the info from the origin into our local copy
      // (the tree points to the local copy)
      if (b->fSrc == kBranch) {
         b->CallLoadBranch(this);
      } else if (b->fSrc == kEvtObj) {
         TObject* o = FindObjThisEvt(b->GetObjNm());
         const Bool_t copied = b->CopyFrom(o);
         if ( copied==kFALSE ) {
            return SendError(kAbortAnalysis, "Process",
                             "Could not copy object [%s] from event",
                             b->GetBrNm());
         }
      } else {
         return SendError(kAbortAnalysis, "Process",
                          "Unknown branch object source [%d] for [%s]",
                          static_cast<Int_t>(b->fSrc), b->GetBrNm());
      }
      // make the branch if need be
      if (fMakeBranches) {
         b->MakeBranch(fOt, fBufsz, fSplit);
      }
   }
   
   // fill the tree
   fOt->Fill();
   fMakeBranches = kFALSE;
   
}

