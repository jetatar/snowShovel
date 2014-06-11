#ifndef SNS_TSnVoltageTreeLoader
#define SNS_TSnVoltageTreeLoader

#include "TAMVirtualLoader.h"
class TAMVirtualBranchLoader;

#include "TSnTimeLinkBranchLoader.h"

class TSnVoltageTreeLoader : public TAMVirtualLoader {
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
 private:
   TSnTimeLinkBranchLoader::EMethod_t fMeth; // the time linking method (default: absolute closest in time)

 public:
   TSnVoltageTreeLoader() : fMeth(TSnTimeLinkBranchLoader::kAbsoluteClosest) {}
   TSnVoltageTreeLoader(const TSnTimeLinkBranchLoader::EMethod_t m) : fMeth(m) {}
   virtual ~TSnVoltageTreeLoader() {}
   
   TAMVirtualBranchLoader* CreateBranchLoader(TTree *tree, 
                                              TAMBranchInfo* brInfo);
   
   ClassDef(TSnVoltageTreeLoader, 2); // use to load the Voltage tree while looping over the event tree
};

#endif // SNS_TSnVoltageTreeLoader
