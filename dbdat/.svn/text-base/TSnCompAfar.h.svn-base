#ifndef SNS_TSnCompAfar
#define SNS_TSnCompAfar

#include "TString.h"

#include "TSnComponent.h"

class TSnCompAfar : public TSnComponent {
 private:
   TString     fMAC;    // mac address. waste of memory to store as string, but
   TString     fIP;     // ip address (may be empty)
   TString     fModel;  // the Afar model (may be empty)
   TString     fSerial; // the serial number (may be empty)
   
   
   static const Char_t*    kCompNm;
   
 public:
   TSnCompAfar() {}
   virtual ~TSnCompAfar() {}
   
   Bool_t        IsValidForDb() const { return fMAC.Length()>0; }
   
   const Char_t* GetMAC()    const { return fMAC.Data(); }
   const Char_t* GetIP()     const { return fIP.Data(); }
   const Char_t* GetModel()  const { return fModel.Data(); }
   const Char_t* GetSerial() const { return fSerial.Data(); }
   
   void          SetMAC(const Char_t* mac)    { fMAC = mac; }
   void          SetIP(const Char_t* ip)      { fIP = ip; }
   void          SetModel(const Char_t* mod)  { fModel = mod; }
   void          SetSerial(const Char_t* ser) { fSerial = ser; }
   
   Bool_t        HasIP()     const { return fIP.Length()>0; }
   Bool_t        HasModel()  const { return fModel.Length()>0; }
   Bool_t        HasSerial() const { return fSerial.Length()>0; }
   
   const Char_t* GetCompType() const { return kCompNm; }
   
   ClassDef(TSnCompAfar, 1); // an Afar component
};

#endif // SNS_TSnCompAfar
