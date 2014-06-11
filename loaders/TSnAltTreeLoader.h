#ifndef SNS_TSnAltTreeLoader
#define SNS_TSnAltTreeLoader

#include <TString.h>
class TTree;
class TEntryList;

#include "TAMVirtualLoader.h"

class TSnAltTreeLoader : public TAMVirtualLoader {
 private:
   TString     fAltTreeTag; // tag prefix for branches that should be loaded via this loader
   TTree*      fAltTree;    // the alternate tree to load from
   TEntryList* fEntryList;  // an entry list (to apply event selection to alternate tree). not owned. (default: 0 - no selection)
   Long64_t    fAltEntries; // cache the num entries in alt tree

 public:
   TSnAltTreeLoader() : fAltTree(0), fEntryList(0), fAltEntries(0) {}
   TSnAltTreeLoader(const Char_t* altTreeTag,
                    TTree* altTree, TEntryList* el=0);
   virtual ~TSnAltTreeLoader() {}
   
   virtual Long64_t  GetAltEntryFor(const Long64_t mainEntry) = 0;
   
   const Char_t*     GetAltTreeTagStr() const { return fAltTreeTag.Data(); }
   TString           GetAltTreeTag() const { return fAltTreeTag; }
   const TTree*      GetAltTree() const { return fAltTree; }
         TTree*      GetAltTree()       { return fAltTree; }
   const TEntryList* GetEntryList() const { return fEntryList; }
         TEntryList* GetEntryList()       { return fEntryList; }
   Long64_t          GetAltTreeEntries() const { return fAltEntries; }
   Long64_t          GetNumAltEntriesSlow() const;

   void              SetAltTreeTag(const Char_t* t) { fAltTreeTag = t; }
   void              SetAltTree(TTree* t) { fAltTree = t; }
   void              SetEntryList(TEntryList* e) { fEntryList = e; }
   
   virtual Long64_t  GetEntryNumberFor(const Long64_t e);
   
   virtual 
   TAMVirtualBranchLoader* CreateBranchLoader(TTree *tree, 
                                              TAMBranchInfo* brInfo);

   ClassDef(TSnAltTreeLoader, 1); // ABC for alternate tree loaders
   
};

#endif // SNS_TSnAltTreeLoader
