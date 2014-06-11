#ifndef SNS_TSnCompMotherboard
#define SNS_TSnCompMotherboard

#include <TRef.h>
#include <TString.h>
class TTimeStamp;

#include "TSnComponent.h"
class TSnCompFpga;

class TSnCompMotherboard : public TSnComponent {
 private:
   Int_t       fNum;       // the motherboard number
   TRef        fFpga;      // the fpga
   TTimeStamp* fDateCnstr; // date constructed (may be empty)
   TString     fModel;     // motherboard model
   
   static const Char_t*     kCompNm;

 public:
   TSnCompMotherboard() : fNum(-1), fDateCnstr(0) {}
   virtual ~TSnCompMotherboard();
   
   Bool_t         IsValidForDb() const;
   
   Int_t                GetNum()             const { return fNum; }
   const TSnCompFpga&   GetFpga()            const;
         TSnCompFpga&   GetFpga();
   const TTimeStamp*    GetDateConstructed() const { return fDateCnstr; }
   const Char_t*        GetModel()           const { return fModel.Data(); }
   
   Bool_t               HasDateConstructed() const { return fDateCnstr!=0; }
   
   void        SetNum(const Int_t n)               { fNum = n; }
   void        SetFpga(TSnCompFpga& fpga);
   void        SetDateConstructed(const TTimeStamp& d);
   void        SetModel(const Char_t* m)           { fModel = m; }
   
   const Char_t*  GetCompType()  const { return kCompNm; }
   
   ClassDef(TSnCompMotherboard, 1); // a motherboard hardware component
};

#endif // SNS_TSnCompMotherboard
