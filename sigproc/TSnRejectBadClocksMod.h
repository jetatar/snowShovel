#ifndef SNS_TSnRejectBadClocksMod
#define SNS_TSnRejectBadClocksMod

class TTimeStamp;

#include <TAModule.h>

class TSnEventHeader;

class TSnRejectBadClocksMod : public TAModule {
 private:
   Double_t      fMinTime; // minimum reasonable unix time.nanosec (default: Jan 2010)
   Double_t      fMaxTime; // maximum reasonable unix time.nanosec (default: Jan 2017)
    Bool_t          bInclude;   // If set to true events between time periods
                                // are included, if false, events are excluded

   // event objs
   TSnEventHeader* fHdr;   //! event header

 protected:
   virtual void  SlaveBegin();
   virtual void  Process();

   void          CheckTime(const Double_t t);

 public:
   TSnRejectBadClocksMod() : 
      fMinTime(1262304000.0), fMaxTime(1483228800.0), bInclude(1),
      fHdr(0) {}
   TSnRejectBadClocksMod(const Char_t* name) :
      TAModule(name, "Reject events with unreasonable times"),
      fMinTime(1262304000.0), fMaxTime(1483228800.0), bInclude(1),
      fHdr(0) {}
   virtual ~TSnRejectBadClocksMod() {}

   Double_t      GetMinTime() const { return fMinTime; }
   Double_t      GetMaxTime() const { return fMaxTime; }
   
   void          SetMinTime(const Double_t t) { fMinTime = t; }
   void          SetMaxTime(const Double_t t) { fMaxTime = t; }
    void        SetInclude( Bool_t i ){ bInclude = i; }

   ClassDef(TSnRejectBadClocksMod,1); // reject events with unreasonable times
};

#endif // SNS_TSnRejectBadClocksMod
