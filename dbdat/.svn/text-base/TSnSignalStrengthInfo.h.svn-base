#ifndef SNS_TSnSignalStrengthInfo
#define SNS_TSnSignalStrengthInfo

#include <TObject.h>
#include <TTimeStamp.h>

class TSnSignalStrengthInfo : public TObject {
 private:
   UChar_t    fCommType; // the comm type (see EDatPackBit in TSnDAQConfig)
   Float_t    fSigStr;   // the signal strength
   TTimeStamp fSSTime;   // time the signal strength was measured

 public:
   TSnSignalStrengthInfo() : fCommType(0), fSigStr(0) {}
   virtual ~TSnSignalStrengthInfo() {}

   virtual void      Print(Option_t* option="") const;
   virtual void      Copy(TObject& object) const;
   
   Int_t             PyGetCommType() const {
      // helper function for pyroot
      return static_cast<Int_t>(GetCommType());
   }
   UChar_t           GetCommType()   const { return fCommType; }
   Float_t           GetSigStr()     const { return fSigStr; }
   const TTimeStamp& GetSigStrTime() const { return fSSTime; }

   void  SetCommType(const UChar_t t) { fCommType = t; }
   void  SetSigStr(const Float_t ss) { fSigStr = ss; }
   void  SetSigStrTime(const TTimeStamp& t) { fSSTime = t; }
   
   ClassDef(TSnSignalStrengthInfo, 1); // signal strength info
};

#endif // SNS_TSnSignalStrengthInfo
