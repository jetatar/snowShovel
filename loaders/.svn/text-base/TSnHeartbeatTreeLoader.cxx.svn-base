#include "TSnHeartbeatTreeLoader.h"

#include <TString.h>

#include "TSnRawTreeMaker.h"
#include "TSnHeartbeatBranchLoader.h"

ClassImp(TSnHeartbeatTreeLoader);
////////////////////////////////////////////////////////////////////////////////
//
// TSnHeartbeatTreeLoader
//
// This loader finds the entry in the heartbeat tree that is the closest
// in time to the current event time. The default behavior is to find
// the closest in time before or after, but the loader may also be
// set to find the closest in time prior to.
//
// See TSnTimeLinkBranchLoader.
//
////////////////////////////////////////////////////////////////////////////////

TAMVirtualBranchLoader* 
TSnHeartbeatTreeLoader::CreateBranchLoader(TTree* tree,
                                           TAMBranchInfo* brInfo) {
   // if the branch being requested is in the heartbeat tree,
   // make a TSnTimeLinkBranchLoader for it
   
   TString brname(brInfo->GetName());
   // is it one of the voltage tree branches?
   if (brname.CompareTo(TSnRawTreeMaker::kHrtBrNm)==0) {
      return ( new TSnHeartbeatBranchLoader(brInfo,
					    TSnRawTreeMaker::kHTname,
					    TSnRawTreeMaker::kHrtBrNm,
					    TSnRawTreeMaker::kEHdBrNm,
					    fMeth) );
   }
   
   return 0;   
}
