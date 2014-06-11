#ifndef SNS_TSnCommFiActRequestRun
#define SNS_TSnCommFiActRequestRun

#include "TSnCommFileAction.h"

class TSnCommFiActRequestRun : public TSnCommFileAction {
 private:
   UInt_t    fRun; // the run to delete
   
   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActRequestRun() : fRun(0) {}
   TSnCommFiActRequestRun(TSnCommActor* a, const UInt_t r) :
      TSnCommFileAction(a), fRun(r) {}
   virtual ~TSnCommFiActRequestRun() {}
   
   UInt_t   GetRun() const { return fRun; }
   virtual TString        GetLabel() const { return "Send Run From Stn"; }
   
   void     SetRun(const UInt_t r) { fRun = r; }
   
   ClassDef(TSnCommFiActRequestRun, 1); // send the delete run signal
};

#endif // SNS_TSnCommFiActRequestRun
