#include "TSnCommActHiRate.h"

#include <TString.h>
#include <TTimeStamp.h>

#include "NSnConstants.h"
#include "TSnDAQConfig.h"
#include "TSnRunInfo.h"
#include "TSnNetAdrSet.h"
#include "TSnDacSet.h"
#include "TSnCommWinSet.h"
#include "TSnStatusUpdate.h"
#include "TSnCommActor.h"
#include "TSnCommConfPack.h"
#include "TSnCommAlertPack.h"
#include "TSnCommIncStatus.h"
#include "TSnIOStatusFrame.h"
#include "TSnIOConfigFrame.h"

ClassImp(TSnCommActHiRate);

const Char_t*  TSnCommActHiRate::kTrgOffConfLbl = "Thermal Trigs Off";

TSnCommObject* TSnCommActHiRate::operator()(const TSnCommIncStatus& is) {
   TSnCommObject* o(0);
   const TSnStatusUpdate* status = is.GetStatus();
   if ( status!=0 ) {
      if ( (status->GetThmRate() > fThmRateCut) ||
           (status->GetEvtRate() > fEvtRateCut) ) {
         TSnCommObject* c = NewThermsOffConfPack(status->GetMacAdrAsStr(),
                                                 is.GetStatusIOversion());
         const TSnCommActor& act = GetActorSafe();
         if (act.IsUsingDB()) {
            throw std::runtime_error("<TSnCommActHiRate::operator()> : "
                                     "DB interaction not yet implemented.");
         }
         TString subj(Form("HIGH RATES on stn %s",
                         status->GetMacAdrAsStr()));
         TString msg(Form("Rates on station %s were reportedly %g Hz "
                          "thermal triggering and %g Hz event saving as "
                          "of %s, during run %u, seq %hu. SD "
                          "card free space at %g MB.\r\n"
                          "\r\nThermal triggers have been disabled.",
                          status->GetMacAdrAsStr(),
                          status->GetThmRate(),
                          status->GetEvtRate(),
                          status->GetStatusTime().AsString("s"),
                          status->GetRunNum(),
                          status->GetSeqNum(),
                          status->GetFreeMB()));

         o = TSnCommAlertPack::NewAlertPackPassComm(c,
                                  "TSnCommActHiRate",
                                  subj.Data(), msg.Data());
      }
   }
   return o;
}

TSnCommObject* TSnCommActHiRate::NewThermsOffConfPack(const TString& stnlbl,
                                                      const Int_t svers) {
   // make a new thermals-off configuration TSnCommConfPack
   
   TString clbl(kTrgOffConfLbl);
   
   TSnNetAdrSet nets;
   TSnRunInfo runf;
   TSnDAQConfig conf;

   SetRunInfo(runf, clbl, stnlbl, svers);
   SetDAQConfig(conf, clbl, svers);
   
   Int_t cvers = -1;
   if (svers<=TSnIOStatusFrame::k2012to2013IOVers) {
      cvers = TSnIOConfigFrame::k2012to2013IOVers;
   }
   
   return (new TSnCommConfPack(conf, runf, nets, cvers));
}

void TSnCommActHiRate::SetDAQConfig(TSnDAQConfig& conf,
                                    const TString& clbl,
                                    const Int_t svers) {
   conf.SetLabel(clbl.Data());
   conf.SetUsage("auto");
   conf.SetUser("auto");
   conf.SetDescription("Emergency thermal triggers off. Automatic response "
                       "to high rates by TSnCommActHiRate.");
   for (TSnDacSet::DacSet_t::size_type ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (TSnDacSet::ChDacs_t::size_type dn=0; dn<NSnConstants::kNdacs; ++dn) {
         conf.SetDac(ch, dn, 4000);
      }
   }
   conf.SetPla(0, "HAAAAAAA");
   conf.SetNumCardsMajLog(2);
   conf.SetThermalTriggering(kFALSE); // the main goal
   conf.SetCommWinDuration(600);
   conf.SetCommWinPeriod(300);
   conf.SetRunMode(0);
   conf.SetHeartbeatPeriod(0);
   conf.SetPowerMode(TSnDAQConfig::kAmpsDatTak |
                     TSnDAQConfig::kCardDatTak |
                     TSnDAQConfig::kAmpsComWin |
                     TSnDAQConfig::kCardComWin |
                     TSnDAQConfig::kIridComWin |
                     TSnDAQConfig::kAfarComWin);
   conf.SetVoltCheckPeriod(100);
   conf.SetCommWinSendData(TSnCommWinSet::kUseBits |
                           TSnCommWinSet::kTimeout);

   // other parameters default

   if (svers<=TSnIOStatusFrame::k2012to2013IOVers) {
      conf.SetObsForcedPeriodUShort(67);
      conf.SetObsBatVoltToLowPwrShort(0);
   }
}

void TSnCommActHiRate::SetRunInfo(TSnRunInfo& runf,
                                  const TString& clbl,
                                  const TString& stnlbl,
                                  const Int_t svers) {
   runf.SetConfLabel(clbl.Data());
   runf.SetStationLabel(stnlbl);
   runf.SetRun(0);
   if (svers<=TSnIOStatusFrame::k2012to2013IOVers) {
      runf.SetObsFirstEvt(0);
   } else {
      runf.SetFirstSeq(0);
   }
   runf.SetEvtsPerSeq(100);
}
