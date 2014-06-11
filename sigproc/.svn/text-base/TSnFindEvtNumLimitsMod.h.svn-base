#ifndef SNS_TSnFindEvtNumLimitsMod
#define SNS_TSnFindEvtNumLimitsMod

#include <TString.h>
#include <TParameter.h>

#include "TAModule.h"

class TSnEventHeader;

class TSnFindEvtNumLimitsMod : public TAModule {
 private:
   TString   fMinName; // name of the min evt number object in the output (default "MinEvtNum")
   TString   fMaxName; // name of the max evt number object in the output (default "MaxEvtNum")
   
   // results
   TParameter<UInt_t>* fMinEvtNum; //! the min event number; use TParameter so it inherits from TObjct (for AddOutput)
   TParameter<UInt_t>* fMaxEvtNum; //! the max event number; use TParameter so it inherits from TObjct (for AddOutput)
   
   // during running
   TSnEventHeader*     fHdr;       //! the event header
   Bool_t              fFirst ;    //! if this is the first event

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   virtual void Terminate();
   
 public:
   TSnFindEvtNumLimitsMod() : 
      fMinEvtNum(0), fMaxEvtNum(0), fHdr(0), fFirst(kTRUE) {}
   TSnFindEvtNumLimitsMod(const Char_t* name);
   virtual ~TSnFindEvtNumLimitsMod();
   
   const Char_t* GetMinEvtNumName() const { return fMinName.Data(); }
   const Char_t* GetMaxEvtNumName() const { return fMaxName.Data(); }
   
   void          SetMinEvtNumName(const Char_t* n) { fMinName = n; }
   void          SetMaxEvtNumName(const Char_t* n) { fMaxName = n; }
   
   ClassDef(TSnFindEvtNumLimitsMod, 1); // find the min/max event number
};

#endif // SNS_TSnFindEvtNumLimitsMod
