#ifndef SNS_TSnSmoothDataLstSqrCnvMod
#define SNS_TSnSmoothDataLstSqrCnvMod

#include <TString.h>

#include "TSnCalDatModule.h"
class TSnCalWvData;

class TSnSmoothDataLstSqrCnvMod : public TSnCalDatModule {
 private:
   TString    fDatBrNm;  // name of the input branch (or evt object) containing the TSnCalWvData data
   TString    fIntpNm;   // name of the TObjArray holding the resulting TSnInterp1DLstSqConvo objects in the event, one for each channel
   UInt_t     fN;        // order of Gram polynomials (default: 3)
   UInt_t     fM;        // 2m+1 points per Gram polynomial fit (default: 2 => 5 points per win)
   UInt_t     fS;        // the s'th derivative of the data to find (default: 0 => smooth the data itself)
   Bool_t     fVsTime;   // whether to make the xaxis time (true) or sample (false) (default: false)
   
   // data
   TSnCalWvData* fData;  //! the data to filter
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnSmoothDataLstSqrCnvMod() : fN(3), fM(2), fS(0), fVsTime(kFALSE) {
      // default ctor
   }
   TSnSmoothDataLstSqrCnvMod(const Char_t* name,
                             const Char_t* datbrnm,
                             const Char_t* interpNm,
                             const UInt_t n=3, 
                             const UInt_t m=2, 
                             const UInt_t s=0,
                             const Bool_t vsTime=kFALSE) :
      TSnCalDatModule(name, "Low pass filter using least square convolution"),
      fDatBrNm(datbrnm), fIntpNm(interpNm),
      fN(n), fM(m), fS(s), fVsTime(vsTime) {
      // normal ctor
   }
   virtual ~TSnSmoothDataLstSqrCnvMod() {}
   
   TString GetDataBranchName() const { return fDatBrNm; }
   TString GetInterpDataArrayName() const { return fIntpNm; }
   UInt_t  GetGramPolyOrder() const { return fN; }
   UInt_t  GetMwindowWidthParam() const { return fM; }
   UInt_t  GetDerivativeOrder() const { return fS; }
   Bool_t  IsXaxisTime() const { return fVsTime; }
   
   void    SetDataBranchName(const Char_t* n) { fDatBrNm = n; }
   void    SetInterpDataArrayName(const Char_t* n) { fIntpNm = n; }
   void    SetGramPolyOrder(const UInt_t n) { fN = n; }
   void    SetMwindowWidthParam(const UInt_t m) { fM = m; }
   void    SetDerivativeOrder(const UInt_t s) { fS = s; }
   void    SetXaxisTime(const Bool_t b) { fVsTime = b; }
   
   
   ClassDef(TSnSmoothDataLstSqrCnvMod,1); // add a TSnInterp1DLstSqConvo to the event that has smoothed the data (or its s'th derivative). effectively a low pass filter.
};

#endif // SNS_TSnSmoothDataLstSqrCnvMod
