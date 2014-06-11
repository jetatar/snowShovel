#ifndef SNS_TSnLinkedTreeBranchLoader
#define SNS_TSnLinkedTreeBranchLoader

#include <TClass.h>
#include <TDataType.h>
#include <TBranch.h>
#include <TString.h>
class TTree;

#include "TAMTreeBranchLoader.h"

#include <stdexcept>

class TSnLinkedTreeBranchLoader : public TAMTreeBranchLoader {
 private:
   TBranch*     fMainBr; //! the main branch with linking data
   TTree*       fMyTree; //! the linked tree
   TBranch*     fMyBr;   //! the linking branch in the linked tree
   TString      fLinkedTreeName; //  name of the linked tree with the desired data

   
   Int_t        LoadEntryInBranch(TBranch* br, const Long64_t entry);
   
 protected:
   Int_t        LoadEntryInMain(const Long64_t entry);
   Int_t        LoadMyEntry(const Long64_t entry);
   Int_t        LoadRequestedEntry(const Long64_t entry);
   
         TBranch*   GetMainBranch()       { return fMainBr; }
   const TBranch*   GetMainBranch() const { return fMainBr; }
           TTree*   GetMyTree()       { return fMyTree; }
     const TTree*   GetMyTree() const { return fMyTree; }
         TBranch*   GetMyBranch()       { return fMyBr; }
   const TBranch*   GetMyBranch() const { return fMyBr; }

   // how to load the linked tree
   virtual TTree*        LoadMyTree(TTree* mainTree);
   // how to get the linking branch
   virtual TString       GetMyBranchName() const = 0;
   virtual TBranch*      LoadMyBranch();
   // how to get the main branch
   virtual TString       GetMainBranchName() const = 0;
   virtual TBranch*      LoadBranchInMain(TTree* mainTree);
   
   template<class T>
   void SetPointerToLinkDataIn(T*& link, TBranch* br) {
      // This is a bit of an ugly little function, but...
      // it sets the value of the pointer passed in to the address
      // into which the main branch is reading reading its data.
      // this data is used to link the two trees together.
      
      link = 0;

      if (br->GetAddress() != 0) {
         // attempt to check that the cast up will be ok
         TClass* cls;
         EDataType type;
         br->GetExpectedType(cls, type);
         if ( cls == T::Class() ) {
            // TBranch for an object stores the address of the pointer
            // to the object, not the address of the object itself.
            // So we have to:
            //    (a) cast from char* to a void**
            //    (b) de-reference once to get the memory location of the obj
            //    (c) cast from void* into a typed pointer
            void* const * const ptrLoc =
               reinterpret_cast<void**>(br->GetAddress());
            link = reinterpret_cast<T*>( *ptrLoc );
         } else if ( type == TDataType::GetType(typeid(T)) ) {
            // for a fundamental type, the address is the address of the number
            link = reinterpret_cast<T*>(br->GetAddress());
         } else {
            throw std::runtime_error(
               Form("<TSnLinkedTreeBranchLoader::SetPointerToLinkDataIn>: "
                    "Cannot cast address of branch [%s] of type [%s] to "
                    "pointer of type [%s].",
                    br->GetName(),
                    (cls!=0) ? cls->GetName() : TDataType::GetTypeName(type),
                    typeid(T).name()));
         }
      } 
      // else no branch: this can happen if no modules requested the
      // branch and no entry has been loaded from the branch. so we
      // don't throw an exception here, but just leave the link set to
      // 0. the loader may try setting the pointer again after data
      // has been loaded from the branch
      
   }



 public:
   TSnLinkedTreeBranchLoader() : fMainBr(0), fMyTree(0), fMyBr(0) {}
   TSnLinkedTreeBranchLoader(TAMBranchInfo* binfo,
                             const Char_t* linkedTreeName) : 
      TAMTreeBranchLoader(binfo), fMainBr(0), fMyTree(0), fMyBr(0),
      fLinkedTreeName(linkedTreeName) {}
   virtual ~TSnLinkedTreeBranchLoader() {}
   
   // how to load the correct corresponding entry
   virtual Int_t  GetEntry(Long64_t mainEntry) = 0;

   using TAMTreeBranchLoader::Notify;
   virtual Bool_t Notify(TTree* mainTree);

   
   ClassDef(TSnLinkedTreeBranchLoader, 2); // abstract base class for branch loaders that want to load from a tree that is linked somehow with entries in the main tree
};

#endif // SNS_TSnLinkedTreeBranchLoader


