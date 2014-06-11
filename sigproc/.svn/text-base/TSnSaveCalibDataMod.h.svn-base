#ifndef SNS_TSnSaveCalibDataMod
#define SNS_TSnSaveCalibDataMod

#include <TString.h>
class TTree;
class TBranch;
class TFile;

#include "TAModule.h"

class TSnCalWvData;

class TSnSaveCalibDataMod : public TAModule {
 public:
   static const Char_t* kFTFprf;        // default prefix of FPN sub tree file name
   static const Char_t* kFpnEvtTreeNm;  // name of FPN sub event tree
   static const Char_t* kCTFprf;        // default prefix of cal tree file name
   static const Char_t* kCalEvtTreeNm;  // name of calib event tree
   static const Char_t* kFPNSubDatBrNm; // name of the FPN sub wvfm branch
   static const Char_t* kAmpOutDatBrNm; // name of the digitizer calib branch

 private:
   TString fFPNSubDatNm; // name of FPN sub wvfm data in event (i.e. from TSnFPNSubMod) (default "FPNSubData")
   Bool_t  fSaveFPNSub;  // whether or not to save FPN subtracted data (default: true)
   TString fAmpOutDatNm; // name of digitizer calibrated wvfm data in event (i.e. from TSnDigitizerCalibMod) (default "AmpOutData")
   Bool_t  fSaveAmpOut;  // whether or not to save digitizer calib data (default: true)
   TString fOutdir;      // output file directory
   TString fOutprf;      // output file prefix
   Int_t   fBufsz;       // buffer size for new branch(es)
   Int_t   fSplit;       // split level for new branch(es)
   TString fOutTreeTitle;// title of output tree (default: (semi-)calibrated waveform events)
   
   // for use during event
   TSnCalWvData* fFPNSubDat; //! the FPN subtracted data
   TSnCalWvData* fAmpOutDat; //! the digitizer calib data

   TTree*        fOt;        //! the output event data tree
   TFile*        fOutf;      //! the output file
   TBranch*      fFPNSubBr;  //! the FPN sub data branch
   TBranch*      fAmpOutBr;  //! the digitizer calib data branch

   void         CopyAllKeys(const TFile* inf);
   TString      GetTreeFilename(const TString infn) const;
   
 protected:
   virtual Bool_t Notify();
   virtual void   Process();
   virtual void   SlaveTerminate();
   
 public:
   TSnSaveCalibDataMod() : fSaveFPNSub(kTRUE), fSaveAmpOut(kTRUE),
                           fBufsz(262144), fSplit(1),
                           fFPNSubDat(0), fAmpOutDat(0), fOt(0),
                           fOutf(0), fFPNSubBr(0), fAmpOutBr(0) {}
   TSnSaveCalibDataMod(const Char_t* name,
                       const Char_t* outdir,
                       const Char_t* outpre=TSnSaveCalibDataMod::kCTFprf,
                       const Char_t* fpnsubnm="FPNSubData",
                       const Char_t* ampoutnm="AmpOutData",
                       const Int_t   bufsz=262144,
                       const Int_t   split=1);
   virtual ~TSnSaveCalibDataMod();

   const Char_t* GetOutputDirectory() const { return fOutdir.Data(); }
   const Char_t* GetOutputPrefix() const { return fOutprf.Data(); }
   const Char_t* GetFPNSubDatNm() const { return fFPNSubDatNm.Data(); }
   const Char_t* GetAmpOutDatNm() const { return fAmpOutDatNm.Data(); }
   Bool_t        IsSavingFPNSubData() const { return fSaveFPNSub; }
   Bool_t        IsSavingAmpOutData() const { return fSaveAmpOut; }
   Int_t         GetBranchBufferSize() const { return fBufsz; }
   Int_t         GetBranchSplitLevel() const { return fSplit; }
   const Char_t* GetOutputTreeTitle() const { return fOutTreeTitle; }
   
   void          SetOutputDirectory(const Char_t* d) { fOutdir = d; }
   void          SetOutputPrefix(const Char_t* p) { fOutprf = p; }
   void          SetFPNSubDatNm(const Char_t* n) { fFPNSubDatNm = n; }
   void          SetAmpOutDatNm(const Char_t* n) { fAmpOutDatNm = n; }
   void          SetSaveFPNSubData(const Bool_t s) { fSaveFPNSub = s; }
   void          SetSaveAmpOutData(const Bool_t s) { fSaveAmpOut = s; }
   void          SetBranchBufferSize(const Int_t s) { fBufsz = s; }
   void          SetBranchSplitLevel(const Int_t s) { fSplit = s; }
   void          SetOutputTreeTitle(const Char_t* t) { fOutTreeTitle = t; }
   
   ClassDef(TSnSaveCalibDataMod, 2); // module to save FPN subtracted and/or fully calibrated data. should NOT be run under Proof
};

#endif // SNS_TSnSaveCalibDataMod
