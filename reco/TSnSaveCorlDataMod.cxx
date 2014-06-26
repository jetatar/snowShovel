#include "TSnSaveCorlDataMod.h"

#include <TObjArray.h>
#include <TTree.h>
#include <TClass.h>

#include "TSnCalDatModule.h"
#include "TSnCalWvData.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnRawTreeMaker.h"
#include "TSnInterp1DWvData.h"

ClassImp(TSnSaveCorlDataMod);


TSnSaveCorlDataMod::TSnSaveCorlDataMod(const Char_t* name, const Char_t* mainDataNm,
                                       const Char_t* treeName, const Char_t* treeTitle,
                                       const Int_t   bufsz,
                                       const Int_t   split) :
   TSnCalDatModule(name, "save correlation data"),
   fEvtBrA(mainDataNm),
   fCorlNm("CrossChanCorls."),
   fOutTreeNm(treeName),
   fOutTreeTitle(treeTitle),
   fSaveHeader(kTRUE), fSaveData(kTRUE),
   fOt(0), fSaveAltEvt(kFALSE), fBufSz(bufsz),
   fSplit(split), fDatA(0), fDatB(0), fCorls(0),
   fHdrA(0), fHdrB(0), fMetaA(0), fMetaB(0),
   fMakeBranches(kTRUE) {
   // normal ctor
}
   

TSnSaveCorlDataMod::~TSnSaveCorlDataMod() {
   delete fOt;
}

void TSnSaveCorlDataMod::MakeBranches(const TObjArray& corls) {

   fOt->Branch(TSnRawTreeMaker::kEMtBrNm, &fMetaA, fBufSz, fSplit);
   if (fSaveHeader) {
      fOt->Branch(TSnRawTreeMaker::kEHdBrNm,  &fHdrA, fBufSz, fSplit);
   }
   if (HasAltBranch()) {
      fOt->Branch(fAltMetaBrNm.Data(), &fMetaB, fBufSz, fSplit);
      if (fSaveHeader) {
         fOt->Branch(fAltHdrBrNm.Data(),   &fHdrB,  fBufSz, fSplit);
      }
   }

   if (fSaveData) {
      MakeDatBranch(fDatA, fEvtBrA.Data());
      if (HasAltBranch()) {
         MakeDatBranch(fDatB, fEvtBrB.Data());
      }
   }
   
   // need to set up the objects in the array
   // they can't move around in memory from event to event
   // (like the one in FindObjThisEvt, which is new every time)
   CopyCorls(corls, kTRUE);
   fOt->Branch(fCorls, fBufSz, fSplit);
   
   fMakeBranches = kFALSE;
   
}

void TSnSaveCorlDataMod::MakeDatBranch(TSnCalWvData*& dat,
                                       const Char_t* brnm) {
   if (dat!=0) {
      const TClass* c = dat->IsA();
      if (c!=0) {
         fOt->Branch(brnm, c->GetName(),
                     &dat, fBufSz, fSplit);
      } else {
         SendError(kAbortAnalysis, "MakeDatBranch",
                   "Could not get class for data branch at [%p].",
                   static_cast<const void*>(dat));
      }
   } else {
      SendError(kAbortAnalysis, "MakeDatBranch",
                "Cannot make data branch with null pointer.");
   }
}

void TSnSaveCorlDataMod::CopyCorls(const TObjArray& corls, 
                                   const Bool_t makeNew) {
   const Int_t n = corls.GetSize();
   if (makeNew) {
      fCorls = new TObjArray(n);
   }
   if (n==fCorls->GetSize()) {
      
      TSnInterp1DWvData* myc;
      const TSnInterp1DWvData* c;
      for (Int_t i=0; i<n; ++i) {
         c = dynamic_cast<const TSnInterp1DWvData*>(corls.At(i));
         if (c!=0) {
            if (makeNew) {
               myc = dynamic_cast<TSnInterp1DWvData*>(c->Clone());
               fCorls->AddAt(myc, i);
            } else {
               myc = dynamic_cast<TSnInterp1DWvData*>(fCorls->At(i));
            }
            if (myc!=0) {
               
               myc->CopyFrom(*c);
               
            } else {
               SendError(kAbortAnalysis, "CopyCorls",
                         "No local correlation to copy chan corl [%d] to.",
                         i);
            }
         }
      }
      
   } else {
      SendError(kAbortAnalysis, "CopyCorls",
                "Size of correlation arrays differ. fCorls=%d, corls=%d.",
                fCorls->GetSize(), n);
   }
}

void TSnSaveCorlDataMod::SlaveBegin() {
   // always write the main event header and metadata
   ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMetaA);
   if (fSaveHeader) {
      ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdrA);
   }
   if (fSaveData) {
      ReqCalData(fEvtBrA.Data(), fDatA);
   }
   
   if (HasAltBranch()) {
      fAltMetaBrNm = Form("%s%s",fAltTag.Data(),TSnRawTreeMaker::kEMtBrNm);
      ReqBranch(fAltMetaBrNm.Data(), fMetaB);
      if (fSaveHeader) {
         fAltHdrBrNm  = Form("%s%s",fAltTag.Data(),TSnRawTreeMaker::kEHdBrNm);
         ReqBranch(fAltHdrBrNm.Data(), fHdrB);
      }
      if (fSaveData) {
         fEvtBrB = Form("%s%s",fAltTag.Data(),fEvtBrA.Data());
         ReqCalData(fEvtBrB.Data(), fDatB);
      }
   }
   
   fOt = new TTree(fOutTreeNm.Data(), fOutTreeTitle.Data());

   fOt->SetAutoSave();
   AddOutput(fOt);

}

void TSnSaveCorlDataMod::Process() {
   LoadBranch(TSnRawTreeMaker::kEMtBrNm);
   if (fSaveHeader) {
      LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   }
   if (fSaveData) {
      LoadCalData(fEvtBrA.Data());
   }
   
   if (HasAltBranch()) {
      LoadBranch(fAltMetaBrNm.Data());
      if (fSaveHeader) {
         LoadBranch(fAltHdrBrNm.Data());
      }
      if (fSaveData) {
         LoadCalData(fEvtBrB.Data());
      }
   }
   
   const TObjArray* corls =
      dynamic_cast<const TObjArray*>(FindObjThisEvt(fCorlNm.Data()));
   if (corls!=0) {
      
      if (fMakeBranches) {
         // this is done here instead of SlaveBegin because we don't
         // know what kind of thing fDatA will be until we get it from
         // an event
         MakeBranches(*corls);
      } else {
         CopyCorls(*corls);
      }
      
      // fill the tree
      fOt->Fill();
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get channel correlation functions named "
                "[%s].", fCorlNm.Data());
   }

}

