#include "TSnChanTimeCorlMod.h"

#include <TObjArray.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TSpline.h>

#include "TSnSpectral.h"
#include "TSnRecoChanOffsets.h"
#include "TSnCalWvData.h"
#include "TSnInterp1DSpline3.h"
#include "TSnPlotBrowser.h"

ClassImp(TSnChanTimeCorlMod);

const UInt_t TSnChanTimeCorlMod::kDebugCorls = 100;

void TSnChanTimeCorlMod::SlaveBegin() {
   ReqCalData(fWvDatNm.Data(), fDat);
}

void TSnChanTimeCorlMod::Process() {
   LoadCalData(fWvDatNm.Data());
   if (fDat!=0) {
      // fCorls will be deleted by TAMSelector after all modules' Process
      // has run, since it is added to the event objects
      fCorls = new TObjArray(TSnRecoChanOffsets::kChCmbs);
      fCorls->SetOwner(kTRUE);

      const TSnCalWvData* env(0);
      if (IsCheckingEnvelope()) {
         env = dynamic_cast<const TSnCalWvData*>(
            FindObjThisEvt(fEnvNm.Data()));
         if (env==0) {
            return SendError(kAbortAnalysis, "Process",
                             "Could not get envelope named [%s].",
                             fEnvNm.Data());
         }
      }
      
      TString hn;
      TGraph* g(0);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (Int_t xc=0; xc<ch; ++xc) {
            if (fCorlType==kWrapCorl) {
               g = TSnSpectral::NewCorrelateGraph( fDat->GetData(ch),
                                                   fDat->GetData(xc),
                                                   NSnConstants::kNsamps,
                                                   kTRUE );
               // put in units of time
               Double_t* x = g->GetX();
               for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++x) {
                  *x /= NSnConstants::kSampRate;
               }
            } else if (fCorlType==kScanToEnvMax) {
               const Float_t* ea = env->GetData(ch);
               const Float_t* ma = 
                  std::max_element(ea, ea+NSnConstants::kNsamps);
               const Float_t* eb = env->GetData(xc);
               const Float_t* mb = 
                  std::max_element(eb, eb+NSnConstants::kNsamps);
               TGraph* gs = 
                  TSnSpectral::NewScanPrsCorrCoefGraphUneq(fDat->GetData(ch),
                                                           ma-ea,
                                                           fDat->GetData(xc),
                                                           mb-eb);
               if ((gs!=0) && (gs->GetN()>0)) {
                  g = new TGraph(NSnConstants::kNsamps);
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
                            "ch%hhu max at %d, ch%hhu max at %d",
                            ch, ma-ea, xc, mb-eb);
               }
               delete gs;
            } else {
               SendError(kAbortAnalysis,"Process",
                         "Unknown correlation type [%d]",
                         static_cast<Int_t>(fCorlType));
            }
            hn = GetChanCorlInterpName(ch, xc);
            fCorls->AddAt( new TSnInterp1DSpline3(hn.Data(),
                                                  Form(";time shift "
                                                       "(ch%d - ch%d) (ns); "
                                                       "correlation", ch, xc),
                                                  g),
                           TSnRecoChanOffsets::IndexFor(ch, xc) );
            delete g;
         }
      }
      
      // add fCorls to the event
      AddObjThisEvt(fCorls, fCorlNm);


      // debugging?
      if (GetVerbosity()>=kDebugCorls) {
         TCanvas* c = new TCanvas("cctc","cctc - chan time corl",1200,1000);
         TSnPlotBrowser* pb = new TSnPlotBrowser("ctcpb");
         pb->SetCanvas(c);
         TObjArray graphs;
         graphs.SetOwner(kTRUE);
         static const Int_t cols[NSnConstants::kNchans] =
               { kOrange+7, kAzure-2, kViolet-1, kGreen+2 };
         static const Int_t mrk = 7;
         Int_t pi=0;
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            for (UChar_t xc=0; xc<ch; xc++) {
               pi = TSnRecoChanOffsets::IndexFor(ch, xc);
               
               g = fDat->NewGraphForChan(ch, kTRUE);
               graphs.Add(g);
               g->SetLineColor(cols[ch]);
               g->SetMarkerColor(cols[ch]);
               g->SetMarkerStyle(mrk);
               pb->AddPlotToSet("Waveforms",g, "apl", pi+1);

               g = fDat->NewGraphForChan(xc, kTRUE);
               graphs.Add(g);
               g->SetLineColor(cols[xc]);
               g->SetMarkerColor(cols[xc]);
               g->SetMarkerStyle(mrk);
               pb->AddPlotToSet("Waveforms",g, "pl", pi+1);

               if (env!=0) {
                  g = env->NewGraphForChan(ch, kTRUE);
                  graphs.Add(g);
                  g->SetLineColor(cols[ch]);
                  pb->AddPlotToSet("Waveforms",g,"l",pi+1);
                  g = env->NewGraphForChan(xc, kTRUE);
                  graphs.Add(g);
                  g->SetLineColor(cols[xc]);
                  pb->AddPlotToSet("Waveforms",g,"l",pi+1);
               }
               
               TSnInterp1DSpline3* s = dynamic_cast<TSnInterp1DSpline3*>(
                  fCorls->At(pi));
               if (s!=0) {
                  s->GetSpline()->SetLineColor(cols[1]);
                  s->GetSpline()->SetMarkerColor(cols[1]);
                  s->GetSpline()->SetMarkerStyle(mrk);
                  pb->AddPlotToSet("Corls",s,"pl",pi+1);
               }
               
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

      }
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get data [%s]",
                fWvDatNm.Data());
   }   
}


void TSnChanTimeCorlMod::SetCorlType(const TString t) {
   if (t.CompareTo(GetCorlTypeStr(kWrapCorl))==0) {
      fCorlType = kWrapCorl;
   } else if (t.CompareTo(GetCorlTypeStr(kScanToEnvMax))==0) {
      fCorlType = kScanToEnvMax;
   } else {
      throw std::invalid_argument(
         Form("<TSnChanTimeCorlMod::SetCorlType>: "
              "Unknown correlation type [%s].", t.Data()));
   }
}
   
const Char_t* TSnChanTimeCorlMod::GetCorlTypeStr(
                          const TSnChanTimeCorlMod::ECorlType_t t) {
   switch (t) {
      case kWrapCorl:
         return "WrapCorl";
      case kScanToEnvMax:
         return "ScanToEnvMax";
      default:
         break;
   };
   throw std::invalid_argument(
      Form("<TSnChanTimeCorlMod::GetCorlTypeStr>: "
           "Unknown correlation type [%d].", static_cast<Int_t>(t)));
   return NULL;
}

