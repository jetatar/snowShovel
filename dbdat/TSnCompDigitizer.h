#ifndef SNS_TSnCompDigitizer
#define SNS_TSnCompDigitizer

#include <TString.h>

#include "TSnComponent.h"

class TSnCompDigitizer : public TSnComponent {
 private:
   Int_t          fNum;    // the digitizer number
   TString        fModel;  // the digitizer model
   
   static const Char_t*  kCompNm;
   
 public:
   TSnCompDigitizer() : fNum(-1) {}
   virtual ~TSnCompDigitizer() {}
   
   Bool_t         IsValidForDb() const
      { return (fNum>=0) && (fModel.Length()>0); }
   
   Int_t          GetNum()       const { return fNum; }
   const Char_t*  GetModel()     const { return fModel.Data(); }
   
   void           SetNum(const Int_t n) { fNum = n; }
   void           SetModel(const Char_t* m) { fModel = m; }
   
   const Char_t*  GetCompType()  const { return kCompNm; }
  
   ClassDef(TSnCompDigitizer, 1); // a digitizer hardware component
};

#endif // SNS_TSnCompDigitizer
