#include "TSnAltTreeBranchLoader.h"

#include <TBranch.h>

#include "TSnAltTreeLoader.h"

ClassImp(TSnAltTreeBranchLoader);

TSnAltTreeBranchLoader::TSnAltTreeBranchLoader(TAMBranchInfo* binfo,
                                               TSnAltTreeLoader* ldr) :
      TAMTreeBranchLoader(binfo),
      fAltLoader(ldr) {
   // normal ctor
}

Int_t  TSnAltTreeBranchLoader::GetEntry(Long64_t mainEntry) {
   const Long64_t re = fAltLoader->GetAltEntryFor(mainEntry);
   return TAMTreeBranchLoader::GetEntry(re);
}

Bool_t TSnAltTreeBranchLoader::Notify(TTree* mainTree) {
   // call TAMTreeBranchLoader::Notify with the alternate tree
   
   return TAMTreeBranchLoader::Notify(fAltLoader->GetAltTree());
}

