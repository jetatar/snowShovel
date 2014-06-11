#ifndef SNS_TSnConfigTreeLoader
#define SNS_TSnConfigTreeLoader

#include "TAMVirtualLoader.h"
class TAMVirtualBranchLoader;

class TSnConfigTreeLoader : public TAMVirtualLoader {
 public:
   TSnConfigTreeLoader() {}
   virtual ~TSnConfigTreeLoader() {}

   TAMVirtualBranchLoader* CreateBranchLoader(TTree *tree, 
                                              TAMBranchInfo* brInfo);


   ClassDef(TSnConfigTreeLoader, 1); // use to load the Config tree while looping over the event tree
};

#endif // SNS_TSnConfigTreeLoader
