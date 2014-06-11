#ifndef SNS_TSnCommFiActSendQueued
#define SNS_TSnCommFiActSendQueued

#include "TSnCommFileAction.h"

class TSnCommFiActSendQueued : public TSnCommFileAction {
 private:
   virtual TSnCommObject* operator()(const TSnCommIncFile& in);
   
 public:
   TSnCommFiActSendQueued() {}
   TSnCommFiActSendQueued(TSnCommActor* a) :
      TSnCommFileAction(a) {
   }
   virtual ~TSnCommFiActSendQueued() {}
   
   virtual TString GetLabel() const { return "Send From File Queue To Stn"; }
   
   ClassDef(TSnCommFiActSendQueued, 1); // send the next (file) queued comm object

};

#endif // SNS_TSnCommFiActSendQueued
