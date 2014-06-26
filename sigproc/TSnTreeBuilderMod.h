#ifndef SNS_TSnTreeBuilderMod
#define SNS_TSnTreeBuilderMod

#include <TNamed.h>
#include <TString.h>
#include <TTree.h>
#include <TObjArray.h>

#include "TAModule.h"

class TSnTreeBuilderMod : public TAModule {
 public:
   enum EBrObjSource_t {
      kBranch,
      kEvtObj
   };

   struct TSnTreeBldrBranch : public TNamed {
      EBrObjSource_t fSrc;
      TSnTreeBldrBranch(const Char_t* name,
                        const EBrObjSource_t src) :
         TNamed(name,NULL), fSrc(src) {}
      TSnTreeBldrBranch(const Char_t* name, const Char_t* objname,
                        const EBrObjSource_t src) :
         TNamed(name,objname), fSrc(src) {}
      virtual ~TSnTreeBldrBranch() {}
      const Char_t* GetBrNm() const { return GetName(); }
      const Char_t* GetObjNm() const { return GetTitle(); }
      virtual void MakeBranch(TTree* tree,
                              const Int_t bufsz, const Int_t split) = 0;
      virtual void CallReqBranch(TSnTreeBuilderMod* m) = 0;
      virtual void CallLoadBranch(TSnTreeBuilderMod* m) = 0;
      virtual Bool_t CopyFrom(const TObject*) = 0;
   };
   
   template<class T>
   struct TSnTypedTreeBldrBranch : public TSnTreeBldrBranch {
      // used to store a local copy of the thing we want in the tree
      // the thing must have a valid assignment operator!
      T*             fObj;
      
      TSnTypedTreeBldrBranch() : fObj(0) {}
      TSnTypedTreeBldrBranch(const Char_t* name,
                             const EBrObjSource_t src) : 
         TSnTreeBldrBranch(name, src), fObj(0) {}
      TSnTypedTreeBldrBranch(const Char_t* name, const Char_t* objname,
                             const EBrObjSource_t src) :
         TSnTreeBldrBranch(name,objname,src), fObj(0) {}
      virtual ~TSnTypedTreeBldrBranch() { delete fObj; }
      virtual void MakeBranch(TTree* t, const Int_t bufsz, const Int_t split) {
         if (fObj==0) {
            fObj = new T;
         }
         t->Branch(GetName(), &fObj, bufsz, split);
      }
      virtual void CallReqBranch(TSnTreeBuilderMod* m) { 
         m->ReqBranch(GetName(), fObj);
      }
      virtual void CallLoadBranch(TSnTreeBuilderMod* m) { 
         m->LoadBranch(GetName());
      }
      virtual Bool_t CopyFrom(const TObject* o) {
         const T* x = dynamic_cast<const T*>(o);
         if (x!=0) {
            if (fObj==0) {
               fObj = new T;
            }
            *fObj = *x;
            return kTRUE;
         }
         return kFALSE;
      }
   };
   
 private:
   TString     fOutTreeNm;    // name of the output tree
   TString     fOutTreeTitle; // title of the output tree
   TTree*      fOt;           // the output tree 
   TObjArray   fBrNames;      // name of each reco obj that should be stored mapped to a local copy of it
   Int_t       fBufsz;        // branch buffer size (default 2^18 = 262144 bytes)
   Int_t       fSplit;        // branch split level (default 1)
   Bool_t      fMakeBranches; //! whether branches need to be made
   
   TSnTreeBldrBranch* FindBrNameObj(const Char_t* kn) const {
      // always stores a TSnTreeBldrBranch, so use static cast
      return static_cast<TSnTreeBldrBranch*>(fBrNames.FindObject(kn));
   }
   void               RemoveOldEntry(const Char_t* n) {
      TSnTreeBldrBranch* b = FindBrNameObj(n);
      if (b!=0) {
         fBrNames.Remove(b);
         delete b;
      }
   }
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();

 public:
   TSnTreeBuilderMod() : fOt(0), fBufsz(262144), fSplit(1),
                         fMakeBranches(kTRUE) {}
   TSnTreeBuilderMod(const Char_t* name,
                     const Char_t* treenm, const Char_t* treetitle,
                     const Int_t   bufsz=262144, const Int_t split=1) :
      TAModule(name, "tree builder module"),
      fOutTreeNm(treenm), fOutTreeTitle(treetitle),
      fOt(0), fBufsz(bufsz), fSplit(split), fMakeBranches(kTRUE) {}
      
   virtual ~TSnTreeBuilderMod() {
      delete fOt;
   }
   
   TString GetOutTreeName() const { return fOutTreeNm; }
   TString GetOutTreeTitle() const { return fOutTreeTitle; }
   Int_t   GetBranchBufferSize() const { return fBufsz; }
   Int_t   GetBranchSplitLevel() const { return fSplit; }
   
   void    SetOutTreeName(const Char_t* n)  { fOutTreeNm = n; }
   void    SetOutTreeTitle(const Char_t* t) { fOutTreeTitle = t; }
   void    SetBranchBufferSize(const Int_t b) { fBufsz = b; }
   void    SetBranchSplitLevel(const Int_t s) { fSplit = s; }
   
   
   template<class T>
   void AddBranchForEvtObj(const Char_t* objname,
                           const Char_t* brname) {
      RemoveOldEntry(brname);
      fBrNames.Add(new TSnTypedTreeBldrBranch<T>(brname, objname, kEvtObj));
   }
   
   template<class T>
   void CopyBranchFromTree(const Char_t* brname) {
      RemoveOldEntry(brname);
      fBrNames.Add(new TSnTypedTreeBldrBranch<T>(brname, kBranch));
   }
   
   
   ClassDef(TSnTreeBuilderMod, 1); // module to dynamically build trees
   
   template<typename T> friend struct TSnTypedTreeBldrBranch;
};

#endif // SNS_TSnTreeBuilderMod
