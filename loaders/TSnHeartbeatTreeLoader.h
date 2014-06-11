#ifndef SNS_TSnHeartbeatTreeLoader
#define SNS_TSnHeartbeatTreeLoader

#include "TAMVirtualLoader.h"
class TAMVirtualBranchLoader;

#include "TSnTimeLinkBranchLoader.h"

class TSnHeartbeatTreeLoader : public TAMVirtualLoader {
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
 private:
   TSnTimeLinkBranchLoader::EMethod_t fMeth; // the time linking method (default: absolute closest in time)
   
 public:
   TSnHeartbeatTreeLoader() : fMeth(TSnTimeLinkBranchLoader::kAbsoluteClosest) {}
   TSnHeartbeatTreeLoader(const TSnTimeLinkBranchLoader::EMethod_t m) : fMeth(m) {}
   virtual ~TSnHeartbeatTreeLoader() {}
   
   TAMVirtualBranchLoader* CreateBranchLoader(TTree *tree, 
                                              TAMBranchInfo* brInfo);
   
   ClassDef(TSnHeartbeatTreeLoader, 2); // use to load the Heartbeat tree while looping over the event tree
};

#endif // SNS_TSnHeartbeatTreeLoader
