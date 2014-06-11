#include "TSnCommFiActTransComplete.h"

#include <TString.h>
#include <TTimeStamp.h>

#include "TSnIOHeaderFrame.h"
#include "TSnCommProtocolMsg.h"
#include "TSnCommAlertPack.h"
#include "TSnStatusUpdate.h"
#include "TSnCommIncFile.h"

ClassImp(TSnCommFiActTransComplete);


TSnCommObject* TSnCommFiActTransComplete::operator()(const TSnCommIncFile& in) {
   
   TSnCommObject* o(0);
   
   // compiler should CSE all the getters

   const TString& fn = in.GetFilename();
   if (fn.Length()>0) {
      const TSnStatusUpdate* status = in.GetStatus();
      const TTimeStamp*      time   = in.GetTime();
      TString mac( (status!=0) ? (status->GetMacAdrAsStr()) : "UNKNOWN" );
      TString tim( (time!=0) ? (time->AsString("s")) : "UNKNOWN" );
   
      if ( (in.GetNevtsRcvd()==in.GetNevtsSent()) &&
           (in.GetNpwrsRcvd()==in.GetNpwrsSent()) ) {
         if ( (in.GetFileVersion()<4) || // no mbed crc prior to v4
              ( (in.GetMbedCRC()!=0) &&  // 0=0 is not a good match
                (in.GetMbedCRC()==in.GetLocalCRC()) ) ) {
            o = new TSnCommProtocolMsg(TSnIOHeaderFrame::kHnShOkComplCode);
         } else {
            TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
               TSnIOHeaderFrame::kHnShFailPrtCode);
            TString m(Form("Comm win with station [%s] at [%s].\r\n\r\n"
                           "CRC calculated on station [%u] differs from "
                           "CRC calculated on local file received [%u] "
                           "for file [%s].",
                           mac.Data(), tim.Data(),
                           in.GetMbedCRC(), in.GetLocalCRC(),
                           in.GetFilenameStr()));
            o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                                "TSnCommFiActTransComplete",
                                                "Received file CRC mismatch",
                                                m.Data());
         }
      } else {
         // number of evts/pwrs received differs from sent

         if ( (in.GetNevtsRcvd()>in.GetNevtsSent()) ||
              (in.GetNpwrsRcvd()>in.GetNpwrsSent()) ) {
            TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
               TSnIOHeaderFrame::kHnShFailPrtCode);
            TString m(Form("Comm win with station [%s] at [%s].\r\n\r\n"
                       "Received [%u] events and [%u] power readings.\r\n\r\n."
                       "Station reports [%u] events and [%u] power readings "
                       "were sent from file [%s].",
                       mac.Data(), tim.Data(),
                       in.GetNevtsRcvd(), in.GetNpwrsRcvd(),
                       in.GetNevtsSent(), in.GetNpwrsSent(),
                       in.GetFilenameStr()));
            o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                               "TSnCommFiActTransComplete",
                                               "Received more events/powers "
                                               "than station sent",
                                               m.Data());
         } else {
            Float_t evtfr(1.0), pwrfr(1.0); // if 0 sent, 0 rcvd is 100%
            if (in.GetNpwrsSent()>0) {
               pwrfr = static_cast<Float_t>(in.GetNpwrsRcvd()/
                                            in.GetNpwrsSent());
            }
            if (in.GetNevtsSent()>0) {
               evtfr = static_cast<Float_t>(in.GetNevtsRcvd()/
                                            in.GetNevtsSent());
            }
            if ( (evtfr>fMinEvtFrc) && (pwrfr>fMinPwrFrc) ) {
               o = new TSnCommProtocolMsg(TSnIOHeaderFrame::kHnShOkPartlCode);
            } else if ( (in.GetNevtsRcvd()>0) && (in.GetNpwrsRcvd()>0) ) {
               TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
                  TSnIOHeaderFrame::kHnShFailPrtCode);
               TString m(Form("Comm win with station [%s] at [%s].\r\n\r\n"
                             "Received insufficient fraction event,power "
                             "(%g,%g) received for file [%s]. Require (%g,%g).",
                             mac.Data(), tim.Data(),
                             evtfr,pwrfr,in.GetFilenameStr(),
                             fMinEvtFrc,fMinPwrFrc));
               o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                                  "TSnCommFiActTransComplete",
                                                  "Incoming file incomplete.",
                                                   m.Data(),
                                                   TSnCommAlertPack::kLogWarn);
            } else {
               TSnCommProtocolMsg* p = new TSnCommProtocolMsg(
                  TSnIOHeaderFrame::kHnShFailNonCode);
               TString m(Form("Comm win with station [%s] at [%s].\r\n\r\n"
                            "Station sent %u events and %u power readings, but "
                            "%u events and %u power readings were received for "
                            "file [%s].",
                            mac.Data(), tim.Data(),
                            in.GetNevtsSent(), in.GetNpwrsSent(),
                            in.GetNevtsRcvd(), in.GetNpwrsRcvd(),
                            in.GetFilenameStr()));
               o = TSnCommAlertPack::NewAlertPackPassComm(p,
                                                  "TSnCommFiActTransComplete",
                                                  "Incoming file not received.",
                                                  m.Data(),
                                                  TSnCommAlertPack::kLogWarn);
            }
         }

      }

   } // if have a filename

   return o;
}
