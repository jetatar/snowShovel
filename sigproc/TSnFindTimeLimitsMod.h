#ifndef SNS_TSnFindTimeLimitsMod
#define SNS_TSnFindTimeLimitsMod

#include <TString.h>
#include <TParameter.h>
class TTimeStamp;

#include "TAModule.h"

class TSnEventHeader;

class TSnFindTimeLimitsMod : public TAModule {
 private:
   // parameter
   TString               fAETimeNm; // name of the absolute time parameter object (default "AbsEvtTime")
   TString               fMinName;  // name of the min time object in the output list (default "MinEvtTime")
   TString               fMaxName;  // name of the max time object in the output list (default "MaxEvtTime")
   
   // result
   TParameter<Double_t>* fMinTime;  //! min time: unixtime.nanosecs ; use TParameter to facilitate AddOutput
   TParameter<Double_t>* fMaxTime;  //! max time: unixtime.nanosecs ; use TParameter to facilitate AddOutput

   // during running
   TSnEventHeader*       fHdr;      //! the event header
   Bool_t                fFirst;    //! if this is the first event

 protected:
   virtual void  SlaveBegin();
   virtual void  Process();
   virtual void  Terminate();

 public:
   TSnFindTimeLimitsMod() : fMinTime(0), fMaxTime(0), fHdr(0), fFirst(kTRUE) {}
   TSnFindTimeLimitsMod(const Char_t* name);
   virtual ~TSnFindTimeLimitsMod();

   const Char_t* GetAbsEvtTimeName() const { return fAETimeNm; }
   const Char_t* GetMinTimeName() const    { return fMinName; }
   const Char_t* GetMaxTimeName() const    { return fMaxName; }
   
   void          SetAbsEvtTimeName(const Char_t* n) { fAETimeNm = n; }
   void          SetMinTimeName(const Char_t* n)    { fMinName = n; }
   void          SetMaxTimeName(const Char_t* n)    { fMaxName = n; }
   
   ClassDef(TSnFindTimeLimitsMod, 1); // module to find the min/max times of all events
};

#endif // SNS_TSnFindTimeLimitsMod
