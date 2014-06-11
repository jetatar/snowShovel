#ifndef SNS_TSnCompMbed
#define SNS_TSnCompMbed

#include <TString.h>

#include "TSnComponent.h"

class TSnCompMbed : public TSnComponent {
 private:
   TString     fMac;    // the mac address (a waste of memory as a string, but)
   TString     fModel;  // the mbed model
   
   static const Char_t*     kCompNm;
   
 public:
   TSnCompMbed() {}
   TSnCompMbed(const Char_t* mac,
               const Char_t* model="LPC1768") : 
      fMac(mac), fModel(model) {}
   virtual ~TSnCompMbed() {}
   
   Bool_t         IsValidForDb() const
      { return (fMac.Length()>0) && (fModel.Length()>0); }
   virtual Bool_t IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const { return fMac.Hash(); }

   
   const Char_t*  GetMAC()       const { return fMac.Data(); }
   ULong64_t      GetMACVal()    const { return GetMACValFromStr(fMac); }
   const Char_t*  GetModel()     const { return fModel.Data(); }
   
   void           SetMAC(const Char_t* m)    { fMac = m; }
   void           SetMAC(const ULong64_t m)  { fMac = GetMACFromVal(m); }
   void           SetModel(const Char_t* m)  { fModel = m; }
   
   const Char_t*  GetCompType()  const { return kCompNm; }
   
   static
   const Char_t*  GetMACFromVal(const ULong64_t m);
   
   static
   ULong64_t      GetMACValFromStr(const TString macstr);

   
   ClassDef(TSnCompMbed, 1); // an mbed hardware component
};

#endif // SNS_TSnCompMbed
