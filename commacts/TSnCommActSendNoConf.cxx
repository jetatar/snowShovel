#include "TSnCommActSendNoConf.h"

#include "TSnCommProtocolMsg.h"
#include "TSnIOHeaderFrame.h"

ClassImp(TSnCommActSendNoConf);

TSnCommObject* TSnCommActSendNoConf::operator()(const TSnCommIncStatus& is) {
   // send the no config protocol message
   return new TSnCommProtocolMsg(TSnIOHeaderFrame::kNoConfigCode, 0);
}

