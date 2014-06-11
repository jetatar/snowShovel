#ifndef SNS_TSnCommIncStatus
#define SNS_TSnCommIncStatus

#include "TSnCommIncoming.h"
class TSnPowerReading;

class TSnCommIncStatus : public TSnCommIncoming {
 private:
   const TSnPowerReading*   fPower; // incoming power reading
   Int_t                  fPwIOvers;// power frame io version

 public:
   TSnCommIncStatus() : fPower(0) {}
   TSnCommIncStatus(const TTimeStamp& t, 
                    const TSnStatusUpdate& s,
                    const TSnPowerReading& p,
                    const Int_t stvers,
                    const Int_t pwvers);
   TSnCommIncStatus(const TTimeStamp& t, 
                    const TSnStatusUpdate& s,
                    const Int_t stvers)
      : TSnCommIncoming(t, s, stvers), fPower(0) {}
   virtual ~TSnCommIncStatus();
   
   const TSnPowerReading* GetPower() const { return fPower; }
   Int_t                  GetPowerIOversion() const { return fPwIOvers; }
   
   void  SetPower(const TSnPowerReading& p);
   void  SetPowerIOversion(const Int_t p) { fPwIOvers = p; }

   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TSnCommIncStatus, 2); // an incoming status info packet
};

#endif // SNS_TSnCommIncStatus
