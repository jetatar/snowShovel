#ifndef SNS_TSnSaveRecoResultsMod
#define SNS_TSnSaveRecoResultsMod

#include <TString.h>
#include <TMap.h>
class TTree;

#include "TAModule.h"

class TSnRecoResult;
class TSnEventHeader;
class TSnEventMetadata;

class TSnSaveRecoResultsMod : public TAModule {
 public:
   struct TSnRecoResultWrapper : public TObject {
      // this is just to make a pointer to result that's on the heap
      // so it will stick around and can be passed to TTree::Branch
      TObject* fRR; // the reco result
      TSnRecoResultWrapper() : fRR(0) {}
      TSnRecoResultWrapper(TObject* x) : fRR(x) {}
      virtual ~TSnRecoResultWrapper() {}
      ClassDef(TSnRecoResultWrapper, 1); // wrapper for a TSnRecoResult
   };

 private:
   TString     fOutTreeNm;    // name of the output tree
   TString     fOutTreeTitle; // title of the output tree
   TTree*      fOt;           // the output tree
   TMap        fRecoObjNames; // name of each reco obj that should be stored (mapped to a local copy of it)
   TString     fAltTag;       // alternate tree tag (no default)
   Bool_t      fSaveAltEvt;   // whether to save alternate event header and metadata (default: false). useful for, e.g. TTSnRecoSingleChanOffsetSet.
   // metadata is always saved, to allow the tree to be aligned with other trees
   Bool_t      fSaveHeader;   // whether to save the event header (default: true). necessary to align event numbers across trees.
   Int_t       fBufSz;        // buffer size for new branch(es)
   Int_t       fSplit;        // split level for new branch(es)
   
   // internal vars
   Bool_t      fMakeBranches; // whether branches need to be made (in first event)
   TString     fAltMetaBrNm;  // name of the alternate event metadata branch
   TString     fAltHdrBrNm;   // name of the alternate event header branch
   TSnEventHeader*   fHdrA;   //! main event header
   TSnEventHeader*   fHdrB;   //! alternate event header
   TSnEventMetadata* fMetaA;  //! main event metadata
   TSnEventMetadata* fMetaB;  //! main event metadata
   
   
   void MakeBranches();
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnSaveRecoResultsMod() : fOt(0), fSaveAltEvt(kFALSE), fSaveHeader(kTRUE),
                             fBufSz(262144), fSplit(1),
                             fMakeBranches(kTRUE),
                             fHdrA(0), fHdrB(0), fMetaA(0), fMetaB(0) {
      // default ctor
      fRecoObjNames.SetOwnerKeyValue(kTRUE,kTRUE);
   }
   TSnSaveRecoResultsMod(const Char_t* name,
                         const Char_t* treeName, const Char_t* treeTitle,
                         const Int_t   bufsz=262144,
                         const Int_t   split=1);
   virtual ~TSnSaveRecoResultsMod();
   
   
   // output tree accessible via the TAMOutput
   TString     GetTreeName()        const { return fOutTreeNm; }
   TString     GetOutTreeTitle()    const { return fOutTreeTitle; }
   Bool_t      HasAltBranch()       const { return fSaveAltEvt; }   
   Bool_t      IsSavingHeaders()    const { return fSaveHeader; }
   const TMap& GetRecoResultNames() const { return fRecoObjNames; }
   // intentionally no getter for fMakeBranches
   
   void        SetTreeName(const Char_t* n)     { fOutTreeNm = n; }
   void        SetOutTreeTitle(const Char_t* n) { fOutTreeTitle = n; }
   void        SetSaveHeaders(const Bool_t s)   { fSaveHeader = s; }
   // intentionally no setter for the tree
   // intentionally no setter for the entire map
   void        SetAltTreeTag(const Char_t* tag) {
      // set alt tree tag and enable writing of alt evt header/metadata
      fAltTag = tag;
      fSaveAltEvt = !(fAltTag.IsNull());
   }
   void        SaveRecoResult(const Char_t* name);
   
   
   ClassDef(TSnSaveRecoResultsMod, 1); // save reconstruction results to a tree
};

#endif // SNS_TSnSaveRecoResultsMod
