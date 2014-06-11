#ifndef SNS_TSnCommFiActNoReplyHndshk
#define SNS_TSnCommFiActNoReplyHndshk

#include "TSnCommFileAction.h"
class TSnCommActor;

class TSnCommFiActNoReplyHndshk : public TSnCommFileAction {
 private:
   
   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActNoReplyHndshk() {}
   TSnCommFiActNoReplyHndshk(TSnCommActor* a) :
      TSnCommFileAction(a) {}
   virtual ~TSnCommFiActNoReplyHndshk() {}

   virtual TString        GetLabel() const { return "No Reply To Handshake Demand"; }

   ClassDef(TSnCommFiActNoReplyHndshk, 1); // send NoReply code as response to handshake demand
};

#endif // SNS_TSnCommFiActNoReplyHndshk
