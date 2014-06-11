#include "TSnCommActSendQueued.h"

#include "TSnCommActor.h"

ClassImp(TSnCommActSendQueued);

TSnCommObject* TSnCommActSendQueued::operator()(const TSnCommIncStatus& is) {
   // return the next comm object from the actor's queue, if there is one
   // (return 0 if not)
   return GetActor().PullNextComm(); // may be 0
}

