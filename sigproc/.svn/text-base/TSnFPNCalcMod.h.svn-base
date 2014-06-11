#ifndef SNS_TSnFPNCalcMod
#define SNS_TSnFPNCalcMod

#include <TString.h>

#include "TAModule.h"
class TH2;

class TSnFPNCalibSet;
class TSnRawWaveform;

class TSnFPNCalcMod : public TAModule {
 private:
   TSnFPNCalibSet*     fFpnSet;  // the resulting fpn set
   Float_t             fPeakFrc; // fraction of the max peak to step to (def: 0.05)
   TString             fFpnSetNm;// name of the resulting fpn set
   Bool_t              fGausFit; // if true, fit with a gaussian to find mean/rms (def: false)
   
   // hist
   Int_t               fSbins;   // number of sample bins
   Float_t             fSmin;    // min sample
   Float_t             fSmax;    // max sample
   Int_t               fAbins;   // number of ADC bins
   Float_t             fAmin;    // min adc
   Float_t             fAmax;    // max adc
   TH2*                fHadcs;   //! histogram of adc values on each sample

   // event data
   TSnRawWaveform*     fRaw;     //! the raw waveform data


 protected:
   virtual void        SlaveBegin();
   virtual void        Process();
   virtual void        Terminate();

 public:
   TSnFPNCalcMod() : fFpnSet(0), fPeakFrc(0.05), fGausFit(kFALSE),
                     fSbins(0), fSmin(0), fSmax(0),
                     fAbins(0), fAmin(0), fAmax(0), fHadcs(0), fRaw(0) {}
   TSnFPNCalcMod(const Char_t* name,
                 const Char_t* fpnsetname);
   virtual ~TSnFPNCalcMod();

   const TH2*       GetAdcHist() const { return fHadcs; }
   // intentionally no non-const hist getter. make a copy to draw it.

   const TSnFPNCalibSet* GetFpnSet() const { return fFpnSet; }
   const Char_t*    GetFpnSetName() const { return fFpnSetNm.Data(); }

   Bool_t           IsGausFitting() const { return fGausFit; }

   Float_t          GetPeakFrc() const { return fPeakFrc; }
   void             SetPeakFrc(const Float_t f) { fPeakFrc = f; }

   void             SetFpnSetName(const Char_t* n) { fFpnSetNm = n; }
   void             SetGausFit(const Bool_t b) { fGausFit = b; }
   
   void        SetSampleAxis(const Int_t n, 
                             const Float_t min, const Float_t max) {
      fSbins = n; fSmin = min; fSmax = max;
   }
   
   void        SetAdcAxis(const Int_t n,
                          const Float_t min, const Float_t max) {
      fAbins = n; fAmin = min; fAmax = max;
   }
   
   ClassDef(TSnFPNCalcMod, 2); // module to calculate FPN from raw data
};

#endif // SNS_TSnFPNCalcMod
