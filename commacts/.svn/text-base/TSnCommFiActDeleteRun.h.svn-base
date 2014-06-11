#ifndef SNS_TSnCommFiActDeleteRun
#define SNS_TSnCommFiActDeleteRun

#include "TSnCommFileAction.h"

class TSnCommFiActDeleteRun : public TSnCommFileAction {
 private:
   UInt_t    fRun; // the run to delete
   
   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActDeleteRun() : fRun(0) {}
   TSnCommFiActDeleteRun(TSnCommActor* a, const UInt_t r) :
      TSnCommFileAction(a), fRun(r) {}
   virtual ~TSnCommFiActDeleteRun() {}
   
   UInt_t   GetRun() const { return fRun; }
   virtual TString        GetLabel() const { return "Delete Run From Stn"; }
   
   void     SetRun(const UInt_t r) { fRun = r; }
   
   ClassDef(TSnCommFiActDeleteRun, 1); // send the delete run signal
};

#endif // SNS_TSnCommFiActDeleteRun
