#include "TSnCommActSendPeriodic.h"

#include <TTimeStamp.h>
#include <TClass.h>

#include "TSnCommObject.h"
#include "TSnCommActor.h"
#include "TSnCommIncStatus.h"

ClassImp(TSnCommActSendPeriodic);

TSnCommActSendPeriodic::TSnCommActSendPeriodic(TSnCommActor* a,
                                               const TSnCommObject& comm,
                                               const TTimeStamp& first,
                                               const UInt_t per,
                                               const UInt_t dur) :
   TSnCommStatusAction(a), fSend(first.GetSec()), fPeriod(per), 
   fDur(dur), fRevert(0), fCommObj(comm.NewClone()), fReplCom(0) {
}

TSnCommActSendPeriodic::~TSnCommActSendPeriodic() {
   delete fCommObj;
   delete fReplCom;
}

void TSnCommActSendPeriodic::SetFirstTime(const TTimeStamp& first) {
   fSend = first.GetSec();
}

void TSnCommActSendPeriodic::SetCommObject(const TSnCommObject& c) {
   delete fCommObj;
   fCommObj = c.NewClone();
}

void TSnCommActSendPeriodic::CopyReplComm(const TSnCommObject* c) {
   delete fReplCom;
   fReplCom = (c==0) ? 0 : c->NewClone();
}

const TSnCommObject* TSnCommActSendPeriodic::GetLastSentComm() const {
   if (fCommObj!=0) {
      const TSnCommActor& act = GetActorSafe();
      return act.GetLastSent(fCommObj->IsA()->GetName());
   }
   return 0;
}

TSnCommObject* TSnCommActSendPeriodic::operator()(const TSnCommIncStatus& is) {
   TSnCommObject* o(0);
   const TTimeStamp* time = is.GetTime();
   if ((time!=0) && (fSend>0)) {
      const UInt_t st = static_cast<UInt_t>(time->GetSec());
      if (st > fSend) {
         // set the clock for the next revert and periodic sending
         // setting revert is really only needed the first time
         //
         // use the time the status was received instead of just the
         // "old" fSend because fSend may be some time far in the past
         // (who knows what the user set), which could result in always
         // sending the periodic comm
         fRevert = (fDur==0)    ? 0 : (st+fDur); // before we change send
         fSend   = (fPeriod==0) ? 0 : (st+fPeriod);
         // send the periodic comm object
         o = fCommObj;
         // store the one we're replacing
         const TSnCommObject* last = GetLastSentComm();
         CopyReplComm(last);
      } else if ((fRevert>0) && (st > fRevert)) {
         // next reversion
         fRevert = (fPeriod==0) ? 0 : (st+fPeriod);
         if (fCommObj!=0) {
            const TSnCommObject* last = GetLastSentComm();
            if ( (last!=0) && (last->IsEqual(fCommObj)) ) {
               // last thing sent was the periodic object
               // send the replacement comm object
               o = fReplCom;
            }
         }
      }
   }
   return o;
}

