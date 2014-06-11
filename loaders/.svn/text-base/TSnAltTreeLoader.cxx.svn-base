#include "TSnAltTreeLoader.h"

#include <TTree.h>
#include <TEntryList.h>

#include "TAMBranchInfo.h"

#include "TSnAltTreeBranchLoader.h"

#include <stdexcept>

ClassImp(TSnAltTreeLoader);


TSnAltTreeLoader::TSnAltTreeLoader(const Char_t* altTreeTag,
                                   TTree* altTree, TEntryList* el) :
   fAltTreeTag(altTreeTag), fAltTree(altTree), fEntryList(el),
   fAltEntries(0) {
   // normal ctor
   //
   // NOTE: If neither the tree nor this loader has an EntryList specified,
   // then entries will be loaded from the alternate tree completely at
   // ranomd -- thus no event selection will be applied. If an event selection
   // is required for events from the alternate tree, an EntryList will have
   // to be created beforehand and passed either to this loader or to the
   // tree. If both are specified, the EntryList passed to the loader is used.
   //
   
   if (fAltTree!=0) {
      fAltEntries = GetNumAltEntriesSlow();
   }
}

TAMVirtualBranchLoader*
TSnAltTreeLoader::CreateBranchLoader(TTree *tree, 
                                     TAMBranchInfo* brInfo) {
   // make a TSnAltTreeBranchLoader for the branch if the branch
   // name starts with our specified alternate tree tag.
   //
   // NOTE: If neither the tree nor this loader has an EntryList specified,
   // then entries will be loaded from the alternate tree completely at
   // ranomd -- thus no event selection will be applied. If an event selection
   // is required for events from the alternate tree, an EntryList will have
   // to be created beforehand and passed either to this loader or to the
   // tree. If both are specified, the EntryList passed to the loader is used.

   TAMVirtualBranchLoader* bl = 0;

   if (GetAltTree()!=0) {
      TString brname(brInfo->GetName());
      if ( brname.BeginsWith(GetAltTreeTagStr()) ) {
         // set an alias to the real branch name.
         // the routines in TAMTreeBranchLoader check for this.
         // so we only need a special branch loader in order to send
         // the alternate instead of the main tree.
         const Ssiz_t l = GetAltTreeTag().Length();
         const TString x( brname(l, brname.Length() - l) );
         GetAltTree()->SetAlias( brname.Data(), x.Data() );
         bl = new TSnAltTreeBranchLoader(brInfo, this);
      }
   } else {
      throw std::runtime_error(
         "<TSnAltTreeLoader::CreateBranchLoader>: "
         "Cannot create branch loader without alternate tree!");
   }   
   
   return bl;
}

Long64_t TSnAltTreeLoader::GetEntryNumberFor(const Long64_t e) {
   // use the entry list to find the entry number for the 'e' or else
   // get it from the tree (if the tree has an entry list)
   
   return (GetEntryList()!=0) 
      ? GetEntryList()->GetEntry(e)      // if we have EntryList
      : GetAltTree()->GetEntryNumber(e); // tree might have E.L.
}

Long64_t TSnAltTreeLoader::GetNumAltEntriesSlow() const {
   // get the number of entries in the entry list (if there is one) or
   // else the tree
   //
   // note that for chains this can be quite slow, as it counts all
   // trees. therefore, this is called once in the constructor of
   // TSnAltTreeLoader in order to cache the value. loaders may
   // access the cached value via GetAltTreeEntries()
   
   const Long64_t n = (GetEntryList()!=0)
      ? GetEntryList()->GetN()
      : GetAltTree()->GetEntries();
   return n;
}
