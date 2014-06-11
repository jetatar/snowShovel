#ifndef SNS_TSnAltTreeBranchLoader
#define SNS_TSnAltTreeBranchLoader

#include "TAMTreeBranchLoader.h"
class TSnAltTreeLoader;

class TSnAltTreeBranchLoader : public TAMTreeBranchLoader {
 private:
   TSnAltTreeLoader* fAltLoader; // the alt tree loader
   
 public:
   TSnAltTreeBranchLoader() : fAltLoader(0) {}
   TSnAltTreeBranchLoader(TAMBranchInfo* binfo,
                          TSnAltTreeLoader* ldr);
   virtual ~TSnAltTreeBranchLoader() {}
   
   virtual Int_t  GetEntry(Long64_t mainEntry);
   using TAMTreeBranchLoader::Notify;
   virtual Bool_t Notify(TTree* mainTree);
   
   ClassDef(TSnAltTreeBranchLoader, 2); // load a random entry from a branch in an alternate tree
};

#endif // SNS_TSnAltTreeBranchLoader
