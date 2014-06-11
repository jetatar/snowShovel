#include "TSnCommFiActStopTrans.h"

#include <TString.h>
#include <TTimeStamp.h>

#include "TSnCommIncFile.h"
#include "TSnCommProtocolMsg.h"
#include "TSnIOHeaderFrame.h"
#include "TSnCommAlertPack.h"

ClassImp(TSnCommFiActStopTrans);


TSnCommObject* TSnCommFiActStopTrans::operator()(const TSnCommIncFile& in) {
   
   TSnCommAlertPack* o(0);
   
   const TTimeStamp* time = in.GetTime();
   UInt_t d(fDelay); // stop if we don't know when the comm win opened
   if (time!=0) {
      TTimeStamp now;
      d = now.GetSec() - time->GetSec();
   }
   if (d>=fDelay) {
      TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
               TSnIOHeaderFrame::kHnShOkStopCode);
      o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                                 "TSnCommFiActStopTrans",
                                                 "Stopping file transfer",
                                                 "",
                                                 TSnCommAlertPack::kLogInfo);
   }
   
   return o;
}
