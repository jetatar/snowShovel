#include "TSnSaveRecoResultsMod.h"

#include <TClass.h>
#include <TTree.h>
#include <TObjString.h>
#include <TIterator.h>

#include "TSnRawTreeMaker.h"
#include "TSnRecoResult.h"
#include "TSnRecoSingleChanOffsetSet.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

ClassImp(TSnSaveRecoResultsMod);
ClassImp(TSnSaveRecoResultsMod::TSnRecoResultWrapper);

TSnSaveRecoResultsMod::TSnSaveRecoResultsMod(const Char_t* name,
                                             const Char_t* treeName,
                                             const Char_t* treeTitle,
                                             const Int_t   bufsz,
                                             const Int_t   split) :
   TAModule(name, "save reconstruction results"),
   fOutTreeNm(treeName),
   fOutTreeTitle(treeTitle),
   fOt(0),
   fSaveAltEvt(kFALSE), fSaveHeader(kTRUE),
   fBufSz(bufsz), fSplit(split),
   fMakeBranches(kTRUE),
   fHdrA(0), fHdrB(0), fMetaA(0), fMetaB(0) {
   // normal ctor
   fRecoObjNames.SetOwnerKeyValue(kTRUE,kTRUE);
}

TSnSaveRecoResultsMod::~TSnSaveRecoResultsMod() {
   delete fOt;
}

void TSnSaveRecoResultsMod::MakeBranches() {
   // the event metadata and header branches are built using
   // the AltTreeTag+BranchName.
   
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
   
   TIter nro(fRecoObjNames.MakeIterator());
   TObject* nm(0);
   //TSnRecoResult* r(0);
   TObject* r(0);
   while ( (nm=nro())!=0 ) {
      //r = dynamic_cast<TSnRecoResult*>( FindObjThisEvt(nm->GetName()) );
      r = FindObjThisEvt(nm->GetName());
      if (r!=0) {
         const TClass* c = r->IsA();
         if (c!=0) {
            Int_t split = fSplit;
            if (c->InheritsFrom(TSnRecoSingleChanOffsetSet::Class())) {
               // split up the collection
               ++split;
            }
            
            // reset this entry to a non-null version of this object
            // and use TClass to get the correct type of reco result
            fRecoObjNames.Remove(nm);
            TSnRecoResultWrapper* myr = new TSnRecoResultWrapper(
               reinterpret_cast<TObject*>(c->New()) );
            fRecoObjNames.Add(nm, myr);
            
            // make the branch for this reconstruction result
            fOt->Branch(r->GetName(), c->GetName(),
                        &(myr->fRR),     // the reason for the wrapper
                        fBufSz, split);
         } else {
            return SendError(kAbortAnalysis, "MakeBranches",
                             "Could not get TClass for [%s] at [%p].",
                             nm->GetName(), static_cast<const void*>(r));
         }
      }
   }
   
   fMakeBranches = kFALSE;
}

void TSnSaveRecoResultsMod::SlaveBegin() {
   
   // always write the main event header and metadata
   ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMetaA);
   if (fSaveHeader) {
      ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdrA);
   }
   
   if (HasAltBranch()) {
      fAltMetaBrNm = Form("%s%s",fAltTag.Data(),TSnRawTreeMaker::kEMtBrNm);
      ReqBranch(fAltMetaBrNm.Data(), fMetaB);
      if (fSaveHeader) {
         fAltHdrBrNm  = Form("%s%s",fAltTag.Data(),TSnRawTreeMaker::kEHdBrNm);
         ReqBranch(fAltHdrBrNm.Data(), fHdrB);
      }
   }
   
   fOt = new TTree(fOutTreeNm.Data(), fOutTreeTitle.Data());
   fOt->SetAutoSave();
   AddOutput(fOt);
   
}

void TSnSaveRecoResultsMod::Process() {
   // load the data and fill the tree.
   //
   // note that this module could be used to add branches to a tree for
   // ANY object in the event, as long as it has the Copy(TObject& o) const
   // function from TObject overloaded. all TSnRecoResult objects have
   // this property automatically (from TSnRecoResult). without the Copy
   // function, the branch will contain a bunch of zeros (or whatever
   // the object initializes things to in its default constructor).
   
   LoadBranch(TSnRawTreeMaker::kEMtBrNm);
   if (fSaveHeader) {
      LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   }
   
   if (HasAltBranch()) {
      LoadBranch(fAltMetaBrNm.Data());
      if (fSaveHeader) {
         LoadBranch(fAltHdrBrNm.Data());
      }
   }
   
   if (fMakeBranches) {
      // note that the branches cannot be created and filled in the
      // same loop over the reco object names
      MakeBranches();
   }
   
   // copy each reco object into our corresponding reco object (which
   // is in the same place in memory every event, while the ones from
   // FindObjThisEvt are not)
   TIter nro(fRecoObjNames.MakeIterator());
   TObject* nm(0);
   const TObject* r(0);
   TSnRecoResultWrapper* myr(0);
   while ( (nm=nro())!=0 ) {
      myr = dynamic_cast<TSnRecoResultWrapper*>(fRecoObjNames.GetValue(nm));
      r = FindObjThisEvt(nm->GetName());
      if (r!=0) {
         r->Copy(*(myr->fRR));
      }
   }
   
   // fill the tree
   fOt->Fill();
}

void TSnSaveRecoResultsMod::SaveRecoResult(const Char_t* name) {
   fRecoObjNames.Add(new TObjString(name), 0);
}

