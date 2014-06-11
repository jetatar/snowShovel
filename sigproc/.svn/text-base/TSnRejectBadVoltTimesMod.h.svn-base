#ifndef SNS_TSnRejectBadVoltTimesMod
#define SNS_TSnRejectBadVoltTimesMod

#include "TSnRejectBadClocksMod.h"
class TSnPowerReading;

class TSnRejectBadVoltTimesMod : public TSnRejectBadClocksMod {
 private:
   
   // branch(es)
   TSnPowerReading* fPwr; //!
   
 protected:
   virtual void  SlaveBegin();
   virtual void  Process();
   
 public:
   TSnRejectBadVoltTimesMod() : fPwr(0) {}
   TSnRejectBadVoltTimesMod(const Char_t* name) :
      TSnRejectBadClocksMod(name), fPwr(0) {}
   virtual ~TSnRejectBadVoltTimesMod() {}
   
   ClassDef(TSnRejectBadVoltTimesMod,1); // reject power readings with unreasonable times
};


#endif // SNS_TSnRejectBadVoltTimesMod
