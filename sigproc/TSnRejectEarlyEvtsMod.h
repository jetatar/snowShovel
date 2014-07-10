#ifndef SNS_TSnRejectEarlyEvtsMod
#define SNS_TSnRejectEarlyEvtsMod

#include "TAModule.h"

class TSnClockSetInfo;
class TSnEventHeader;
class TSnEventMetadata;
class TSnRunInfo;

class TSnRejectEarlyEvtsMod : public TAModule {
 private:
   Double_t    fEarlyTime;     // reject events whose evt time - trg start time is less than this (in seconds) (default: 2.0)
   Bool_t      fFirstEvtOnly;  // if true, reject only the first event in the time window. if false, reject all events in the time window. (default: true)

   // from the tree or loader
   TSnClockSetInfo*  fStart;    //! seq start time (requires a TSnConfigTreeLoader)
   TSnEventHeader*   fHdr;      //! event header
   TSnEventMetadata* fConfMeta; //! config metadata
   TSnRunInfo*       fRunInfo;  //! config run info
   
   Bool_t IsFirstEvtOfSeq() const;
   
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   
 public:
   TSnRejectEarlyEvtsMod() : fEarlyTime(0), fFirstEvtOnly(kTRUE),
                             fStart(0), fHdr(0), fConfMeta(0), fRunInfo(0) {}
   TSnRejectEarlyEvtsMod(const Char_t* name,
                         const Double_t earlyTime=2.0,
                         const Bool_t firstEvtOnly=kTRUE) :
      TAModule(name, "reject events too close to trigger start"),
      fEarlyTime(earlyTime), fFirstEvtOnly(firstEvtOnly),
      fStart(0), fHdr(0), fConfMeta(0), fRunInfo(0) {
   }
   virtual ~TSnRejectEarlyEvtsMod() {}
   
   Double_t    GetEarlyTime() const { return fEarlyTime; }
   Bool_t      IsRejectingOnlyFirstEvent() const { return fFirstEvtOnly; }
   
   void        SetEarlyTime(const Double_t t) { fEarlyTime = t; }
   void        SetRejectOnlyFirstEvent(const Bool_t x) { fFirstEvtOnly = x; }
   
   
   ClassDef(TSnRejectEarlyEvtsMod, 2); // reject event(s) too close to the trigger start
};

#endif // SNS_TSnRejectEarlyEvtsMod
