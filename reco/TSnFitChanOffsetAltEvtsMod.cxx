#include "TSnFitChanOffsetAltEvtsMod.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TNtuple.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TSystem.h>

#include "NSnChanCorl.h"
#include "NSnConstants.h"
#include "TSnCalWvData.h"
#include "TSnRecoSingleChanOffset.h"
#include "TSnRecoSingleChanOffsetSet.h"
#include "TSnInterp1DWvData.h"
#include "TSnInterp1DSpline3.h"
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"
#include "TSnPlotBrowser.h"
#include "TSnSpectral.h"

#include <stdexcept>

ClassImp(TSnFitChanOffsetAltEvtsMod);

const UInt_t TSnFitChanOffsetAltEvtsMod::kMakeScanNtp = 100;
const UInt_t TSnFitChanOffsetAltEvtsMod::kDebugFit    = 150;

TSnFitChanOffsetAltEvtsMod::~TSnFitChanOffsetAltEvtsMod() {
   delete fNt;
}

Double_t TSnFitChanOffsetAltEvtsMod::GetCorlDelayLL(
                                               const Double_t* dts) const {
   const Double_t p = NSnChanCorl::GetProbForDTFromCorl(dts[kDt], *fCorl);
   return -TMath::Log(p);
}

Double_t TSnFitChanOffsetAltEvtsMod::GetEnvRiseDistLL(
                                               const Double_t* dts) const {
   const Double_t l = NSnChanCorl::GetLLfromDistandOffset(fEnvA, fEnvB,
                                                          fEnvNoiseCut,
                                                          fEnvMaxFrac,
                                                          dts[kDt]);
   return l;
}

Double_t TSnFitChanOffsetAltEvtsMod::GetCorlAndEnvRiseDistDelayLL(
                                               const Double_t* dts) const {
   const Double_t lcorl = GetCorlDelayLL(dts);
   const Double_t lenvd = GetEnvRiseDistLL(dts);
   // -ln(Ptot) = -ln( p_corl * p_envd ) = -[ ln(p_corl) + ln(p_envd) ]
   //           = [-ln(p_corl)] + [-ln(p_envd)]
   return lcorl + lenvd;
}

Double_t TSnFitChanOffsetAltEvtsMod::GetEnvMatchLL(
                                               const Double_t* dts) const {
   return NSnChanCorl::GetProbFromNormCorl( fEnvCorl->Eval(dts[kDt]) );
}

Double_t TSnFitChanOffsetAltEvtsMod::GetCorlAndEnvMatchLL(
                                               const Double_t* dts) const {
   const Double_t lcorl = GetCorlDelayLL(dts);
   const Double_t lenvd = GetEnvMatchLL(dts);
   // -ln(Ptot) = -ln( p_corl * p_envd ) = -[ ln(p_corl) + ln(p_envd) ]
   //           = [-ln(p_corl)] + [-ln(p_envd)]
   return lcorl + lenvd;
}

void TSnFitChanOffsetAltEvtsMod::SlaveBegin() {
   
   if (GetVerbosity()>=kMakeScanNtp) {
      ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMeta);
      ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
      fNt = new TNtuple("FitChOffAE","TSnFitChanOffsetAltEvtsMod fit space",
                        "run:evt:ch:dt:lik");
      AddOutput(fNt);
   }

}

