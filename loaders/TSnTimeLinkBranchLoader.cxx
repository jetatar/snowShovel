#include "TSnTimeLinkBranchLoader.h"

#include <TTree.h>
#include <TTimeStamp.h>

#include <stdexcept>

ClassImp(TSnTimeLinkBranchLoader);
////////////////////////////////////////////////////////////////////////////////
//
// TSnTimeLinkBranchLoader
//
// Base class for loaders linking entries in two trees via a TTimeStamp.
// 
// Finds the entry in the linked tree that is connected in time to the
// time in main (event) tree. Two methods are available: "at or prior
// to" or "absolute closest".
//
// Note that the behavior depends on
// TTree::GetEntryNumberWithBestIndex, and if the behavior of that
// function changes, then so will this loader.
//
// Used by TSnVoltageTreeLoader and TSnHeartbeatTreeLoader.
//
////////////////////////////////////////////////////////////////////////////////

TSnTimeLinkBranchLoader::~TSnTimeLinkBranchLoader() {
   delete fMainCache;
}

Int_t TSnTimeLinkBranchLoader::GetEntry(Long64_t mainEntry) {
   // given the entry in the main tree, match up the time info and
   // load the corresponding entry in the linked tree
   
   LoadEntryInMain(mainEntry);
   // it's a shame to set the pointer every time, but it may be the
   // case that no module is requesting the metadata, in which case
   // SetAddress never gets called on the branch, and the automatic
   // location might change every event.  TODO: find a way to call
   // SetAddress that behaves well with modules?
   SetPointerToMainLinkData();
   
   if (GetMainTime()!=0) {
      Bool_t loadmine = (GetMyTime()==0) || (GetBInfo()->IsLoaded()==kFALSE) ||
                        (fMainCache==0);
      if (loadmine==kFALSE) {
         // check if we're still on the same timestamp
 	 loadmine = (GetMainTime()->GetSec()     == fMainCache->GetSec()) ||
	            (GetMainTime()->GetNanoSec() == fMainCache->GetNanoSec());
      }
      if (loadmine) {
         // need to load data from my tree
         TTree* mytree = GetMyTree();
         if (mytree!=0) {
            
            // NOTE: currently, this ttree function does a lower_bound
            // search and if the result is notan exact match, it
            // returns the next lower one. this works perfectly given
            // these two index variable, as we will find "the closest
            // in time at or prior to"... but if the behavior of
            // the function changes, so will the behavior of this loader!
            const Long64_t e = 
	       mytree->GetEntryNumberWithBestIndex(GetMainTime()->GetSec(),
						   GetMainTime()->GetNanoSec());
            if (e>-1) {
               
               // load closest prior unless specified method chooses
               // something else
               Long64_t ment = e;
               
               LoadMyEntry(e);
               // same comment as before (TODO?)
	       SetPointerToMyLinkData();
               
               if (fMeth == kAbsoluteClosest) {
                  
 		  const Double_t tmain  = GetMainTime()->AsDouble();
		  const Double_t tprior = GetMyTime()->AsDouble();
                  
                  const Int_t bytes = LoadMyEntry(e+1);
                  if (bytes>0) {
                     
		     const Double_t tnext = GetMyTime()->AsDouble();
                     const Double_t dtp = TMath::Abs(tmain-tprior);
                     const Double_t dtn = TMath::Abs(tmain-tnext);
                     if (dtn<dtp) {
                        // next is closer; use it
                        ment = e+1;
                     }
                     
                  } // else there is no later time in this file, so stick
                    // with the closest prior
                  
                  
               } // else already found the closest prior
               
               // keep synchronized so the cached check will work
               LoadMyEntry(ment);
               if (fMainCache==0) {
		  fMainCache = new TTimeStamp(*GetMainTime());
               } else {
		  *fMainCache = *GetMainTime();
               }
               
               // load the linked entry of the branch the module really wants
               return LoadRequestedEntry(ment);
               
            } else {
               throw std::runtime_error(
                  Form("<TSnTimeLinkBranchLoader::GetEntry>: "
                       "Could not get linked entry in [%s] "
                       "for time [%u], nano sec [%d] at entry %lld.",
                       mytree->GetName(),
                       static_cast<UInt_t>(GetMainTime()->GetSec()),
                       GetMainTime()->GetNanoSec(),
                       mainEntry));
            }
            
         } else {
            throw std::runtime_error("<TSnTimeLinkBranchLoader::GetEntry>: "
                                     "Could not get linked tree.");
         }

      } // else we're still on the same entry of the linked tree

   } else {
      throw std::runtime_error("<TSnTimeLinkBranchLoader::GetEntry>: "
                               "Could not get event time.");
   }
   
   return 0;
}

Bool_t TSnTimeLinkBranchLoader::Notify(TTree* mainTree) {
   // call TSnLinkedTreeBranchLoader::Notify and then set the pointer to
   // the link data and build the index to quickly link the trees

   const Bool_t ok = TSnLinkedTreeBranchLoader::Notify(mainTree);
   
   if (ok) {
      SetPointerToMainLinkData();
      SetPointerToMyLinkData();
      
      TTree* mytree = GetMyTree();
      if (mytree!=0) {
         // build the index on seconds, nano seconds
         //
         // extra dots (if kEMtBrNm ends in a dot) don't matter
 	 const Int_t ents = BuildIndexOn(mytree);
         return (ents>0);
      } else {
         throw std::runtime_error("<TSnMetadataLinkBranchLoader::Notify>: "
                                  "Could not get my tree.");
         
      }
   }
   return kFALSE;
}

Int_t TSnTimeLinkBranchLoader::BuildIndexOn(TTree* mytree) {

  TString hn(Form("%s.fTime.GetSec()",GetMyBranchName().Data()));
  const Int_t ents =
    mytree->BuildIndex(hn.Data(),
                       Form("%s.fTime.GetNanoSec()",
                            GetMyBranchName().Data()));
  return ents;
}
