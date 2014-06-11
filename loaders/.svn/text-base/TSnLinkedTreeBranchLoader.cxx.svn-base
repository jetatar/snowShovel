#include "TSnLinkedTreeBranchLoader.h"

#include <TTree.h>
#include <TFile.h>

// for compatibility
#if ROOT_VERSION_CODE < ROOT_VERSION(5,11,3) && !defined(R__ASSERT)
#define R__ASSERT Assert
#endif

ClassImp(TSnLinkedTreeBranchLoader);
////////////////////////////////////////////////////////////////////////////////
//
// This class has some general routines that can be used by any loader which
// behaves in the following way: as a module is looping over some particular
// entry in the main tree, the loader may want to look at that entry,
// and then determine how to load corresponding data from another tree.
//
// Classes inheriting from TSnLinkedTreeBranchLoader should implement the
// actual, typed, pointer to the data of the entry itself.
//
// There are 3 relevant branches:
// (a) the branch with the link data in the main tree the selector is looping
//     over. LoadEntryInMain will load the entry for this branch.
// (b) the branch with the link data in the linked tree. this data is used
//     for matching entries in the two trees. LoadMyEntry will load the
//     specified entry on this branch
// (c) the branch in the linked tree that holds the data the module has
//     called ReqBranch for. this is accessible via fBranch
//
// All type checking is done in this virtual base class during Notify. Thus
// classes inheriting from TSnLinkedTreeBranchLoader should be sure that this
// class' Notify get called -- either by not overloading Notify or by
// explicitly calling TSnLinkedTreeBranchLoader::Notify
//
////////////////////////////////////////////////////////////////////////////////


Int_t TSnLinkedTreeBranchLoader::LoadEntryInBranch(TBranch* br,
                                                   const Long64_t entry) {
   // Get requested entry from the requested branch.
   
   R__ASSERT( br != 0 );
   
   const Int_t ret = br->GetEntry(entry);
   if(ret<0) {
      throw std::runtime_error(
         Form("<TSnLinkedTreeBranchLoader::LoadEntryInBranch>: "
              "I/O error in file [%s] (at entry %lld) "
              "when accessing branch with name [%s].",
              (br->GetFileName()!=0) ? (br->GetFileName())
              : "null", 
              entry, br->GetName()));
   }
   
   return ret;

}

TTree* TSnLinkedTreeBranchLoader::LoadMyTree(TTree* mainTree) {
   // given the main tree, get the current file and look for the
   // linked tree in the file.
   //
   // this is called from TSnLinkedTreeBranchLoader::Notify

   TFile* tf = mainTree->GetCurrentFile();

   if ( (tf!=0) && (tf->IsZombie()==kFALSE) ) {
      TTree* ct = dynamic_cast<TTree*>(tf->Get(fLinkedTreeName.Data()));

      if (ct!=0) {
         return ct;
      } else {
         throw std::runtime_error(
            Form("<TSnLinkedTreeBranchLoader::GetMyBranch>: "
                 "Could not get config tree named [%s] from file [%s].",
                 fLinkedTreeName.Data(), tf->GetName()));
      }
   } else {
         throw std::runtime_error(
            Form("<TSnLinkedTreeBranchLoader::GetMyBranch>: "
                 "Could not get current file of main tree."));
   }
   return 0;
}

TBranch* TSnLinkedTreeBranchLoader::LoadMyBranch() {
   // get the metadata branch in the linked tree
   //
   // this is called from TSnLinkedTreeBranchLoader::Notify
   TTree* mytree = GetMyTree();
   if (mytree!=0) {
      return mytree->GetBranch( GetMyBranchName() );
   } else {
      throw std::runtime_error(
         Form("<TSnLinkedTreeBranchLoader::LoadMyBranch>: "
              "Could not get my tree."));
   }
   return 0;
}

TBranch* TSnLinkedTreeBranchLoader::LoadBranchInMain(TTree* mainTree) {
   // get the meta data branch from the main tree
   //
   // this is called from TSnLinkedTreeBranchLoader::Notify

   if (mainTree!=0) {
      return mainTree->GetBranch( GetMainBranchName() );
   } else {
      throw std::runtime_error(
         "<TSnLinkedTreeBranchLoader::GetBranchInMain>: "
         "Cannot got branch from null pointer.");
   }
   return 0;
}

Int_t TSnLinkedTreeBranchLoader::LoadEntryInMain(const Long64_t entry) {
   // Get requested entry from the linking branch in the main tree
   
   return LoadEntryInBranch(fMainBr, entry);
}

Int_t TSnLinkedTreeBranchLoader::LoadMyEntry(const Long64_t entry) {
   // Get requested entry from the linking branch in the linked tree
   
   return LoadEntryInBranch(fMyBr, entry);
}

Int_t TSnLinkedTreeBranchLoader::LoadRequestedEntry(const Long64_t entry) {
   // Get requested entry from the module requested branch in the
   // linked tree
   
   return LoadEntryInBranch(fBranch, entry);
}

Bool_t TSnLinkedTreeBranchLoader::Notify(TTree* mainTree) {
   // Notify via TAMSelector and TAMBranchInfo that a tree is
   // being connected from a (new) file.
   // Get the local tree from the file and do the type checking
   //
   // tree = the main tree
   
   R__ASSERT( mainTree != 0 );
   
   // load the main branch
   fMainBr = LoadBranchInMain(mainTree);
   if (fMainBr!=0) {
      
      fMyTree = LoadMyTree(mainTree);
      if (fMyTree!=0) {
         
         // load the link branch
         fMyBr   = LoadMyBranch();
         
         // load the requested branch like normal
         return TAMTreeBranchLoader::Notify(fMyTree);

      } else {
         throw std::runtime_error(
	      Form("<TSnLinkedTreeBranchLoader::Notify>: "
		   "LoadMyTree(%s) failed for branch [%s].",
		   mainTree->GetName(),
		   GetBInfo()->GetName()));
      }
   } else {
      throw std::runtime_error(
             Form("<TSnLinkedTreeBranchLoader::Notify>: "
		  "LoadBranchInMain() failed at tree [%s]. "
		  "MyBrNm=[%s] MainBrNm=[%s]",
		  mainTree->GetName(),
		  GetMyBranchName().Data(), GetMainBranchName().Data()));
   }
   
   return kFALSE;
      
}

