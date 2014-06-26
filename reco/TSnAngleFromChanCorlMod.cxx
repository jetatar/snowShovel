#include "TSnAngleFromChanCorlMod.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <TMath.h>
#include <TVector3.h>
#include <TGeoManager.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <TArrayD.h>

#include "TSnInterp1DWvData.h"
#include "TSnRecoDirection.h"
#include "TSnRecoChanOffsets.h"
#include "NSnConstants.h"
#include "NSnChanCorl.h"
#include "TSnGeoStnSite.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

#include <stdexcept>

const UInt_t TSnAngleFromChanCorlMod::kDrawParSpace = 100;

TSnAngleFromChanCorlMod::TSnAngleFromChanCorlMod(const Char_t* name,
                                                 const Char_t* miniName,
                                                 const Char_t* algoName) :
   TSnFittingModule(name, "reco angle from chan correlations",
                    miniName, algoName),
   fChDtsNm("ChanDtOffets"),
   fGeoNm("AriannaGeo"), fCorlNm("CrossChanCorls."),
   fRecAngNm("AngleFromChanCorl."),
   fNgTopFern(1.33),
   fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
   fThetaStr(3.0), fPhiStr(5.5), fNrndStr(0), fThetaScn(0), fPhiScn(0),
   fDts(0), fGeo(0), fCorls(0),
   fHdr(0), fMeta(0) {
   // normal ctor
}

Double_t TSnAngleFromChanCorlMod::GetAngleLL(const Double_t* pars) {

   // get the log likelihood of a plane wave from theta,phi with 
   // the channel offsets found by GetPlaneWaveOffsets
   
   //return NSnChanCorl::GetLLfromDTsForPlaneWv(
   return NSnChanCorl::GetLLfromParallelChDTsForPlaneWv(
      NSnChanCorl::GetPlaneWaveOffsets(pars[kTheta], pars[kPhi],
                                       fDts->GetChanOffsets(),
                                       fPosCh,
                                       fNgTopFern),
      *fCorls,
      fCsig);
}

