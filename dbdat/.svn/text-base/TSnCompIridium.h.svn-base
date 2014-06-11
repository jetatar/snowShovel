#ifndef SNS_TSnCompIridium
#define SNS_TSnCompIridium

#include <TString.h>

#include "TSnComponent.h"

class TSnCompIridium : public TSnComponent {
 private:
   TString        fIMEI;   // the IMEI of this modem
   TString        fModel;  // the iridium model
   TString        fEmail;  // receiving email address
   
   static const Char_t*     kCompNm;
   
 public:
   TSnCompIridium() {}
   virtual ~TSnCompIridium() {}
   
   Bool_t         IsValidForDb() const
      { return (fIMEI.Length()>0) && (fModel.Length()>0) 
            && (fEmail.Length()>0); }
   
   const Char_t*  GetIMEI()      const { return fIMEI.Data(); }
   const Char_t*  GetModel()     const { return fModel.Data(); }
   const Char_t*  GetEmail()     const { return fEmail.Data(); }
   
   void           SetIMEI(const Char_t* imei) { fIMEI = imei; }
   void           SetModel(const Char_t* m)   { fModel = m; }
   void           SetEmail(const Char_t* e)   { fEmail = e; }
   
   const Char_t*  GetCompType()  const { return kCompNm; }
  
   ClassDef(TSnCompIridium, 1); // an iridium hardware component
};

#endif // SNS_TSnCompIridium
