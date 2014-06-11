#ifndef SNS_TSnCalcAbsTimeMod
#define SNS_TSnCalcAbsTimeMod

#include <TString.h>
#include <TTimeStamp.h>

#include "TAModule.h"

class TSnClockSetInfo;
class TSnEventHeader;

class TSnCalcAbsTimeMod : public TAModule {
 private:
   // parameters
   TString          fAETimeNm; // name of the absolute time parameter object (default "AbsEvtTime")
   
   // used during running
   TTimeStamp       fPrev;     //! last event or seq start time
   Bool_t           fNewFile;  //! when true, use fStart as the previous time
   
   // from the tree or loader
   TSnClockSetInfo* fStart;    //! seq start time (requires a TSnConfigTreeLoader)
   TSnEventHeader*  fHdr;      //! event header

   TSnClockSetInfo* fPrevStart;//! seq start time at last Process call
 
 protected:
   virtual void     SlaveBegin();
   virtual void     Process();

 public:
   TSnCalcAbsTimeMod() : fNewFile(kTRUE), fStart(0), fHdr(0), fPrevStart(0) {}
   TSnCalcAbsTimeMod(const Char_t* name);
   virtual ~TSnCalcAbsTimeMod();
   
   const Char_t* GetAbsEvtTimeName() const { return fAETimeNm; }
   
   void          SetAbsEvtTimeName(const Char_t* n) { fAETimeNm = n; }

   ClassDef(TSnCalcAbsTimeMod, 2); // calculate the absolute time of an event. requires a TAMObjLoader for the trigger clock start
};

#endif // SNS_TSnCalcAbsTimeMod
