#include "TSnCommFiActDeleteAllData.h"

ClassImp(TSnCommFiActDeleteAllData);

#include "TSnCommDeleteDataMsg.h"
#include "TSnIOHeaderFrame.h"
#include "TSnCommAlertPack.h"

TSnCommObject* TSnCommFiActDeleteAllData::operator()(const TSnCommIncFile& in) {
   
   TSnCommDeleteDataMsg* p = new TSnCommDeleteDataMsg(
      TSnIOHeaderFrame::kHnShOkDelAlCode);
   TSnCommAlertPack* o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                              "TSnCommFiActDeleteAllData",
                                              "Sending DELETE ALL DATA signal!",
                                              "",
                                              TSnCommAlertPack::kLogWarn);
   return o;
}
