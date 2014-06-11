#ifndef SNS_TSnFitChanOffsetMod
#define SNS_TSnFitChanOffsetMod

#include <TString.h>
class TObjArray;

#include "TSnFittingModule.h"
#include "NSnConstants.h"

class TSnFitChanOffsetMod : public TSnFittingModule {
 private:
   TString fCorlNm; // name of input channel correlations (default: CrossChanCorls.)
   TString fDtsNm;  // name of output time offset object (default: ChanTimeOffsets.)
   Float_t fCsig;   // sigma of gaussian that determines how strict the time constraints are applied (default: 1/(2*sampRate))
   
   // event data
   const TObjArray* fCorl; //! the correlation functions
         TObjArray* fDts;  //! the output time offsets for this event

 protected:
   void Process();
   
 public:
   TSnFitChanOffsetMod() : fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
                           fCorl(0), fDts(0) {}
   TSnFitChanOffsetMod(const Char_t* name,
                       const Char_t* miniName="Minuit2",
                       const Char_t* algoName="Migrad") :
      TSnFittingModule(name, "fit chan offsets in same event",
                       miniName, algoName),
      fCorlNm("CrossChanCorls."),
      fDtsNm("ChanTimeOffsets."),
      fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
      fCorl(0), fDts(0) {
      // normal ctor
   }
   virtual ~TSnFitChanOffsetMod() {}
   
   Double_t GetChanDelayLL(const Double_t* dts) const;
   
   TString  GetTimeOffsetsName() const { return fDtsNm; }
   TString  GetCorrelationsName() const { return fCorlNm; }
   Float_t  GetCorlConstraintSigma() const { return fCsig; }
   
   void     SetTimeOffsetsName(const Char_t* n) { fDtsNm = n; }
   void     SetCorrelationsName(const Char_t* n) { fCorlNm = n; }
   void     SetCorlConstraintSigma(const Float_t c) { fCsig = c; }

   static
   TString GetParName(const UChar_t ch, const UChar_t xc) {
      TString t(Form("Dt_%hhu_%hhu",ch,xc));
      return t;
   }   
   
   
   ClassDef(TSnFitChanOffsetMod, 2); // fits channel offsets between chans in the same event
};

#endif // SNS_TSnFitChanOffsetMod
