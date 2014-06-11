#ifndef SNS_TSnCompFpga
#define SNS_TSnCompFpga

#include <TString.h>

#include "TSnComponent.h"

class TSnCompFpga : public TSnComponent {
 private:
   Int_t       fNum;    // the fpga number
   TString     fModel;  // the fpga model
   
   static const Char_t*     kCompNm;
   
 public:
   TSnCompFpga() : fNum(-1) {}
   virtual ~TSnCompFpga() {}
   
   Bool_t         IsValidForDb() const
      { return (fNum>=0) && (fModel.Length()>0); }
   
   Int_t          GetNum()   const { return fNum; }
   const Char_t*  GetModel() const { return fModel.Data(); }
   
   void           SetNum(const Int_t n) { fNum = n; }
   void           SetModel(const Char_t* m) { fModel = m; }
   
   const Char_t*  GetCompType() const { return kCompNm; }
  
   ClassDef(TSnCompFpga, 1); // an fpga hardware component
};

#endif // SNS_TSnCompFpga
