#ifndef SNS_TSnRejectEarlyEvtsMod
#define SNS_TSnRejectEarlyEvtsMod

#include "TAModule.h"

class TSnClockSetInfo;
class TSnEventHeader;

class TSnRejectEarlyEvtsMod : public TAModule {
 private:
   Double_t    fEarlyTime;  // reject events whose evt time - trg start time is less than this (in seconds) (default: 2.0)

   // from the tree or loader
   TSnClockSetInfo* fStart;    //! seq start time (requires a TSnConfigTreeLoader)
   TSnEventHeader*  fHdr;      //! event header
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnRejectEarlyEvtsMod() : fEarlyTime(0), fStart(0), fHdr(0) {}
   TSnRejectEarlyEvtsMod(const Char_t* name,
                         const Double_t earlyTime=2.0) :
      TAModule(name, "reject events too close to trigger start"),
      fEarlyTime(earlyTime), fStart(0), fHdr(0) {
   }
   virtual ~TSnRejectEarlyEvtsMod() {}
   
   Double_t    GetEarlyTime() const { return fEarlyTime; }
   
   void        SetEarlyTime(const Double_t t) { fEarlyTime = t; }
   
   
   ClassDef(TSnRejectEarlyEvtsMod, 1); // reject event(s) too close to the trigger start
};

#endif // SNS_TSnRejectEarlyEvtsMod
