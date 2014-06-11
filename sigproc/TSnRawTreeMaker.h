#ifndef SNS_TSnRawTreeMaker
#define SNS_TSnRawTreeMaker

#include <TObject.h>
#include <TString.h>
class TFile;
class TTree;

// this file may be included by many modules in order to get branch name
// variables. so we don't want to have too many #include's in this header
// (or .o files will get really big and compiling will slow down).
// lots of forward declarations are ok tho.
class TSnDAQConfig;
class TSnRunInfo;
class TSnNetAdrSet;
class TSnCompMbed;
class TSnEvent;
class TSnPowerReading;
class TSnHeartbeat;
class TSnClockSetInfo;
class TSnEventMetadata;

#include <cstdio>

class TSnRawTreeMaker : public TObject {
 public:
   static const Char_t* kRTFprf;    // default raw tree filename prefix
   static const Char_t* kVTname;    // voltage tree name
   static const Char_t* kHTname;    // heartbeat tree name
   static const Char_t* kETname;    // event tree name
   static const Char_t* kCTname;    // config tree name
   static const Char_t* kPwrBrNm;   // name of TSnPowerReading in voltage tree
   static const Char_t* kHrtBrNm;   // name of TSnHeartbeat in heartbeat tree
   static const Char_t* kEMtBrNm;   // name of TSnEventMetadata in event tree
   static const Char_t* kEHdBrNm;   // name of TSnEventHeader in event tree
   static const Char_t* kEWvBrNm;   // name of TSnRawWaveform in event tree
   static const Char_t* kCMtBrNm;   // name of TSnEventMetadata in config tree
   static const Char_t* kCnfBrNm;   // name of TSnDAQConfig in config tree
   static const Char_t* kRunBrNm;   // name of TSnRunInfo in config tree
   static const Char_t* kNetBrNm;   // name of TSnNetAdrSet in config tree
   static const Char_t* kCSrBrNm;   // name of trigger start in config tree
   static const Char_t* kCSpBrNm;   // name of trigger stop in config tree
   
 private:
   TString         fInfn;  // input filename
   TString         fOdir;  // output directory
   TString         fOpre;  // output prefix
   Int_t           fBufsz; // branch buffer size (default 2^18 = 262144 bytes)
   Int_t           fSplit; // branch split level (default 1)
   Int_t           fCompr; // compression level (default 2, so split branches are compressed)
   
   // temp variables
   FILE*           fInf;   //! input file
   TFile*          fOutf;  //! output file

   // the trees
   TTree*           fVt; //! the voltage tree
   TTree*           fHt; //! the heartbeat tree
   TTree*           fCt; //! the heartbeat tree
   TTree*           fEt; //! the event tree
   
   // branch variables
   TSnEvent*        fEvt; //! current event
   TSnPowerReading* fPwr; //! current voltage
   TSnHeartbeat*    fHrt; //! current heartbeat
   TSnEventMetadata* fEvtMeta; //! event metadata
   TSnDAQConfig*    fConf;  //! daq config
   TSnRunInfo*      fRunf;  //! run info
   TSnNetAdrSet*    fNets;  //! internet addresses
   TSnRunInfo*      fHrun;  //! header run info (just the number)
   TSnCompMbed*     fComp;  //! mbed info (mac address)
   UShort_t         fSeq;   //! seq number
   TSnClockSetInfo* fClkStrt; //! clock set at trg start
   TSnClockSetInfo* fClkStop; //! clock set at trg stop
   
   Bool_t OpenFiles();
   void   MakeTrees();
   void   LoopBlocks();
   
 public:
   TSnRawTreeMaker() : fBufsz(262144), fSplit(1), fCompr(2), fInf(0), fOutf(0),
                       fVt(0), fHt(0), fCt(0), fEt(0),
                       fEvt(0), fPwr(0), fHrt(0), fEvtMeta(0),
                       fConf(0), fRunf(0), fNets(0), fHrun(0),
                       fComp(0), fSeq(0), fClkStrt(0), fClkStop(0) {}
   TSnRawTreeMaker(const Char_t* infn,
                   const Char_t* outdir,
                   const Char_t* outpre=TSnRawTreeMaker::kRTFprf,
                   const Int_t   brBufSize=262144,
                   const Int_t   split=1,
                   const Int_t   compr=2);
   virtual ~TSnRawTreeMaker();
   
   void     ProcFile();
   
   TString  GetTreeFilename() const;
   Long64_t GetNevtsWritten() const;
   Long64_t GetNpwrsWritten() const;
   Long64_t GetNhrtsWritten() const;

   // no getters for the file or trees in order to avoid dangling pointers
   
   // intentionally no setters
   
   ClassDef(TSnRawTreeMaker, 4); // make a run tree from a raw data file
};

#endif // SNS_TSnRawTreeMaker