void TSnFitChanOffsetAltEvtsMod::Process() {
   
   const Bool_t useCorl = IsCheckingCorl();
   const Bool_t useEnvs = IsCheckingEnvelope();
   
   Bool_t corlsOk(kFALSE);
   const TObjArray* corls = 0;
   if (useCorl) {
      corls = dynamic_cast<const TObjArray*>(
         FindObjThisEvt(fCorlNm.Data()));
      if (corls==0) {
         SendError(kAbortModule, "Process",
                   "Could not get single channel correlation functions "
                   "named [%s].", fCorlNm.Data());
      } else {
         corlsOk=kTRUE;
      }
   } else {
      corlsOk=kTRUE;
   }
   
   Bool_t envOk(kFALSE);
   const TSnCalWvData* envA(0), * envB(0);
   if (useEnvs) {
      envA = dynamic_cast<const TSnCalWvData*>(FindObjThisEvt(fEnvNmA.Data()));
      envB = dynamic_cast<const TSnCalWvData*>(FindObjThisEvt(fEnvNmB.Data()));
      if ( (envA==0) || (envB==0) ) {
         SendError(kAbortModule, "Process",
                   "Could not get waveform envelopes named [%s] and/or [%s]",
                   fEnvNmA.Data(), fEnvNmB.Data());
      } else {
         envOk=kTRUE;
      }
   } else {
      envOk=kTRUE;
   }
   
   if (corlsOk && envOk) {

      if (GetVerbosity()>=kMakeScanNtp) {
         LoadBranch(TSnRawTreeMaker::kEMtBrNm);
         LoadBranch(TSnRawTreeMaker::kEHdBrNm);
      }
         
      // this will be deleted by the selector after all modules' Process
      // has finished, since we're adding it to the event
      TSnRecoSingleChanOffsetSet* off = 
         new TSnRecoSingleChanOffsetSet(fDtsNm.Data(), GetDefMinTag());
      
      // to delete the envelope correlation fcns
      TObjArray envCorls(NSnConstants::kNchans);
      envCorls.SetOwner(kTRUE);
      
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         
         Double_t startDt(0);
         if (useEnvs) {
            fEnvA = envA->GetData(ch);
            fEnvB = envB->GetData(ch);
            TGraph* gEnvCorl =
               TSnSpectral::NewCorrelateGraph(fEnvA, fEnvB,
                                              NSnConstants::kNsamps,
                                              kTRUE);
            Double_t* x = gEnvCorl->GetX();
            for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++x) {
               *x /= NSnConstants::kSampRate;
            }
            fEnvCorl = new TSnInterp1DSpline3(Form("EnvCorl_ch%hhu",ch),
                                              ";time shift (ns);correlation",
                                              gEnvCorl);
            envCorls.AddAt(fEnvCorl, ch);
            delete gEnvCorl;
         }
         if (useCorl) {
            fCorl = dynamic_cast<const TSnInterp1DWvData*>(corls->At(ch));
            startDt = fCorl->FindXAtMaximum();
         }
         
         ROOT::Math::Minimizer& mini = GetMiniSafe();
         mini.Clear();
         SetMiniFitFunction();
         if ( GetMinimizerName().CompareTo("Genetic")==0 ) {
            mini.SetLimitedVariable(kDt, GetParName(kDt),
                                    startDt, 0.01,
                                    -NSnChanCorl::kMaxDt,
                                    NSnChanCorl::kMaxDt);
         } else {
            mini.SetVariable(kDt, GetParName(kDt),
                             startDt, 0.01);
         }
         mini.SetErrorDef(0.5); // because neg. log likelihood
         
         mini.Minimize();
         const Double_t* const pars = mini.X();
         const Double_t* const errs = mini.Errors();
         const TSnRecoSingleChanOffset choff(fDtsNm.Data(),
                                             GetDefMinTag(),
                                             pars[kDt], 
                                             (mini.IsValidError() 
                                              ? errs[kDt] : 0),
                                             mini.MinValue());
         off->SetOffsetsOfChan(ch, choff);
         
         if (GetVerbosity()>=kMakeScanNtp) {
            static const Int_t   steps = 100;
            static const Double_t step = (2.0*NSnChanCorl::kMaxDt)
               / static_cast<Double_t>(steps);
            Double_t dt = -NSnChanCorl::kMaxDt;
            for (Int_t i=0; i<steps; ++i, dt+=step) {
               const Double_t lik = (GetFunctorSafe())(&dt);
               fNt->Fill(fMeta->GetRunNum(), fHdr->GetEvtNum(),
                         ch, dt, lik);
            }
         }
      }
      
      AddObjThisEvt(off, fDtsNm.Data());


      if (GetVerbosity()>=kDebugFit) {
         // for debugging: draw stuff
         TObjArray graphs; // to delete our graphs for us at the end
         graphs.SetOwner(kTRUE);
         TCanvas* c = new TCanvas("cfcoae","cfcoae - fit chan offset alt evts",
                                  1200,1100);
         TSnPlotBrowser* pb = new TSnPlotBrowser("fitchaltpb");
         pb->SetCanvas(c);
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            if (useEnvs) {
               TGraph* gEnvA = envA->NewGraphForChan(ch, kTRUE);
               TGraph* gEnvB = envB->NewGraphForChan(ch, kTRUE);
               graphs.Add(gEnvA);
               graphs.Add(gEnvB);
               gEnvA->SetLineColor(kAzure-6);
               gEnvA->SetMarkerColor(kAzure-6);
               gEnvA->SetMarkerStyle(7);
               gEnvB->SetLineColor(kOrange+7);
               gEnvB->SetMarkerColor(kOrange+7);
               gEnvB->SetMarkerStyle(7);
               pb->AddPlotToSet("Envelopes",gEnvB, "apl", ch+1,
                                TSnPlotBrowser::k1DDefaults);
               pb->AddPlotToSet("Envelopes",gEnvA,  "pl", ch+1,
                                TSnPlotBrowser::k1DDefaults);

               fEnvA = envA->GetData(ch);
               fEnvB = envB->GetData(ch);
               TGraph* gEnvRDL = new TGraph;
               for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
                  const Double_t dt = (sm/NSnConstants::kSampRate) -
                     (0.5*NSnConstants::kNsamps/NSnConstants::kSampRate);
                  gEnvRDL->SetPoint( gEnvRDL->GetN(),
                                     dt, GetEnvRiseDistLL(&dt) );
               }
               pb->AddPlotToSet("EnvDistLL",gEnvRDL,"apl",ch+1,
                                TSnPlotBrowser::k1DDefaults);
               pb->AddPlotToSet("EnvelopeCorl",envCorls.At(ch),"pl",ch+1);
            }
            if (useCorl) {
               fCorl = dynamic_cast<const TSnInterp1DWvData*>(corls->At(ch));
               pb->AddPlotToSet("Correlations",
                                const_cast<TSnInterp1DWvData*>(fCorl), // ew!
                                "pl",ch+1);
               TGraph* gCorDL = new TGraph;
               graphs.Add(gCorDL);
               for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
                  const Double_t dt = (sm/NSnConstants::kSampRate) -
                     (0.5*NSnConstants::kNsamps/NSnConstants::kSampRate);
                  gCorDL->SetPoint( gCorDL->GetN(),
                                    dt, GetCorlDelayLL(&dt) );
               }
               pb->AddPlotToSet("CorlDelLL",gCorDL,"apl",ch+1,
                                TSnPlotBrowser::k1DDefaults);
            }
            if (useEnvs && useCorl) {
               TGraph* gCorEnvDL = new TGraph;
               graphs.Add(gCorEnvDL);
               TGraph* gCorEnvMatchDL = new TGraph;
               graphs.Add(gCorEnvMatchDL);
               for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
                  const Double_t dt = (sm/NSnConstants::kSampRate) -
                     (0.5*NSnConstants::kNsamps/NSnConstants::kSampRate);
                  gCorEnvDL->SetPoint( gCorEnvDL->GetN(),
                                       dt, GetCorlAndEnvRiseDistDelayLL(&dt) );
                  gCorEnvMatchDL->SetPoint( gCorEnvMatchDL->GetN(),
                                            dt, GetCorlAndEnvMatchLL(&dt) );
               }
               pb->AddPlotToSet("CorlEnvDistLL",gCorEnvDL,"apl",ch+1,
                                TSnPlotBrowser::k1DDefaults);
               pb->AddPlotToSet("CorlEnvMatchLL",gCorEnvMatchDL,"apl",ch+1,
                                TSnPlotBrowser::k1DDefaults);
            }
            off->Print();
         }
         
         pb->DrawButtonBar();
         if (useCorl) {
            pb->DrawPlots("Correlations");
         } else {
            pb->DrawPlots("EnvDistLL");
         }

         if (c!=0) {
            TObject* o(0);
            while ( (o = c->WaitPrimitive())!=0 ) {
               gSystem->ProcessEvents();
            }
            delete c;
         }
         delete pb;
         
      }
      
   }
      
}

