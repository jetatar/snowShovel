#ifndef SNS_TSnFitChanOffsetAltEvtsMod
#define SNS_TSnFitChanOffsetAltEvtsMod

#include <TString.h>
class TObjArray;
class TNtuple;

#include "TSnFittingModule.h"
class TSnInterp1DWvData;
class TSnEventMetadata;
class TSnEventHeader;

class TSnFitChanOffsetAltEvtsMod : public TSnFittingModule {
 public:
   enum EPars_t {
      kDt,
      kNpars
   };
   
   enum EFitType_t {
      kCorlOnly,           // use only correlation function
      kEnvRiseDistOnly,    // use only the envelope rise distance
      kCorlAndEnvRiseDist, // use both correlation func and env rise dist
      kEnvMatchOnly,       // use only the envelope matching
      kCorlAndEnvMatch     // use both correlation func and env matching
   };
   
   static const UInt_t  kMakeScanNtp; // verbosity level at which an ntuple with a scan of the fit space will be recorded
   static const UInt_t  kDebugFit;    // verbosity level at which fit debugging plots will be drawn each event
  
 private:
   TString fCorlNm; // name of input channel correlations (default: SingleChanCorls.)
   TString fDtsNm;  // name of output time offset object (default: SingleChanTimeOffsets.)
   TString fEnvNmA; // name of envelope obj in the event for run A
   TString fEnvNmB; // name of envelope obj in the event for run B
   TSnFitChanOffsetAltEvtsMod::EFitType_t fFitType; // type of fit to perform, see EFitType_t enum (default: kCorlOnly)
   Float_t fEnvNoiseCut; // y-value at which the start envelope rise (default: 40)
   Float_t fEnvMaxFrac;  // fractional y-value at which to stop envelope rise (default: 0.95)
   
   // event data
   const TSnInterp1DWvData* fCorl; //! the correlation fcn for the current chan
   TObjArray* fDts;                //! the output time offsets for this event
   const Float_t* fEnvA;           //! envelope data
   const Float_t* fEnvB;           //! envelope data (alt event)
   TSnInterp1DWvData* fEnvCorl;    //! envelope matching correlation fcn
   
   // for debugging
   TSnEventMetadata* fMeta;
   TSnEventHeader*   fHdr;
   TNtuple*          fNt;

   Double_t GetCorlDelayLL(const Double_t* dts) const;
   Double_t GetEnvRiseDistLL(const Double_t* dts) const;
   Double_t GetCorlAndEnvRiseDistDelayLL(const Double_t* dts) const;
   Double_t GetEnvMatchLL(const Double_t* dts) const;
   Double_t GetCorlAndEnvMatchLL(const Double_t* dts) const;
   void     SetMiniFitFunction();
   
 protected:
   void SlaveBegin();
   void Process();

 public:
   TSnFitChanOffsetAltEvtsMod() : fFitType(kCorlOnly),
                                  fEnvNoiseCut(40), fEnvMaxFrac(0.95),
                                  fCorl(0), fDts(0),
                                  fMeta(0), fHdr(0), fNt(0) {}
   TSnFitChanOffsetAltEvtsMod(const Char_t* name,
                              const TSnFitChanOffsetAltEvtsMod::EFitType_t ftype
                                 =kCorlOnly,
                              const Char_t* miniName="Minuit2",
                              const Char_t* algoName="Migrad") :
      TSnFittingModule(name, "fit chan offsets from alternate events mod",
                       miniName, algoName),
      fCorlNm("SingleChanCorls."),
      fDtsNm("SingleChanTimeOffsets."),
      fFitType(ftype),
      fEnvNoiseCut(40), fEnvMaxFrac(0.95),
      fCorl(0), fDts(0),
      fMeta(0), fHdr(0), fNt(0) {
      // normal ctor useful for correlation only
   }
   TSnFitChanOffsetAltEvtsMod(const Char_t* name,
                              const Char_t* envName,
                              const Char_t* altEnvName,
                              const TSnFitChanOffsetAltEvtsMod::EFitType_t ftype
                                 =kCorlAndEnvRiseDist,
                              const Char_t* miniName="Minuit2",
                              const Char_t* algoName="Migrad") :
      TSnFittingModule(name, "fit chan offsets from alternate events mod",
                       miniName, algoName),
      fCorlNm("SingleChanCorls."),
      fDtsNm("SingleChanTimeOffsets."),
      fEnvNmA(envName), fEnvNmB(altEnvName),
      fFitType(ftype),
      fEnvNoiseCut(40), fEnvMaxFrac(0.95),
      fCorl(0), fDts(0),
      fMeta(0), fHdr(0), fNt(0) {
      // normal ctor useful for envelopes and correlations
   }
   virtual ~TSnFitChanOffsetAltEvtsMod();

   TString  GetTimeOffsetsName() const { return fDtsNm; }
   TString  GetCorrelationsName() const { return fCorlNm; }
   TString  GetEnvelopeName() const { return fEnvNmA; }
   TString  GetAltEnvelopeName() const { return fEnvNmB; }
   Float_t  GetEnvNoiseCut() const { return fEnvNoiseCut; }
   Float_t  GetEnvMaxFrac() const { return fEnvMaxFrac; }
   TSnFitChanOffsetAltEvtsMod::EFitType_t GetFitType() const 
      { return fFitType; }
   
   Bool_t IsCheckingCorl() const {
      return ( (fFitType==kCorlOnly) || (fFitType==kCorlAndEnvRiseDist)
               || (fFitType==kCorlAndEnvMatch) );
   }
   Bool_t IsCheckingEnvelope() const {
      return (    (fFitType==kEnvRiseDistOnly)
               || (fFitType==kCorlAndEnvRiseDist)
               || (fFitType==kEnvMatchOnly)
               || (fFitType==kCorlAndEnvMatch) );
   }
   Bool_t IsCheckingEnvRiseDist() const {
      return IsCheckingEnvelope();
   }
   
   
   void     SetTimeOffsetsName(const Char_t* n) { fDtsNm = n; }
   void     SetCorrelationsName(const Char_t* n) { fCorlNm = n; }
   void     SetEnvelopeName(const Char_t* n) { fEnvNmA = n; }
   void     SetAltEnvelopeName(const Char_t* n) { fEnvNmB = n; }
   void     SetFitType(const TSnFitChanOffsetAltEvtsMod::EFitType_t t)
      { fFitType = t; }
   void     SetEnvNoiseCut(const Float_t c) { fEnvNoiseCut = c; }
   void     SetEnvMaxFrac(const Float_t m) { fEnvMaxFrac = m; }
   void     SetFitType(const TString t);
   
   static
   const Char_t* GetFitTypeStr(const TSnFitChanOffsetAltEvtsMod::EFitType_t t);
   
   static
   const Char_t* GetParName(const TSnFitChanOffsetAltEvtsMod::EPars_t x) {
      switch (x) {
         case kDt:
            return "Dt";
         default:
            return "UNKNOWN";
      };
   }
   
   ClassDef(TSnFitChanOffsetAltEvtsMod, 2); // fits channel offsets between the same channel in different events
};

#endif // SNS_TSnFitChanOffsetAltEvtsMod
