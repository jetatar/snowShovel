#ifndef SNS_TSnCompAmp
#define SNS_TSnCompAmp

#include "TString.h"
class TTimeStamp;

#include "TSnComponent.h"

class TSnCompAmp : public TSnComponent {
 private:
   Int_t       fNum;       // amp number
   TTimeStamp* fDateCnstr; // date constructed (may be empty)
   TString     fModel;     // amp model

   static const Char_t*     kCompNm;

 public:
   TSnCompAmp() : fNum(-1), fDateCnstr(0) {}
   virtual ~TSnCompAmp();
  
   Bool_t            IsValidForDb() const
      { return (fNum>=0) && (fModel.Length()>0); }

   const Int_t       GetNum()             const { return fNum; }
   const TTimeStamp* GetDateConstructed() const { return fDateCnstr; }
   const Char_t*     GetModel()           const { return fModel.Data(); }

   Bool_t            HasDateConstructed() const { return fDateCnstr!=0; }

   void              SetNum(const Int_t n) { fNum = n; }
   void              SetDateConstructed(const TTimeStamp& d);
   void              SetDateConstructed(const UInt_t tloc,
                                        const Bool_t isUTC,
                                        const Int_t secOffset,
                                        const Bool_t dosDate);
   void              SetModel(const Char_t* m) { fModel = m; }

   const Char_t*     GetCompType() const { return kCompNm; }

   ClassDef(TSnCompAmp, 1); // an amp hardware component
};


#endif // SNS_TSnCompAmp
