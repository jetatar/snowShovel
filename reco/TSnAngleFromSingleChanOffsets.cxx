#include "TSnAngleFromSingleChanOffsets.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <TObjArray.h>
#include <TArrayD.h>
#include <TMath.h>
#include <TGeoManager.h>
#include <TClass.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TSystem.h>

#include "NSnChanCorl.h"
#include "TSnGeoStnSite.h"
#include "TSnRecoChanOffsets.h"
#include "TSnRecoSingleChanOffset.h"
#include "TSnRecoSingleChanOffsetSet.h"
#include "TSnRecoDirection.h"
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"

#include <stdexcept>

ClassImp(TSnAngleFromSingleChanOffsets);

const UInt_t TSnAngleFromSingleChanOffsets::kDrawParSpace = 100;

TSnAngleFromSingleChanOffsets::TSnAngleFromSingleChanOffsets(
                                                   const Char_t* name,
                                                   const Char_t* miniName,
                                                   const Char_t* algoName) :
   TSnFittingModule(name, "reco angle from single chan offsets"),
   fChDtsNm("SingleChanTimeOffsets."),
   fChOffsNm("ChanTimeOffsets"),
   fGeoNm("AriannaGeo"),
   fRecAngNm("AngleFromSingleChanOffsets."),
   fNgTopFern(1.33),
   fDts(0), fGeo(0) {
   // normal ctor
}

Double_t TSnAngleFromSingleChanOffsets::GetAngleChi2(const Double_t* pars) {
   // get the log likelihood of a plane wave from theta,phi with 
   // the channel offsets 
   //
   // no check is made that fDts is non null, nor that it actually contains
   // a non-null pointer for each channel!

   static const TArrayD zeros(TSnRecoChanOffsets::kChCmbs);
   TArrayD pwoff = NSnChanCorl::GetPlaneWaveOffsets(pars[kTheta], pars[kPhi],
                                                    (fOffsets!=0)
                                                    ? fOffsets->GetChanOffsets()
                                                    : zeros,
                                                    fPosCh,
                                                    fNgTopFern);
   Double_t c(0), chi2(0);
   const TSnRecoSingleChanOffset* dtch, * dtxc;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      dtch = fDts->GetOffsetsOfChan(ch);
      for (UChar_t xc=0; xc<ch; ++xc) {
         dtxc = fDts->GetOffsetsOfChan(xc);
         // hopefully the compiler will optimize this a bit
         const Double_t bdt = dtch->GetTimeOffset() - dtxc->GetTimeOffset();
         const Double_t err = TMath::Sqrt( 
            (dtch->GetOffsetError()*dtch->GetOffsetError()) +
            (dtxc->GetOffsetError()*dtxc->GetOffsetError()) );
         const Double_t e = (err>0.0) ? err : 1.0;
         c = pwoff.At(TSnRecoChanOffsets::IndexFor(ch,xc)) - bdt;
         chi2 += (c*c) / (e*e);
      }
   }
   return chi2;
}

void TSnAngleFromSingleChanOffsets::SlaveBegin() {
   fOffsets = dynamic_cast<const TSnRecoChanOffsets*>(
      FindPublicObj(fChOffsNm.Data()));
   if (fOffsets==0) {
      SendError(kWarning, "SlaveBegin",
                "Could not get channel offsets [%s]. Assuming 0 offset.",
                fChOffsNm.Data());
   }
   fGeo = 0;
   const TObject* gg = FindPublicObj(fGeoNm.Data());
   if (gg!=0) {
      const TClass* ggc = gg->IsA();
      if (ggc->InheritsFrom(TGeoManager::Class())) {
         const TGeoManager* gm = static_cast<const TGeoManager*>(gg);
         fGeo = dynamic_cast<const TSnGeoStnSite*>(gm->GetTopVolume());
      } else if (ggc->InheritsFrom(TSnGeoStnSite::Class())) {
         fGeo = dynamic_cast<const TSnGeoStnSite*>(gg);
      } else {
         SendError(kAbortAnalysis, "SlaveBegin",
                   "Unable to handle geo object [%s] of type [%s].",
                   fGeoNm.Data(), ggc->GetName());
      }
   }
   if (fGeo==0) {
      SendError(kAbortAnalysis, "SlaveBegin",
                "Could not get geometry [%s].",
                fGeoNm.Data());
   }
   // get the positions of each antenna
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      try {
         fGeo->SetLPDAPosition(ch, fPosCh[ch]);
      } catch (const std::runtime_error& e) {
         SendError(kAbortAnalysis, "GetAngleLL",
                   "Error while getting LPDA position for ch %hhu: [%s]",
                   ch, e.what());
      }
   }
   
   
   ReqBranch(fChDtsNm.Data(), fDts);
   if (GetVerbosity()>=kDrawParSpace) {
      ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMeta);
      ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   }
}

