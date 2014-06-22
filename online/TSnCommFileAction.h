#ifndef SNS_TSnCommFileAction
#define SNS_TSnCommFileAction

#include "TSnCommAction.h"
class TSnCommActor;
class TSnCommIncFile;

class TSnCommFileAction : public TSnCommAction {
 private:
   // this function should be overridden by specific actions it is
   // called by the TSnCommActor and contains the meat of this
   // action it should use the TSnCommActor interface to check
   // certain conditions and then decide whether to execute its
   // action, the result of which should be to generate a
   // TSnCommObject of some kind
   //
   // returning a null TSnCommObject implies this action was not
   // performed and the TSnCommActor will try the next action
   //
   // if a TSnCommObject is returned, its GetIOByteArray must
   // return a buffer that is valid for sending directly to the
   // station.  see TSnCommAlertPack and TSnCommActHiRate for
   // examples on how compound results can be returned that still
   // satisfy this requirement
   virtual TSnCommObject* operator()(const TSnCommIncFile& in)=0;
   
 public:
   TSnCommFileAction() {}
   TSnCommFileAction(TSnCommActor* a) : TSnCommAction(a) {}
   virtual ~TSnCommFileAction() {}

   virtual TSnCommObject* operator()(const TSnCommIncoming& in);

   ClassDef(TSnCommFileAction, 1); // base of a communication response to a status update
};

#endif // SNS_TSnCommFileAction