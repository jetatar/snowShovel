#ifndef SNS_TSnCommActSendQueued
#define SNS_TSnCommActSendQueued

#include "TSnCommStatusAction.h"

class TSnCommActSendQueued : public TSnCommStatusAction {
 private:
   virtual TSnCommObject* operator()(const TSnCommIncStatus& is);
   
 public:
   TSnCommActSendQueued() {}
   TSnCommActSendQueued(TSnCommActor* a) :
      TSnCommStatusAction(a) {
   }
   virtual ~TSnCommActSendQueued() {}
   
   virtual TString        GetLabel() const { return "Send From Queue To Stn"; }
   
   ClassDef(TSnCommActSendQueued, 1); // send the next queued comm object

};

#endif // SNS_TSnCommActSendQueued
