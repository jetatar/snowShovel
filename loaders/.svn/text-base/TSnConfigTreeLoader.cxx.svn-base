#include "TSnConfigTreeLoader.h"

#include "TSnMetadataLinkBranchLoader.h"
#include "TSnRawTreeMaker.h"

ClassImp(TSnConfigTreeLoader);

TAMVirtualBranchLoader* TSnConfigTreeLoader::CreateBranchLoader(
                                                       TTree *tree, 
                                                       TAMBranchInfo* brInfo) {
   // if the branch being requested is in the config tree,
   // make a TSnMetadataLinkBranchLoader for it
   
   TString brname(brInfo->GetName());
   // is it one of the config tree branches?
   if ( (brname.CompareTo(TSnRawTreeMaker::kCMtBrNm)==0) ||
        (brname.CompareTo(TSnRawTreeMaker::kCnfBrNm)==0) ||
        (brname.CompareTo(TSnRawTreeMaker::kRunBrNm)==0) ||
        (brname.CompareTo(TSnRawTreeMaker::kNetBrNm)==0) ||
        (brname.CompareTo(TSnRawTreeMaker::kCSrBrNm)==0) ||
        (brname.CompareTo(TSnRawTreeMaker::kCSpBrNm)==0) ) {
      return ( new TSnMetadataLinkBranchLoader(brInfo,
                                               TSnRawTreeMaker::kCTname) );
   }
   
   return 0;
}

