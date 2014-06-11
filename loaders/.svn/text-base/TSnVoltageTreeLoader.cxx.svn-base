#include "TSnVoltageTreeLoader.h"

#include <TString.h>

#include "TSnRawTreeMaker.h"
#include "TSnVoltageBranchLoader.h"

ClassImp(TSnVoltageTreeLoader);
////////////////////////////////////////////////////////////////////////////////
//
// TSnVoltageTreeLoader
//
// This loader finds the entry in the voltage tree that is the closest
// in time to the current event time. The default behavior is to find
// the closest in time before or after, but the loader may also be
// set to find the closest in time prior to.
//
// See TSnTimeLinkBranchLoader.
//
////////////////////////////////////////////////////////////////////////////////


TAMVirtualBranchLoader* 
TSnVoltageTreeLoader::CreateBranchLoader(TTree* tree,
                                         TAMBranchInfo* brInfo) {
   // if the branch being requested is in the voltage tree,
   // make a TSnTimeLinkBranchLoader for it
   
   TString brname(brInfo->GetName());
   // is it one of the voltage tree branches?
   if (brname.CompareTo(TSnRawTreeMaker::kPwrBrNm)==0) {
      return ( new TSnVoltageBranchLoader(brInfo,
					  TSnRawTreeMaker::kVTname,
					  TSnRawTreeMaker::kPwrBrNm,
					  TSnRawTreeMaker::kEHdBrNm,
					  fMeth) );
   }
   
   return 0;   
}
