#include "TSnMetadataLinkBranchLoader.h"

#include <TTree.h>

#include "TSnEventMetadata.h"
#include "TSnRawTreeMaker.h"

#include <stdexcept>

ClassImp(TSnMetadataLinkBranchLoader);
////////////////////////////////////////////////////////////////////////////////
//
// This class matches the TSnEventMetadata info from the main tree with the
// TSnEventMetadata in another tree of the same file. This branch loader
// is used by TSnConfigTreeLoader, for example.
//
////////////////////////////////////////////////////////////////////////////////

TString TSnMetadataLinkBranchLoader::GetMyBranchName() const {
   return TSnRawTreeMaker::kCMtBrNm;
}

TString TSnMetadataLinkBranchLoader::GetMainBranchName() const {
   return TSnRawTreeMaker::kEMtBrNm;
}

Int_t TSnMetadataLinkBranchLoader::GetEntry(Long64_t mainEntry) {
   // given the entry in the main tree, match up the meta data info and
   // load the corresponding entry in the linked tree
   
   LoadEntryInMain(mainEntry);
   // it's a shame to set the pointer every time, but it may be the
   // case that no module is requesting the metadata, in which case
   // SetAddress never gets called on the branch, and the automatic
   // location might change every event.  TODO: find a way to call
   // SetAddress that's behaves well with modules?
   SetPointerToLinkDataIn(fMeta, GetMainBranch());
   
   if (fMeta!=0) {
      Bool_t loadmine = (fMyMeta==0) || (GetBInfo()->IsLoaded()==kFALSE);
      if (loadmine==kFALSE) {
         // check if we're still on the same meta data
         loadmine = (fMeta->IsEqual(fMyMeta)==kFALSE);
      }
      if (loadmine) {
         // need to load data from my tree
         TTree* mytree = GetMyTree();
         if (mytree!=0) {
            
            const Long64_t e = 
               mytree->GetEntryNumberWithIndex(fMeta->GetRunNum(),
                                               fMeta->GetSeqNum());
            if (e>-1) {
               
               LoadMyEntry(e);
               // same as before (TODO?)
               SetPointerToLinkDataIn(fMyMeta, GetMyBranch());
               
               if ((fMyMeta!=0) && (fMyMeta->IsEqual(fMeta))) {
		 
                  // this is the correct entry. load the thing we
                  // really want
                  return LoadRequestedEntry(e);
                  
               } else {
                  Printf("MainMeta:");
                  if (fMeta!=0) {
                     fMeta->Print();
                  } else {
                     Printf("(nil)");
                  }
                  Printf("MyMeta:");
                  if (fMyMeta!=0) {
                     fMyMeta->Print();
                  } else {
                     Printf("(nil)");
                  }
                  throw std::runtime_error(
                     Form("<TSnMetadataLinkBranchLoader::GetEntry>: "
                          "Event metadata mismatch in main (%s) and "
                          "linked (%s) branches at entry %lld.",
                          GetMainBranch()->GetName(),
                          fBranch->GetName(), mainEntry));
               }
               
            } else {
               throw std::runtime_error(
                  Form("<TSnMetadataLinkBranchLoader::GetEntry>: "
                       "Could not get linked entry in [%s] "
                       "for run [%u], seq [%hu] at entry %lld.",
                       mytree->GetName(),
                       fMeta->GetRunNum(), fMeta->GetSeqNum(),
                       mainEntry));
               
            }
            
         } else {
            throw std::runtime_error("<TSnMetadataLinkBranchLoader::GetEntry>: "
                                     "Could not get linked tree.");
         }

      } // else we're still on the same entry of the linked tree

   } else {
      throw std::runtime_error("<TSnMetadataLinkBranchLoader::GetEntry>: "
                               "Could not get event metadata.");
   }
   
   return 0;
}

Bool_t TSnMetadataLinkBranchLoader::Notify(TTree* mainTree) {
   // call TSnLinkedTreeBranchLoader::Notify and then set the pointer to
   // the link data and build the index to quickly link the trees
   
   const Bool_t ok = TSnLinkedTreeBranchLoader::Notify(mainTree);
   
   if (ok) {
      SetPointerToLinkDataIn(fMeta,   GetMainBranch());
      SetPointerToLinkDataIn(fMyMeta, GetMyBranch());
      
      TTree* mytree = GetMyTree();
      if (mytree!=0) {
         // build the index only on run/seq. a tree with multiple
         // stations doesn't make sense, but that will be checked later
         //
         // extra dots (if kEMtBrNm ends in a dot) don't matter
         TString hn(Form("%s.fRun",GetMyBranchName().Data()));
         const Int_t ents = 
            mytree->BuildIndex(hn.Data(),
                               Form("%s.fSeq",GetMyBranchName().Data()));
         return (ents>0);
      } else {
         throw std::runtime_error("<TSnMetadataLinkBranchLoader::Notify>: "
                                  "Could not get my tree.");
         
      }
   }
   return kFALSE;
}