void TSnAngleFromChanCorlMod::SlaveBegin() {
   
   fDts = dynamic_cast<const TSnRecoChanOffsets*>(
      FindPublicObj(fChDtsNm.Data()));
   if (fDts==0) {
      SendError(kAbortAnalysis, "SlaveBegin",
                "Could not get channel offsets [%s].",
                fChDtsNm.Data());
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

   if (GetVerbosity()>=kDrawParSpace) {
      ReqBranch(TSnRawTreeMaker::kEMtBrNm, fMeta);
      ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   }
}

void TSnAngleFromChanCorlMod::DoFit(const Double_t thetaStart,
                                    const Double_t thetaMin,
                                    const Double_t thetaMax,
                                    const Double_t phiStart,
                                    const Double_t phiMin,
                                    const Double_t phiMax,
                                    TArrayD& pars,
                                    TArrayD& errs,
                                    Bool_t& isValidErr,
                                    Double_t& minValue) {
      
      ROOT::Math::Minimizer& mini = GetMiniSafe();
      mini.Clear();
      ROOT::Math::Functor fcn(this, 
                              &TSnAngleFromChanCorlMod::GetAngleLL,
                              kNpars);
      mini.SetFunction(fcn);
      mini.SetLimitedVariable(kTheta, GetParName(kTheta), thetaStart, 0.001,
                              thetaMin, thetaMax);
      mini.SetLimitedVariable(kPhi, GetParName(kPhi), phiStart, 0.001,
                              phiMin, phiMax);
      mini.SetErrorDef(0.5); // because neg. log likelihood
      mini.Minimize();
      
      const UInt_t ndim = mini.NDim();
      const Double_t* p = mini.X();
      const Double_t* e = mini.Errors();
      isValidErr        = mini.IsValidError();
      minValue          = mini.MinValue();
      for (UInt_t i=0; i<ndim; ++i, ++p, ++e) {
         pars[i] = *p;
         errs[i] =  (isValidErr) ? *e : 0.0;
      }
      
}

void TSnAngleFromChanCorlMod::Process() {
   
   fCorls = dynamic_cast<const TObjArray*>(FindObjThisEvt(fCorlNm.Data()));
   if (fCorls!=0) {

      static const Double_t thetaMin = TMath::Pi() / 2.0,
                            thetaMax = TMath::Pi(),
                            phiMin   = 0.0,
                            phiMax   = TMath::TwoPi();
      
      if ( (fThetaScn>0) && (fPhiScn>0) ) {
         // scan for a good starting position
         const Double_t tstep = (thetaMax-thetaMin) / 
            static_cast<Double_t>(fThetaScn);
         const Double_t pstep = (phiMax-phiMin) / 
            static_cast<Double_t>(fPhiScn);
         Double_t tp[kNpars];
         Double_t& th = tp[kTheta];
         Double_t& ph = tp[kPhi];
         th = fThetaStr;
         ph = fPhiStr;
         Double_t minValue = GetAngleLL(tp);
         Double_t tryMinValue(minValue);
         th = thetaMin;
         for (UInt_t ti=0; ti<fThetaScn; ++ti, th+=tstep) {
            ph = phiMin;
            for (UInt_t pi=0; pi<fPhiScn; ++pi, ph+=pstep) {
               tryMinValue = GetAngleLL(tp);
               if (tryMinValue<minValue) {
                  fThetaStr = th;
                  fPhiStr   = ph;
                  minValue  = tryMinValue;
               }
            }
         }
      }

      // these will store the result of the best fit
      TArrayD  pars(2), errs(2);
      Bool_t   isValidErr(kFALSE);
      Double_t minValue(99);
      // try the specified starting parameter values first
      DoFit(fThetaStr, thetaMin, thetaMax,
            fPhiStr, phiMin, phiMax,
            pars, errs, isValidErr, minValue);
      
      if (fNrndStr>0) {
         // try other, random, starting points
         TArrayD  tryPars(2), tryErrs(2);
         Bool_t   tryIsValidErr(kFALSE);
         Double_t tryMinValue(99);
         TRandom3 rnd(static_cast<UInt_t>(fThetaStr + (10.*fPhiStr)));
         for (UInt_t i=0; i<fNrndStr; ++i) {
            DoFit( rnd.Uniform(thetaMin, thetaMax), thetaMin, thetaMax,
                   rnd.Uniform(phiMin, phiMax), phiMin, phiMax,
                   tryPars, tryErrs, tryIsValidErr, tryMinValue);
            if (tryMinValue < minValue) {
               pars = tryPars;
               errs = tryErrs;
               isValidErr = tryIsValidErr;
               minValue = tryMinValue;
            }
         }
      }      

      TString hn = GetDefMinTag();
      // this will be deleted by the selector after all modules' Process
      // has finished, since we're adding it to the event
      TSnRecoDirection* r = new TSnRecoDirection(fRecAngNm.Data(),
                                                 hn.Data(),
                                                 pars[kTheta], 
                                                 (isValidErr
                                                    ? errs[kTheta] : 0),
                                                 pars[kPhi],   
                                                 (isValidErr
                                                    ? errs[kPhi] : 0),
                                                 minValue);
      AddObjThisEvt(r);
      // also add the DTs associated with the most likely angle
      TArrayD mldts = NSnChanCorl::GetPlaneWaveOffsets(
         pars[kTheta], pars[kPhi], 
         fDts->GetChanOffsets(),
         fPosCh,
         fNgTopFern);
      TSnRecoChanOffsets* mlro = 
         new TSnRecoChanOffsets(GetRecoDTsName().Data(),
                                hn.Data(),
                                mldts.GetArray(),
                                0, // TODO: propagate errors
                                minValue);
      AddObjThisEvt(mlro);

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
               htp->SetCellContent(xi, yi, GetAngleLL(tp));
            }
         }
         htp->SetStats(0);
         htp->Draw("colz");
         TGraphErrors* gtp = new TGraphErrors(1);
         gtp->SetPoint(0, pars[kTheta]*TMath::RadToDeg(),
                       pars[kPhi]*TMath::RadToDeg());
         if (isValidErr) {
            gtp->SetPointError(0, errs[kTheta]*TMath::RadToDeg(),
                               errs[kPhi]*TMath::RadToDeg());
         }
         gtp->SetMarkerStyle(31);
         gtp->SetMarkerColor(kSpring);
         gtp->Draw("p");
         TGraphErrors* gsp = new TGraphErrors(1);
         gsp->SetPoint(0, fThetaStr*TMath::RadToDeg(), 
                       fPhiStr*TMath::RadToDeg());
         gsp->SetMarkerStyle(24);
         gsp->SetMarkerColor(kSpring);
         gsp->Draw("p");
         cdbgtp->SetLogz();
         cdbgtp->Update();
         TObject* o(0);
         while ( (o = cdbgtp->WaitPrimitive())!=0 ) {
            gSystem->ProcessEvents();
         }
         delete cdbgtp;
         delete htp;
         delete gtp;
         delete gsp;
      }
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get channel time correlations named [%s]",
                fCorlNm.Data());
   }
}

