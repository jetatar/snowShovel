#include "TSnChanTimeCorlAltEvtsMod.h"

#include <TObjArray.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TSpline.h>

#include "NSnConstants.h"
#include "TSnSpectral.h"
#include "TSnCalWvData.h"
#include "TSnInterp1DSpline3.h"
#include "TSnPlotBrowser.h"

#include <algorithm>
#include <stdexcept>

ClassImp(TSnChanTimeCorlAltEvtsMod);

const UInt_t TSnChanTimeCorlAltEvtsMod::kDebugCorls = 100;

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
      
      TObjArray graphs; // will delete the temp graphs
      graphs.SetOwner(kTRUE);
      
      TString hn;
      TGraph* g(0), * gs(0);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         if (fCorlType==kWrapCorl) {
            g = TSnSpectral::NewCorrelateGraph( fDatA->GetData(ch),
                                                fDatB->GetData(ch),
                                                NSnConstants::kNsamps,
                                                kTRUE );
            graphs.AddAt(g, ch);
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
            gs = TSnSpectral::NewScanPrsCorrCoefGraphUneq(fDatA->GetData(ch),
                                                          ma-ea,
                                                          fDatB->GetData(ch),
                                                          mb-eb);
            graphs.AddAt(gs, ch+NSnConstants::kNchans);
            if ((gs!=0) && (gs->GetN()>0)) {
               g = new TGraph(NSnConstants::kNsamps);
               graphs.AddAt(g, ch);
               Double_t*  x = g->GetX();
               Double_t*  y = g->GetY();
               Double_t* sx = gs->GetX();
               Double_t* sy = gs->GetY();
               const Double_t* const syend = sy + gs->GetN();
               static const Int_t hlfs = NSnConstants::kNsamps >> 1; // N/2
               const Int_t first = static_cast<Int_t>(*sx);
               for (Int_t sm=-hlfs; sm<=hlfs; ++sm, ++x, ++y) {
                  *x = sm / NSnConstants::kSampRate;
                  if ((sm>=first) && (sy<syend)) {
                     *y = *sy;
                     // no need to increment sx
                     ++sy;
                  } else {
                     *y = 0;
                  }
               }
            } else {
               SendError(kAbortModule, "Process",
                         "Could not get corl graph for scan to env max. "
                         "ch%hhu, maxA at %d, maxB at %d",
                         ch, ma-ea, mb-eb);
            }
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
         }
         
      }
      
      // add corls to the event
      AddObjThisEvt(corls, fCorlNm);
      
      
      // debugging?
      if (GetVerbosity()>=kDebugCorls) {
         TCanvas* c = new TCanvas("cctcae", "cctcae - chan time corl alt evts",
                                  1200,1000);
         TSnPlotBrowser* pb = new TSnPlotBrowser("ctcaepb");
         pb->SetCanvas(c);
         static const Int_t cols[3] = { kAzure-6, kOrange+7, kGreen+2 };
         static const Int_t mrk = 7;
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            g = fDatA->NewGraphForChan(ch, kTRUE);
            graphs.Add(g);
            g->SetLineColor(cols[0]);
            g->SetMarkerColor(cols[0]);
            g->SetMarkerStyle(mrk);
            pb->AddPlotToSet("Waveforms",g, "apl", ch+1);
            g = fDatB->NewGraphForChan(ch, kTRUE);
            graphs.Add(g);
            g->SetLineColor(cols[1]);
            g->SetMarkerColor(cols[1]);
            g->SetMarkerStyle(mrk);
            pb->AddPlotToSet("Waveforms",g, "pl", ch+1);
            
            if (envA!=0) {
               g = envA->NewGraphForChan(ch, kTRUE);
               graphs.Add(g);
               g->SetLineColor(cols[0]);
               pb->AddPlotToSet("Waveforms",g,"l",ch+1);
               const Float_t* ea = envA->GetData(ch);
               const Float_t* ma = 
                  std::max_element(ea, ea+NSnConstants::kNsamps);
               Printf("ch%hhu: n1=%d",ch,static_cast<Int_t>(ma-ea));
            }
            if (envB!=0) {
               g = envB->NewGraphForChan(ch, kTRUE);
               graphs.Add(g);
               g->SetLineColor(cols[1]);
               pb->AddPlotToSet("Waveforms",g,"l",ch+1);
               const Float_t* eb = envB->GetData(ch);
               const Float_t* mb = 
                  std::max_element(eb, eb+NSnConstants::kNsamps);
               Printf("ch%hhu: n2=%d",ch,static_cast<Int_t>(mb-eb));
            }
            
            g = dynamic_cast<TGraph*>(graphs.At(ch));
            if (g!=0) {
               g->SetLineColor(cols[0]);
               g->SetMarkerColor(cols[0]);
               g->SetMarkerStyle(mrk);
               pb->AddPlotToSet("Corls",g,"apl",ch+1);
            }
            TSnInterp1DSpline3* s = dynamic_cast<TSnInterp1DSpline3*>(
               corls->At(ch));
            if (s!=0) {
               s->GetSpline()->SetLineColor(cols[1]);
               s->GetSpline()->SetMarkerColor(cols[1]);
               s->GetSpline()->SetMarkerStyle(mrk);
               pb->AddPlotToSet("Corls",s,"plsame",ch+1);
            }
            if (fCorlType==kScanToEnvMax) {
               g = dynamic_cast<TGraph*>(graphs.At(ch+NSnConstants::kNchans));
               g->SetLineColor(cols[2]);
               g->SetMarkerColor(cols[2]);
               g->SetMarkerStyle(mrk);
               pb->AddPlotToSet("Env Corls",g,"apl",ch+1);
            }
         }
         
         pb->DrawButtonBar();
         pb->DrawPlots("Waveforms");
         if (c!=0) {
            TObject* o(0);
            while ( (o = c->WaitPrimitive())!=0 ) {
               gSystem->ProcessEvents();
            }
            delete c;
         }
         delete pb;
         
      } // debug
      
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

