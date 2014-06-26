#ifndef SNS_TSnWaveletDenoiseMod
#define SNS_TSnWaveletDenoiseMod

#include <TString.h>

#include "TSnCalDatModule.h"
class TSnWavelet;
class TSnCalWvData;

class TSnWaveletDenoiseMod : public TSnCalDatModule {
 public:
   enum EThreshType {
      kHardThresh, // set coef to 0 if below thresh, else untouched
      kSoftThresh  // set coef to 0 if below thresh, else reduce by threshold
   };
   
 private:
   TSnWavelet*  fWv;      // the wavelet used to do the transformation (default: daub4 type wavelet)
   Float_t      fThresh;  // threshold below which coefficients will be ignored (defaults to noise * sqrt(2 * log10(N_samples)) where noise = 20.0 mV
   EThreshType  fThrType; // how to apply the threshold filter (see EThreshType) (default: kHardThresh)
   TString      fDatBrNm; // name of data branch or event object to de-noise (no default!)
   TString      fOutDatNm;// name of output denoised TSnCalWvData object in the event (no default!)
   
   // event objs
   TSnCalWvData* fDat;    //! the data to denoise
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();

 public:
   TSnWaveletDenoiseMod();
   TSnWaveletDenoiseMod(const Char_t* name,
                        const Char_t* datbrnm, const Char_t* outdatnm);
   virtual ~TSnWaveletDenoiseMod();
   
   const TSnWavelet* GetWavelet() const { return fWv; }
         TSnWavelet* GetWavelet()       { return fWv; }
   Float_t           GetThreshold() const { return fThresh; }
   TSnWaveletDenoiseMod::EThreshType
                     GetThreshType() const { return fThrType; }
   TString           GetDataBrName() const { return fDatBrNm; }
   TString           GetDenoisedDataName() const { return fOutDatNm; }
   
   void              SetWavelet(const Char_t* wvClassName);
   void              SetWavelet(const TSnWavelet& wv, 
                                const Bool_t buildWavelet=kTRUE);
   void              SetThreshold(const Float_t t) { fThresh = t; }
   void              SetThreshType(const TSnWaveletDenoiseMod::EThreshType t)
      { fThrType = t; }
   void              SetDataBrName(const Char_t* n) { fDatBrNm = n; }
   void              SetDenoisedDataName(const Char_t* n) { fOutDatNm = n; }
   
   ClassDef(TSnWaveletDenoiseMod, 1); // module to denoise a waveform using discrete wavelet transforms

};

#endif // SNS_TSnWaveletDenoiseMod
