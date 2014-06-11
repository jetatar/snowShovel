#ifndef SNS_TSnSaveCorlDataMod
#define SNS_TSnSaveCorlDataMod

#include <TString.h>
class TObjArray;
class TTree;

#include "TSnCalDatModule.h"
class TSnCalWvData;
class TSnEventHeader;
class TSnEventMetadata;

class TSnSaveCorlDataMod : public TSnCalDatModule {
 private:
   TString        fEvtBrA;       // name of main event branch - no default
   TString        fCorlNm;       // name of the correlations array (default: CrossChanCorls.)
   TString        fOutTreeNm;    // name of the output tree
   TString        fOutTreeTitle; // title of the output tree
   TString        fAltTag;       // alternate tree tag (no default). if not specified, no alternate event data will be saved
   // metadata is always saved, to allow the tree to be aligned with config trees
   Bool_t         fSaveHeader;   // whether to save the event header (default: true). necessary to align event numbers across trees.
   Bool_t         fSaveData;     // whether to save the data (default: true)
   
   TTree*         fOt;           // the output tree
   Bool_t         fSaveAltEvt;   // whether to save alternate event header and metadata (default: false). useful for, e.g. TTSnRecoSingleChanOffsetSet.
   Int_t          fBufSz;        // buffer size for new branch(es)
   Int_t          fSplit;        // split level for new branch(es)

;   // event data
   TSnCalWvData*     fDatA;   //! the event data
   TSnCalWvData*     fDatB;   //! the event data
   TObjArray*        fCorls;  //! the correlations of this event
   TSnEventHeader*   fHdrA;   //! main event header
   TSnEventHeader*   fHdrB;   //! alternate event header
   TSnEventMetadata* fMetaA;  //! main event metadata
   TSnEventMetadata* fMetaB;  //! main event metadata

   Bool_t      fMakeBranches; // whether branches need to be made (in first event)
   TString        fEvtBrB;    //! name of alternate event branch - no default. set using fAltTag
   TString     fAltMetaBrNm;  // name of the alternate event metadata branch
   TString     fAltHdrBrNm;   // name of the alternate event header branch
   
   
   void MakeBranches(const TObjArray& corls);
   void MakeDatBranch(TSnCalWvData*& dat, const Char_t* brnm);
   void CopyCorls(const TObjArray& corls,
                  const Bool_t makeNew=kFALSE);

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnSaveCorlDataMod() : fSaveHeader(kTRUE), fSaveData(kTRUE), 
                          fOt(0), fSaveAltEvt(kFALSE), fBufSz(262144),
                          fSplit(1), fDatA(0), fDatB(0), fCorls(0),
                          fHdrA(0), fHdrB(0), fMetaA(0), fMetaB(0),
                          fMakeBranches(kTRUE) {}
   TSnSaveCorlDataMod(const Char_t* name, const Char_t* mainDataNm,
                      const Char_t* treeName, const Char_t* treeTitle,
                      const Int_t   bufsz=262144,
                      const Int_t   split=1);
   virtual ~TSnSaveCorlDataMod();
   
   TString  GetMainEventBrName()  const { return fEvtBrA; }
   TString  GetCorrelationsName() const { return fCorlNm; }
   TString  GetTreeName()         const { return fOutTreeNm; }
   TString  GetOutTreeTitle()     const { return fOutTreeTitle; }
   TString  GetAltTreeTag()       const { return fAltTag; }
   Bool_t   HasAltBranch()        const { return fSaveAltEvt; }   
   Bool_t   IsSavingHeaders()     const { return fSaveHeader; }
   Bool_t   IsSavingWvData()      const { return fSaveData; }
   
   void     SetMainEventBrName(const Char_t* n) { fEvtBrA = n; }
   void     SetCorrelationsName(const Char_t* n) { fCorlNm = n; }
   void     SetTreeName(const Char_t* n)     { fOutTreeNm = n; }
   void     SetOutTreeTitle(const Char_t* n) { fOutTreeTitle = n; }
   void     SetAltTreeTag(const Char_t* tag) {
      // set alt tree tag and enable writing of alt evt header/metadata
      fAltTag = tag;
      fSaveAltEvt = !(fAltTag.IsNull());
   }
   void     SetSaveHeaders(const Bool_t s) { fSaveHeader = s; }
   void     SetSaveWvData(const Bool_t s) { fSaveData = s; }
   
   ClassDef(TSnSaveCorlDataMod, 1); // save correlation reco results to a tree
};

#endif // SNS_TSnSaveCorlDataMod
