#ifndef SNS_TSnCommActSendPeriodic
#define SNS_TSnCommActSendPeriodic

class TTimeStamp;

#include "TSnCommStatusAction.h"

class TSnCommActSendPeriodic : public TSnCommStatusAction {
 private:
   UInt_t           fSend;    // send the periodic comm after this unix date/time
   UInt_t           fPeriod;  // send again after increments of this many seconds (0 means send only once)
   UInt_t           fDur;     // this many seconds after fSend, send the comm object that got replaced, if and only if the actor's last sent comm is still the one sent by this periodic action (if fDur==0, the periodic comm will not automatically get replaced)
   UInt_t           fRevert;  // internal clock to revert the comm if the unix date/time is after this but before fSend (i.e. time at which to switch back to normal)
   TSnCommObject*   fCommObj; // the comm object to be sent (owned copy)
   TSnCommObject*   fReplCom; // the comm object we replaced (owned copy)
   
   virtual TSnCommObject* operator()(const TSnCommIncStatus& is);

   void CopyReplComm(const TSnCommObject* c); // intentionally not public
   const TSnCommObject* GetLastSentComm() const;

 public:
   TSnCommActSendPeriodic() :
      fSend(0), fPeriod(0), fDur(0), fRevert(0),
      fCommObj(0), fReplCom(0) {}
   TSnCommActSendPeriodic(TSnCommActor* a) : 
      TSnCommStatusAction(a), fSend(0), fPeriod(0), fDur(0), fRevert(0),
      fCommObj(0), fReplCom(0) {}
   TSnCommActSendPeriodic(TSnCommActor* a, const TSnCommObject& comm,
                          const TTimeStamp& first, const UInt_t per,
                          const UInt_t dur=0);
   virtual ~TSnCommActSendPeriodic();
   
   UInt_t               GetSendTime() const { return fSend; }
   UInt_t               GetPeriod() const { return fPeriod; }
   UInt_t               GetDuration() const { return fDur; }
   const TSnCommObject* GetCommObject() const { return fCommObj; }
   const TSnCommObject* GetReplacedComm() const { return fReplCom; }
   UInt_t               GetRevertTime() const { return fRevert; }

   virtual TString        GetLabel() const { return "Send To Stn Periodically"; }

   void              SetFirstTime(const TTimeStamp& first);
   void              SetFirstTime(const UInt_t first) { fSend = first; }
   void              SetPeriod(const UInt_t p) { fPeriod = p; }
   void              SetDuration(const UInt_t d) { fDur = d; }
   void              SetCommObject(const TSnCommObject& c);
   // intentionally no public setters for fReplCom or fRevert

   void              ResetReplacing() {
      // clear the replacement object and the replacement clock.
      CopyReplComm(0);
      fRevert = 0;
   }
   
   ClassDef(TSnCommActSendPeriodic, 3); // send the specified calib at particular time(s)
};

#endif // SNS_TSnCommActSendPeriodic
