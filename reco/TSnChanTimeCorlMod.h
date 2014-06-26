#ifndef SNS_TSnChanTimeCorlMod
#define SNS_TSnChanTimeCorlMod

#include <TString.h>
class TObjArray;

#include "TSnCalDatModule.h"
class TSnCalWvData;

class TSnChanTimeCorlMod : public TSnCalDatModule {
 public:
   enum ECorlType_t {
      kWrapCorl,     // use TSnSpectral::NewCorrelateGraph
      kScanToEnvMax  // use TSnSpectral::NewScanPrsCorrCoefGraphUneq on start of waveform up to envelope max. requires stop shifted to end and envelope to be in event
   };

   static const UInt_t kDebugCorls; // verbosity at which to draw debug plots

 private:
   TString        fWvDatNm; // name of the calibrated wv data branch or object: i.e. FPN sub, amp out, filtered, etc. (no default!)
   TString        fCorlNm;  // name of the output correlations array (default: CrossChanCorls.)
   TString        fEnvNm; // name of envelope for event - no default (only for corl types that need it)
   TSnChanTimeCorlMod::ECorlType_t fCorlType; // type of correlation (default kWrapCorl)
   
   // event data
   TSnCalWvData*  fDat;   //! the event data
   TObjArray*     fCorls; //! the output correlations of this event
   
 protected:
   virtual void  SlaveBegin();
   virtual void  Process();

 public:
   TSnChanTimeCorlMod() : fCorlType(kWrapCorl), fDat(0), fCorls(0) {}
   TSnChanTimeCorlMod(const Char_t* name, const Char_t* wvdatnm) :
      TSnCalDatModule(name, "chan time correlations mod"),
      fWvDatNm(wvdatnm),
      fCorlNm("CrossChanCorls."),
      fCorlType(kWrapCorl),
      fDat(0), fCorls(0) { // normal ctor
   }
   virtual ~TSnChanTimeCorlMod() {}
   
   static
   TString    GetChanCorlInterpName(const UChar_t ch, const UChar_t xc) {
      TString a = Form("CorlCh%dCh%dSpl3",ch,xc);
      return a;
   }

   Bool_t IsCheckingEnvelope() const {
      return (fCorlType == kScanToEnvMax);
   }
   
   TString    GetWvDataName() const { return fWvDatNm; }
   TString    GetChanCorlName() const { return fCorlNm; }
   TString    GetEnvelopeName() const { return fEnvNm; }
   TSnChanTimeCorlMod::ECorlType_t GetCorlType() const { return fCorlType; }
   
   void       SetWvDataName(const Char_t* n) { fWvDatNm = n; }
   void       SetChanCorlName(const Char_t* n) { fCorlNm = n; }
   void       SetEnvelopeName(const Char_t* n) { fEnvNm = n; }
   void       SetCorlType(const TSnChanTimeCorlMod::ECorlType_t t)
      { fCorlType = t; }
   void       SetCorlType(const TString t);
   
   static
   const Char_t* GetCorlTypeStr(const TSnChanTimeCorlMod::ECorlType_t t);
   
   ClassDef(TSnChanTimeCorlMod, 2); // find channel correlation vs sample functions
};

#endif // SNS_TSnChanTimeCorlMod
