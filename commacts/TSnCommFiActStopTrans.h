#ifndef SNS_TSnCommFiActStopTrans
#define SNS_TSnCommFiActStopTrans

#include "TSnCommFileAction.h"

class TSnCommFiActStopTrans : public TSnCommFileAction {
 private:
   UInt_t    fDelay; // number of seconds after comm win opened to stop transfer
   
   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActStopTrans() : fDelay(0) {}
   TSnCommFiActStopTrans(TSnCommActor* a, const UInt_t delay=0)
      : TSnCommFileAction(a), fDelay(delay) {}
   virtual ~TSnCommFiActStopTrans() {}
   
   UInt_t    GetDelay() const { return fDelay; }
   virtual TString        GetLabel() const { return "Stop Stn Data Transfer"; }
   
   void      SetDelay(const UInt_t d) { fDelay = d; }
   
   ClassDef(TSnCommFiActStopTrans, 1); // stop the file transfer
};

#endif // SNS_TSnCommFiActStopTrans
