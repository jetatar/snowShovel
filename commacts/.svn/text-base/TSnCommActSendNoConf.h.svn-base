#ifndef SNS_TSnCommActSendNoConf
#define SNS_TSnCommActSendNoConf

#include "TSnCommStatusAction.h"

class TSnCommActSendNoConf : public TSnCommStatusAction {
 private:
   virtual TSnCommObject* operator()(const TSnCommIncStatus& is);
   
 public:
   TSnCommActSendNoConf() {}
   TSnCommActSendNoConf(TSnCommActor* a) :
      TSnCommStatusAction(a) {
   }
   virtual ~TSnCommActSendNoConf() {}

   virtual TString        GetLabel() const { return "Send No New Conf Signal"; }

   ClassDef(TSnCommActSendNoConf, 1); // send the "no config" signal
   
};

#endif // SNS_TSnCommActSendNoConf
