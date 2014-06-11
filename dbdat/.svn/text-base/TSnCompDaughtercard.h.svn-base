#ifndef SNS_TSnCompDaughtercard
#define SNS_TSnCompDaughtercard

#include <TString.h>
#include <TRef.h>
class TTimeStamp;

#include "TSnComponent.h"
class TSnCompFpga;
class TSnCompDigitizer;

class TSnCompDaughtercard : public TSnComponent {
 private:
   Int_t          fNum;       // the daughtercard number
   TRef           fFpga;      // the fpga component
   TRef           fDigi;      // the digitizer component
   TTimeStamp*    fDateCnstr; // the date constructed (may be empty)
   TString        fModel;     // the daughtercard model
   
   
   static const Char_t*     kCompNm;
  
   
 public:
   TSnCompDaughtercard() : fNum(0), fDateCnstr(0) {}
   virtual ~TSnCompDaughtercard();
   
   Bool_t      IsValidForDb() const;
   
   Int_t                   GetNum()             const { return fNum; }
   const TSnCompFpga&      GetFpga()            const;
         TSnCompFpga&      GetFpga();
   const TSnCompDigitizer& GetDigitizer()       const;
         TSnCompDigitizer& GetDigitizer();
   const TTimeStamp*       GetDateConstructed() const { return fDateCnstr; }
   const Char_t*           GetModel()           const { return fModel.Data(); }
   
   Bool_t                  HasDateConstructed() const { return fDateCnstr!=0; }
   
   void        SetNum(const Int_t n)                   { fNum = n; }
   void        SetFpga(TSnCompFpga& fpga);
   void        SetDigitizer(TSnCompDigitizer& d);
   void        SetDateConstructed(const TTimeStamp& d);
   void        SetModel(const Char_t* m)               { fModel = m; }
   
   const Char_t*           GetCompType()  const { return kCompNm; }
  
   ClassDef(TSnCompDaughtercard, 1); // a daughtercard hardware component
};

#endif // SNS_TSnCompDaughtercard
