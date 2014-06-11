#include "TSnChanTimeCorlAltEvtsMod.h"

#include <TGraph.h>
#include <TObjArray.h>

#include "NSnConstants.h"
#include "TSnSpectral.h"
#include "TSnCalWvData.h"
#include "TSnInterp1DSpline3.h"

#include <algorithm>
#include <stdexcept>

ClassImp(TSnChanTimeCorlAltEvtsMod);


void TSnChanTimeCorlAltEvtsMod::SlaveBegin() {
   ReqCalData(fEvtBrA.Data(), fDatA);
   ReqCalData(fEvtBrB.Data(), fDatB);
}


void TSnChanTimeCorlAltEvtsMod::Process() {
   // find the correlations. correlations will be versus
   // (event A - event B) shifts
   
   LoadCalData(fEvtBrA.Data());
   LoadCalData(fEvtBrB.Data());
   if ( (fDatA!=0) && (fDatB!=0) ) {

      const TSnCalWvData* envA(0), * envB(0);
      if (IsCheckingEnvelope()) {
         envA = dynamic_cast<const TSnCalWvData*>(
            FindObjThisEvt(fEnvNmA.Data()));
         envB = dynamic_cast<const TSnCalWvData*>(
            FindObjThisEvt(fEnvNmB.Data()));
         if ( (envA==0) || (envB==0) ) {
            return SendError(kAbortAnalysis, "Process",
                             "Could not get envelopes. envA [%s] = (%p) "
                             "envB [%s] = (%p)",
                             fEnvNmA.Data(), static_cast<const void*>(envA),
                             fEnvNmB.Data(), static_cast<const void*>(envB));
         }
      }
      
      // corls will be deleted by TAMSelector after all modules' Process
      // has run, since it is added to the event objects
      TObjArray* corls = new TObjArray(NSnConstants::kNchans);
      corls->SetOwner(kTRUE);
      
      TString hn;
      TGraph* g(0);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         if (fCorlType==kWrapCorl) {
            g = TSnSpectral::NewCorrelateGraph( fDatA->GetData(ch),
                                                fDatB->GetData(ch),
                                                NSnConstants::kNsamps,
                                                kTRUE );
            // put in units of time
            Double_t* x = g->GetX();
            for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++x) {
               *x /= NSnConstants::kSampRate;
            }
         } else if (fCorlType==kScanToEnvMax) {
            const Float_t* ea = envA->GetData(ch);
            const Float_t* eb = envB->GetData(ch);
            const Float_t* ma = std::max_element(ea, ea+NSnConstants::kNsamps);
            const Float_t* mb = std::max_element(eb, eb+NSnConstants::kNsamps);
            TGraph* gs =
               TSnSpectral::NewScanPrsCorrCoefGraphUneq(fDatA->GetData(ch),
                                                        ma-ea,
                                                        fDatB->GetData(ch),
                                                        mb-eb);
            g = new TGraph(NSnConstants::kNsamps+1);
            Double_t*  x = g->GetX();
            Double_t*  y = g->GetY();
            Double_t* sx = gs->GetX();
            Double_t* sy = gs->GetY();
            const Double_t* const syend = sy + gs->GetN();
            static const Int_t hlfs = NSnConstants::kNsamps >> 1; // N/2
            const Int_t first = static_cast<Int_t>(*sx);
            for (Int_t sm=-hlfs; sm<hlfs+1; ++sm, ++x, ++y) {
               *x = sm / NSnConstants::kSampRate;
               if ((sm>=first) && (sy<syend)) {
                  *y = *sy;
                  // no need to increment sx
                  ++sy;
               } else {
                  *y = 0;
               }
            }
            delete gs;
         } else {
            SendError(kAbortAnalysis,"Process",
                      "Unknown correlation type [%d]",
                      static_cast<Int_t>(fCorlType));
         }
         hn = GetChanCorlInterpName(ch);
         if (g!=0) {
            corls->AddAt(new TSnInterp1DSpline3(hn.Data(),
                                                ";time shift (ns); correlation",
                                                g),
                         ch );
            delete g;
         }
         
      }
      
      // add corls to the event
      AddObjThisEvt(corls, fCorlNm);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get data from [%s] (%p) and/or [%s] (%p)",
                fEvtBrA.Data(), static_cast<const void*>(fDatA),
                fEvtBrB.Data(), static_cast<const void*>(fDatB));
   }
}

void TSnChanTimeCorlAltEvtsMod::SetCorlType(const TString t) {
   if (t.CompareTo(GetCorlTypeStr(kWrapCorl))==0) {
      fCorlType = kWrapCorl;
   } else if (t.CompareTo(GetCorlTypeStr(kScanToEnvMax))==0) {
      fCorlType = kScanToEnvMax;
   } else {
      throw std::invalid_argument(
         Form("<TSnChanTimeCorlAltEvtsMod::SetCorlType>: "
              "Unknown correlation type [%s].", t.Data()));
   }
}
   
const Char_t* TSnChanTimeCorlAltEvtsMod::GetCorlTypeStr(
                          const TSnChanTimeCorlAltEvtsMod::ECorlType_t t) {
   switch (t) {
      case kWrapCorl:
         return "WrapCorl";
      case kScanToEnvMax:
         return "ScanToEnvMax";
      default:
         break;
   };
   throw std::invalid_argument(
      Form("<TSnChanTimeCorlAltEvtsMod::GetCorlTypeStr>: "
           "Unknown correlation type [%d].", static_cast<Int_t>(t)));
   return NULL;
}

