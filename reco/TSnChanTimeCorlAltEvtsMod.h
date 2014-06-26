#ifndef SNS_TSnChanTimeCorlAltEvtsMod
#define SNS_TSnChanTimeCorlAltEvtsMod

#include <TString.h>

#include "TSnCalDatModule.h"
class TSnCalWvData;

class TSnChanTimeCorlAltEvtsMod : public TSnCalDatModule {
 public:
   enum ECorlType_t {
      kWrapCorl,     // use TSnSpectral::NewCorrelateGraph
      kScanToEnvMax  // use TSnSpectral::NewScanPrsCorrCoefGraphUneq on start of waveform up to envelope max. requires stop shifted to end and envelope to be in event
   };

   static const UInt_t kDebugCorls; // verbosity level at which to display debugging plots each event
   
 private:
   TString           fEvtBrA; // name of event branch A - no default
   TString           fEvtBrB; // name of event branch B (need not be in same tree if a loader is being used) - no default
   TString           fCorlNm; // name of output correlations (default: SingleChanCorls.)
   TSnChanTimeCorlAltEvtsMod::ECorlType_t fCorlType; // type of correlation (default kWrapCorl)
   TString           fEnvNmA; // name of envelope for event A - no default (only for corl types that need it)
   TString           fEnvNmB; // name of envelope for event B - no default (only for corl types that need it)
   
   // event data
   TSnCalWvData*     fDatA;   //! the event data
   TSnCalWvData*     fDatB;   //! the event data

 protected:
   virtual void SlaveBegin();
   virtual void Process();

 public:
   TSnChanTimeCorlAltEvtsMod() : fCorlType(kWrapCorl), fDatA(0), fDatB(0) {}
   TSnChanTimeCorlAltEvtsMod(const Char_t* name) :
      TSnCalDatModule(name, "calc single channel correlations "
                      "in alterante events"),
      fCorlNm("SingleChanCorls."),
      fCorlType(kWrapCorl),
      fDatA(0), fDatB(0) {}
   TSnChanTimeCorlAltEvtsMod(const Char_t* name,
                             const Char_t* mainEvtBrNm,
                             const Char_t* altEvtBrNm) :
      TSnCalDatModule(name, "calc single channel correlations "
                      "in alterante events"),
      fEvtBrA(mainEvtBrNm), fEvtBrB(altEvtBrNm),
      fCorlNm("SingleChanCorls."), 
      fCorlType(kWrapCorl),
      fDatA(0), fDatB(0) {}
   virtual ~TSnChanTimeCorlAltEvtsMod() {}

   static
   TString    GetChanCorlInterpName(const UChar_t ch) {
      TString a = Form("CorlAltEvtsCh%dSpl3",ch);
      return a;
   }

   Bool_t IsCheckingEnvelope() const {
      return (fCorlType == kScanToEnvMax);
   }
   
   TString  GetMainEventBrName() const { return fEvtBrA; }
   TString  GetAltEventBrName() const { return fEvtBrB; }
   TString  GetCorrelationsName() const { return fCorlNm; }
   TString  GetEnvelopeName() const { return fEnvNmA; }
   TString  GetAltEnvelopeName() const { return fEnvNmB; }
   TSnChanTimeCorlAltEvtsMod::ECorlType_t GetCorlType() const
      { return fCorlType; }

   void     SetMainEventBrName(const Char_t* n) { fEvtBrA = n; }
   void     SetAltEventBrName(const Char_t* n) { fEvtBrB = n; }
   void     SetCorrelationsName(const Char_t* n) { fCorlNm = n; }
   void     SetEnvelopeName(const Char_t* n) { fEnvNmA = n; }
   void     SetAltEnvelopeName(const Char_t* n) { fEnvNmB = n; }
   void     SetCorlType(const TSnChanTimeCorlAltEvtsMod::ECorlType_t t)
      { fCorlType = t; }
   void     SetCorlType(const TString t);
   
   static
   const Char_t* GetCorlTypeStr(const TSnChanTimeCorlAltEvtsMod::ECorlType_t t);
   
   
   ClassDef(TSnChanTimeCorlAltEvtsMod, 2); // calculate correlation between waveforms in the same channel of different events
};

#endif // SNS_TSnChanTimeCorlAltEvtsMod