void TSnFitChanOffsetAltEvtsMod::SetMiniFitFunction() {
   if (fFitType==kCorlOnly) {
      ROOT::Math::Functor fcn(this,
                     &TSnFitChanOffsetAltEvtsMod::GetCorlDelayLL,
                              kNpars);
      SetFunctor(fcn);
   } else if (fFitType==kEnvRiseDistOnly) {
      ROOT::Math::Functor fcn(this,
                     &TSnFitChanOffsetAltEvtsMod::GetEnvRiseDistLL,
                              kNpars);
      SetFunctor(fcn);
   } else if (fFitType==kCorlAndEnvRiseDist) {
      ROOT::Math::Functor fcn(this,
                     &TSnFitChanOffsetAltEvtsMod::GetCorlAndEnvRiseDistDelayLL,
                              kNpars);
      SetFunctor(fcn);
   } else if (fFitType==kEnvMatchOnly) {
      ROOT::Math::Functor fcn(this,
                     &TSnFitChanOffsetAltEvtsMod::GetEnvMatchLL,
                              kNpars);
      SetFunctor(fcn);
   } else if (fFitType==kCorlAndEnvMatch) {
      ROOT::Math::Functor fcn(this,
                     &TSnFitChanOffsetAltEvtsMod::GetCorlAndEnvMatchLL,
                              kNpars);
      SetFunctor(fcn);
   } else {
      throw std::invalid_argument(
         Form("<TSnFitChanOffsetAltEvtsMod::SetMiniFitFunction>: "
              "Unknown fit type [%d]", static_cast<Int_t>(fFitType)));
   }
}

