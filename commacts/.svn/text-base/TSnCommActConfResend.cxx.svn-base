#include "TSnCommActConfResend.h"

#include "TSnCommActor.h"
#include "TSnCommConfPack.h"
#include "TSnDAQConfig.h"
#include "TSnStatusUpdate.h"
#include "TSnCommAlertPack.h"
#include "TSnCommActHiRate.h"
#include "TSnCommIncStatus.h"

#include <stdexcept>

ClassImp(TSnCommActConfResend);


TSnCommObject* TSnCommActConfResend::operator()(const TSnCommIncStatus& is) {
   TSnCommObject* o(0);
   
   const TSnCommActor& act = GetActorSafe();
   const TSnCommConfPack* cc = act.GetLastConf();
   if (cc!=0) {
      const TSnDAQConfig* conf = cc->GetDAQConfig(); // the one we think we sent
      if (conf!=0) {
         const TSnStatusUpdate* status = is.GetStatus();
         if (status!=0) {
            if ((conf->GetLabelString().CompareTo(status->GetConfLabel())!=0)
                && (conf->GetLabelString().CompareTo(
                       TSnCommActHiRate::kTrgOffConfLbl)!=0)) {
               // different configs, and not the emergency one
               // resend the old one
               if ( (fMaxResends==0) || (fConsecResends<=fMaxResends) ) {
                  ++fConsecResends;
                  if (act.IsUsingDB()) {
                     throw std::runtime_error(
                        "<TSnCommActConfResend::operator()> : "
                        "DB interaction not yet implemented.");
                  }
                  if ( fMaxResends==fConsecResends ) {
                     TString subj(Form("Have re-sent config %hu/%hu times to %s",
                                       GetConsecResends(),
                                       GetMaxResends(),
                                       status->GetMacAdrAsStr()));
                     TString msg(Form("I have resent the config with label "
                                      "[%s] %hu/%hu times to station %s. "
                                      "Further resends will not occur.",
                                      conf->GetLabel(),
                                      GetConsecResends(),
                                      GetMaxResends(),
                                      status->GetMacAdrAsStr()));
                     o = TSnCommAlertPack::NewAlertPackCopyComm(*cc,
                                              "TSnCommActConfResend",
                                              subj.Data(), msg.Data());
                  } else {
                     o = cc->NewClone();
                  }
               } // else: stop resending; move on to the next action
            } else {
               // the station got it
               fConsecResends = 0;
            }
         } // else: don't resend if there's no status update
      }
   }
   return o;
}


