#ifndef SNS_TSnFFTCalcMod
#define SNS_TSnFFTCalcMod

#include <TString.h>

#include "TSnCalDatModule.h"
class TSnCalWvData;
class TSnCalFFTData;

class TSnFFTCalcMod : public TSnCalDatModule {
 private:
   TString        fDatBrNm; // name of the input branch containing the data
   TString        fFftNm;   // name of the resulting FFT object
   
   // event data
   TSnCalWvData*  fDat;     //! the data

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnFFTCalcMod() : fDat(0) {}
   TSnFFTCalcMod(const Char_t* name, 
                 const Char_t* datBrNm,
                 const Char_t* fftNm) :
      TSnCalDatModule(name, "mod to calculate FFTs"),
      fDatBrNm(datBrNm),
      fFftNm(fftNm),
      fDat(0) {
      // normal ctor
   }
   virtual ~TSnFFTCalcMod() {}
   
   
   TString        GetDataBranchName() const { return fDatBrNm; }
   TString        GetFFTName() const { return fFftNm; }
   
   void           SetDataBranchName(const Char_t* n) { fDatBrNm = n; }
   void           SetFFTName(const Char_t* n) { fFftNm = n; }
   
   
   ClassDef(TSnFFTCalcMod, 1); // module to calculate FFTs for each chan and add them to the event
};

#endif // SNS_TSnFFTCalcMod