void TSnFitChanOffsetAltEvtsMod::SetFitType(const TString t) {
   if (t.CompareTo(GetFitTypeStr(kCorlOnly))==0) {
      SetFitType(kCorlOnly);
   } else if (t.CompareTo(GetFitTypeStr(kEnvRiseDistOnly))==0) {
      SetFitType(kEnvRiseDistOnly);
   } else if (t.CompareTo(GetFitTypeStr(kCorlAndEnvRiseDist))==0) {
      SetFitType(kCorlAndEnvRiseDist);
   } else if (t.CompareTo(GetFitTypeStr(kEnvMatchOnly))==0) {
      SetFitType(kEnvMatchOnly);
   } else if (t.CompareTo(GetFitTypeStr(kCorlAndEnvMatch))==0) {
      SetFitType(kCorlAndEnvMatch);
   } else {
      throw std::invalid_argument(
         Form("<TSnFitChanOffsetAltEvtsMod::SetFitType>: "
              "Unknown fit type [%s].", t.Data()));
   }
}

const Char_t* TSnFitChanOffsetAltEvtsMod::GetFitTypeStr(
                        const TSnFitChanOffsetAltEvtsMod::EFitType_t t) {

   switch (t) {
      case kCorlOnly:
         return "CorlOnly";
      case kEnvRiseDistOnly:
         return "EnvRiseDistOnly";
      case kCorlAndEnvRiseDist:
         return "CorlAndEnvRiseDist";
      case kEnvMatchOnly:
         return "EnvMatchOnly";
      case kCorlAndEnvMatch:
         return "CorlAndEnvMatch";
      default:
         break;
   };
   throw std::invalid_argument(
      Form("<TSnFitChanOffsetAltEvtsMod::GetFitTypeStr>: "
           "Unknown fit type [%d]", static_cast<Int_t>(t)));
   return NULL;
}
