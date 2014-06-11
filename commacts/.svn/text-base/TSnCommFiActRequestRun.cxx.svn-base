#include "TSnCommFiActRequestRun.h"

#include "TSnCommProtocolMsg.h"
#include "TSnIOHeaderFrame.h"
#include "TSnCommAlertPack.h"

ClassImp(TSnCommFiActRequestRun);

TSnCommObject* TSnCommFiActRequestRun::operator()(const TSnCommIncFile& in) {

   TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
      TSnIOHeaderFrame::kHnShOkReqRnCode);
   TSnCommAlertPack* o = 
      TSnCommAlertPack::NewAlertPackPassComm(p,
                                             "TSnCommFiActRequestRun",
                                             Form("Requesting run [%u].",
                                                  fRun),
                                             "",
                                             TSnCommAlertPack::kLogInfo);
   return o;
}
