#ifndef SNS_TSnMetadataLinkBranchLoader
#define SNS_TSnMetadataLinkBranchLoader

#include "TSnLinkedTreeBranchLoader.h"
class TSnEventMetadata;

class TSnMetadataLinkBranchLoader : public TSnLinkedTreeBranchLoader {
 private:
   TSnEventMetadata* fMeta;   //! the meta data in the main tree to match
   TSnEventMetadata* fMyMeta; //! the meta data my tree to match
 
 public:
   TSnMetadataLinkBranchLoader() : fMeta(0), fMyMeta(0) {}
   TSnMetadataLinkBranchLoader(TAMBranchInfo* binfo,
                               const Char_t* linkedTreeName) :
      TSnLinkedTreeBranchLoader(binfo, linkedTreeName),
      fMeta(0), fMyMeta(0) {}
   virtual ~TSnMetadataLinkBranchLoader() {}
   
   virtual TString GetMyBranchName() const;
   virtual TString GetMainBranchName() const;
   
   virtual Int_t  GetEntry(Long64_t mainEntry);
   using TSnLinkedTreeBranchLoader::Notify;
   virtual Bool_t Notify(TTree* mainTree);
   
   ClassDef(TSnMetadataLinkBranchLoader, 2); // match meta data from the main tree with meta data in the linked tree
};

#endif // SNS_TSnMetadataLinkBranchLoader
