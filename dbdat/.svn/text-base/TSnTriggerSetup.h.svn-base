#ifndef SNS_TSnTriggerSetup
#define SNS_TSnTriggerSetup

#include <TObject.h>
class TMap;

class TSnTriggerSetup : public TObject {

 public:

   static const Char_t*    kMajLogTrgType;

 private:
   TMap*             fTrgLog; // map from trigger logic types to values
   UShort_t          fThrotl; // trigger throttle period (ms) (default 50ms)
   Float_t           fForced; // forced trigger period (s) (default 67s)
   Bool_t            fThmOn;  // thermal trigger enabled? (default true)
   
 public:
   TSnTriggerSetup() : fTrgLog(0), fThrotl(50), fForced(67), fThmOn(kTRUE) {}
   TSnTriggerSetup(const TSnTriggerSetup& t);
   virtual ~TSnTriggerSetup();

   static
   void Swap(TSnTriggerSetup& a, TSnTriggerSetup& b);
   
   TSnTriggerSetup&  operator=(TSnTriggerSetup o) {
      // assign using pass by value for efficient copy
      Swap(*this, o);
      return *this;
   }
   
   const TMap*       GetTrigLogic() const { return fTrgLog; }
   UShort_t          GetTrigLogic(const Char_t* type) const;
   UChar_t           GetNumCardsMajLog() const;
   UShort_t          GetThrottlePeriodms() const { return fThrotl; }
   Float_t           GetForcedPeriod() const { return fForced; }
   Bool_t            IsThermalTriggering() const { return fThmOn; }

   virtual Bool_t    IsValidForDb() const;
   virtual void      Print(Option_t* option = "") const;

   void     SetTrigLogic(const Char_t* type, const UShort_t val);
   void     SetNumCardsMajLog(const UShort_t n)
      { SetTrigLogic(kMajLogTrgType, n); }
   void     SetThrottlePeriodms(const UShort_t p) { fThrotl = p; }
   void     SetForcedPeriod(const Float_t p) { fForced = p; }
   void     SetThermalTriggering(const Bool_t t=kTRUE) { fThmOn = t; }

   ClassDef(TSnTriggerSetup, 1); // store trigger setup properties
};

#endif // SNS_TSnTriggerSetup
