#include "TSnCommFiActNoReplyHndshk.h"

#include <TClass.h>

#include "TSnCommProtocolMsg.h"
#include "TSnIOHeaderFrame.h"
#include "TSnCommIncFile.h"
#include "TSnCommIncHandshake.h"

ClassImp(TSnCommFiActNoReplyHndshk);


TSnCommObject* TSnCommFiActNoReplyHndshk::operator()(const TSnCommIncFile& in) {
   // return a TSnIOHeaderFrame::kHnShNoReplyCode iff
   // (a) the incoming file object is a TSnCommIncHandshake
   // (b) the handshake code is TSnIOHeaderFrame::kHnShDemandCode

   TSnCommObject* o(0);
   if (in.IsA()->InheritsFrom(TSnCommIncHandshake::Class())) {
      const TSnCommIncHandshake& inh = static_cast<const TSnCommIncHandshake&>(in);
      if (inh.GetHnShCode()==TSnIOHeaderFrame::kHnShDemandCode) {
         o = new TSnCommProtocolMsg(TSnIOHeaderFrame::kHnShNoReplyCode);
      }
   }
   return o;
}
