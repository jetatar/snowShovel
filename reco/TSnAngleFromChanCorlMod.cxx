#include "TSnAngleFromChanCorlMod.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <TMath.h>
#include <TVector3.h>

#include "TSnInterp1DWvData.h"
#include "TSnRecoDirection.h"
#include "TSnRecoChanOffsets.h"
#include "NSnConstants.h"
#include "NSnChanCorl.h"
#include "TSnGeoStnSite.h"

#include <stdexcept>


TSnAngleFromChanCorlMod::TSnAngleFromChanCorlMod(const Char_t* name,
                                                 const Char_t* miniName,
                                                 const Char_t* algoName) :
   TSnFittingModule(name, "reco angle from chan correlations"),
   fChDtsNm("ChanDtOffets"),
   fGeoNm("AriannaGeo"), fCorlNm("CrossChanCorls."),
   fRecAngNm("AngleFromChanCorl."),
   fNgTopFern(1.33),
   fCsig(1.0 / (2.0 * NSnConstants::kSampRate)),
   fDts(0), fGeo(0), fCorls(0) {
   // normal ctor
}

Double_t TSnAngleFromChanCorlMod::GetAngleLL(const Double_t* pars) {

   // get the log likelihood of a plane wave from theta,phi with 
   // the channel offsets found by GetPlaneWaveOffsets
   return NSnChanCorl::GetLLfromDTsForPlaneWv(
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
   
   fGeo = dynamic_cast<const TSnGeoStnSite*>(FindPublicObj(fGeoNm.Data()));
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

}

void TSnAngleFromChanCorlMod::Process() {
   
   fCorls = dynamic_cast<const TObjArray*>(FindObjThisEvt(fCorlNm.Data()));
   if (fCorls!=0) {
   
      ROOT::Math::Minimizer& mini = GetMiniSafe();
      mini.Clear();
      ROOT::Math::Functor fcn(this, 
                              &TSnAngleFromChanCorlMod::GetAngleLL,
                              kNpars);
      mini.SetFunction(fcn);
      mini.SetLimitedVariable(kTheta, GetParName(kTheta), 3.0, 0.001,
                              TMath::Pi()/2.0, TMath::Pi());
      mini.SetLimitedVariable(kPhi, GetParName(kPhi), 5.5, 0.001,
                              0.0, TMath::TwoPi());
      mini.SetErrorDef(0.5); // because neg. log likelihood
      mini.Minimize();
      const Double_t* const pars = mini.X();
      const Double_t* const errs = mini.Errors();
   
      // this will be deleted by the selector after all modules' Process
      // has finished, since we're adding it to the event
      TString hn = GetDefMinTag();
      TSnRecoDirection* r = new TSnRecoDirection(fRecAngNm.Data(),
                                                 hn.Data(),
                                                 pars[kTheta], 
                                                 (mini.IsValidError()
                                                    ? errs[kTheta] : 0),
                                                 pars[kPhi],   
                                                 (mini.IsValidError()
                                                    ? errs[kPhi] : 0),
                                                 mini.MinValue());
      AddObjThisEvt(r);
      // also add the DTs associated with the most likely angle
      TArrayD mldts = NSnChanCorl::GetPlaneWaveOffsets(
         pars[kTheta], pars[kPhi], 
         fDts->GetChanOffsets(),
         fPosCh,
         fNgTopFern);
      TSnRecoChanOffsets* mlro = 
         new TSnRecoChanOffsets(Form("%s_dts",fRecAngNm.Data()),
                                hn.Data(),
                                mldts.GetArray(),
                                0, // TODO: propagate errors
                                mini.MinValue());
      AddObjThisEvt(mlro);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get channel time correlations named [%s]",
                fCorlNm.Data());
   }
}

