#include "TSnCommFiActDeleteRun.h"

#include "TSnCommDeleteDataMsg.h"
#include "TSnIOHeaderFrame.h"
#include "TSnCommAlertPack.h"

ClassImp(TSnCommFiActDeleteRun);

TSnCommObject* TSnCommFiActDeleteRun::operator()(const TSnCommIncFile& in) {
   
   TSnCommDeleteDataMsg* p = new TSnCommDeleteDataMsg(
      TSnIOHeaderFrame::kHnShOkDelRnCode, fRun);
   TSnCommAlertPack* o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                              "TSnCommFiActStopTrans",
                                              "Sending DELETE RUN %u signal.",
                                              "",
                                              TSnCommAlertPack::kLogInfo);
   return o;
}
