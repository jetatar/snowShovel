#include "TSnMakeShiftTree.h"

#include <TTree.h>

#include "TSnStopPosnSet.h"
#include "TSnCalWvData.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

#include "TSnRawTreeMaker.h"

ClassImp(TSnMakeShiftTree);

TSnMakeShiftTree::~TSnMakeShiftTree() {
   // dtor
   delete fOt;
   delete fStops;
   delete fShifted;
}

void TSnMakeShiftTree::SlaveBegin() {
   ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMeta);
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   ReqBranch(fDatBrNm.Data(), fDat);
   
   fOt = new TTree(fOutTreeNm.Data(), fOutTreeTitle.Data());
   fOt->SetAutoSave();
   AddOutput(fOt);
   
   fStops = new TSnStopPosnSet;
   fShifted = new TSnCalWvData;
   
   fOt->Branch(TSnRawTreeMaker::kEMtBrNm, &fMeta, fBufSz, fSplit);
   fOt->Branch(TSnRawTreeMaker::kEHdBrNm,  &fHdr, fBufSz, fSplit);
   fOt->Branch(fDatBrNm.Data(), &fDat, fBufSz, fSplit);
   fOt->Branch(GetSplitSafeBrNm(fStopNm), &fStops,  fBufSz, fSplit);
   fOt->Branch(GetSplitSafeBrNm(fShiftNm), &fShifted, fBufSz, fSplit);
}

TString TSnMakeShiftTree::GetSplitSafeBrNm(const TString& s) const {
   // if making a split tree, add a . to the end of the branch name if
   // it doesn't already have one
   TString x(s);
   if ( (fSplit>0) && (s.EndsWith(".")==kFALSE) ) {
      x.Append(".");
   }
   return x;
}

void TSnMakeShiftTree::Process() {
   
   LoadBranch(TSnRawTreeMaker::kEMtBrNm);
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   LoadBranch(fDatBrNm.Data());
   if ( (fMeta!=0) && (fHdr!=0) && (fDat!=0) ) {
      
      const TSnStopPosnSet* stops = 
         dynamic_cast<const TSnStopPosnSet*>(FindObjThisEvt(fStopNm.Data()));
      const TSnCalWvData* shifted =
         dynamic_cast<const TSnCalWvData*>(FindObjThisEvt(fShiftNm.Data()));
      if ( (stops!=0) && (shifted!=0) ) {
         
         // copy to out local variables (that the tree knows about)
         *fStops   = *stops;
         *fShifted = *shifted;
         
         // fill the tree
         fOt->Fill();
         
      } else {
         SendError(kAbortAnalysis, "Process",
                   "Could not load the stops [%s]=(%p) and/or the "
                   "shifted [%s]=(%p)",
                   fStopNm.Data(), static_cast<const void*>(stops),
                   fShiftNm.Data(), static_cast<const void*>(shifted));
      }
      
   } else {
      SendError(kAbortAnalysis, "Process",
                "Could not load all event data: "
                "meta [%s]=(%p), hdr [%s]=(%p), dat [%s]=(%p)",
                TSnRawTreeMaker::kEMtBrNm, static_cast<const void*>(fMeta),
                TSnRawTreeMaker::kEHdBrNm, static_cast<const void*>(fHdr),
                fDatBrNm.Data(), static_cast<const void*>(fDat));
   }
   
}

void TSnMakeShiftTree::SlaveTerminate() {
   delete fStops;
   delete fShifted;
}