void TSnAngleFromSingleChanOffsets::Process() {

   LoadBranch(fChDtsNm.Data());
   if (fDts==0) {
      SendError(kAbortAnalysis, "Process",
                "Could not get channel delays [%s].",
                fChDtsNm.Data());
   } else {

      ROOT::Math::Minimizer& mini = GetMiniSafe();
      mini.Clear();
      ROOT::Math::Functor fcn(this, 
                              &TSnAngleFromSingleChanOffsets::GetAngleChi2,
                              kNpars);
      mini.SetFunction(fcn);
      static const Double_t thetaMin = TMath::Pi() / 2.0,
                            thetaMax = TMath::Pi(),
                            phiMin   = 0.0,
                            phiMax   = TMath::TwoPi();
      mini.SetLimitedVariable(kTheta, GetParName(kTheta), 3.0, 0.001,
                              thetaMin, thetaMax);
      mini.SetLimitedVariable(kPhi, GetParName(kPhi), 5.5, 0.001,
                              phiMin, phiMax);
      mini.SetErrorDef(1.0); // because chi2
      mini.Minimize();
      const Double_t* const pars = mini.X();
      const Double_t* const errs = mini.Errors();
      
      if (GetVerbosity()>=kDrawParSpace) {
         
         LoadBranch(TSnRawTreeMaker::kEMtBrNm);
         LoadBranch(TSnRawTreeMaker::kEHdBrNm);
         
         TCanvas* cdbgtp = new TCanvas("cdbgtp","debug theta phi",1000,900);
         cdbgtp->cd();
         TH2F* htp = new TH2F("htp",
                              Form("run=%u, evt=%u;theta;phi;ll",
                                   (fMeta!=0) ? (fMeta->GetRunNum()) : 0,
                                   (fHdr!=0) ? (fHdr->GetEvtNum()) : 0),
                              90.0, thetaMin*TMath::RadToDeg(),
                              thetaMax*TMath::RadToDeg(),
                              360.0, phiMin*TMath::RadToDeg(),
                              phiMax*TMath::RadToDeg());
         const Int_t nbinsx = htp->GetNbinsX(),
                     nbinsy = htp->GetNbinsY();
         Double_t tp[kNpars];
         for (Int_t xi=nbinsx; xi>0; --xi) {
            tp[kTheta] = htp->GetXaxis()->GetBinCenter(xi) * TMath::DegToRad();
            for (Int_t yi=nbinsy; yi>0; --yi) {
               tp[kPhi] = htp->GetYaxis()->GetBinCenter(yi) * TMath::DegToRad();
               htp->SetCellContent(xi, yi, GetAngleChi2(tp));
            }
         }
         htp->SetStats(0);
         htp->Draw("colz");
         TGraphErrors* gtp = new TGraphErrors(1);
         gtp->SetPoint(0, pars[kTheta]*TMath::RadToDeg(),
                       pars[kPhi]*TMath::RadToDeg());
         if (mini.IsValidError()) {
            gtp->SetPointError(0, errs[kTheta]*TMath::RadToDeg(),
                               errs[kPhi]*TMath::RadToDeg());
         }
         gtp->SetMarkerStyle(31);
         gtp->SetMarkerColor(kSpring);
         gtp->Draw("p");
         cdbgtp->SetLogz();
         cdbgtp->Update();
         TObject* o(0);
         while ( (o = cdbgtp->WaitPrimitive())!=0 ) {
            gSystem->ProcessEvents();
         }
         delete cdbgtp;
         delete htp;
         delete gtp;
      }
      
      // this will be deleted by the selector after all modules' Process
      // has finished, since we're adding it to the event
      TSnRecoDirection* r = new TSnRecoDirection(fRecAngNm.Data(),
                                                 GetDefMinTag(),
                                                 pars[kTheta], 
                                                 (mini.IsValidError()
                                                    ? errs[kTheta] : 0),
                                                 pars[kPhi],   
                                                 (mini.IsValidError()
                                                    ? errs[kPhi] : 0),
                                                 mini.MinValue());
      AddObjThisEvt(r);

   }
   
}

