#ifndef SNS_TSnCompAntenna
#define SNS_TSnCompAntenna

#include "TSnComponent.h"

class TSnCompAntenna : public TSnComponent {
 private:
   Int_t             fNum; // antenna number

   static const Char_t*    kCompNm;

 public:
   TSnCompAntenna() : fNum(-1) {}
   virtual ~TSnCompAntenna() {}
   
   Bool_t         IsValidForDb() const { return fNum>=0; }
   
   const Int_t    GetNum() const { return fNum; }
   
   void           SetNum(const Int_t n) { fNum = n; }
   
   const Char_t*  GetCompType() const { return kCompNm; }
   
   ClassDef(TSnCompAntenna, 1); // an anteanna component
};

#endif // SNS_TSnCompAntenna
