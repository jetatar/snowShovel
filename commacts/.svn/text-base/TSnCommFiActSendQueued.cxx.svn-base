#include "TSnCommFiActSendQueued.h"

#include "TSnCommActor.h"

ClassImp(TSnCommFiActSendQueued);

TSnCommObject* TSnCommFiActSendQueued::operator()(const TSnCommIncFile& in) {
   // return the next comm object from the actor's queue, if there is one
   // (return 0 if not)
   return GetActor().PullNextComm(); // may be 0
}

