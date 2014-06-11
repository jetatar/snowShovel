#include "TSnFitChanOffsetMod.h"

#include <Math/Minimizer.h>
#include <Math/Functor.h>
#include <TObjArray.h>
#include <TMath.h>

#include "TSnInterp1DWvData.h"
#include "TSnRecoChanOffsets.h"
#include "NSnChanCorl.h"
#include "NSnConstants.h"

ClassImp(TSnFitChanOffsetMod);

Double_t TSnFitChanOffsetMod::GetChanDelayLL(const Double_t* dts) const {
   TArrayD d(TSnRecoChanOffsets::kChCmbs, dts);
   return NSnChanCorl::GetLLfromDTsForPlaneWv(d, *fCorl, fCsig);
}

void TSnFitChanOffsetMod::Process() {
   // fit the channel offsets given some correlation functions between
   // the channels.
   //
   // the offsets are constrained such that the appropriate combinations
   // of offsets should add up to other offets. for example,
   // dt_(ch0-1) + dt_(ch2-0) =~ dt_(ch2-1)
   
   fCorl = dynamic_cast<const TObjArray*>(FindObjThisEvt(fCorlNm.Data()));
   if (fCorl!=0) {
      
      // this will be deleted by the selector after all modules' Process
      // has finished, since we're adding it to the event
      TSnRecoChanOffsets* off = new TSnRecoChanOffsets(fDtsNm.Data(),
                                                       GetDefMinTag());
      
      ROOT::Math::Minimizer& mini = GetMiniSafe();
      mini.Clear();
      ROOT::Math::Functor fcn(this,
                              &TSnFitChanOffsetMod::GetChanDelayLL,
                              TSnRecoChanOffsets::kChCmbs);
      mini.SetFunction(fcn);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (UChar_t xc=0; xc<ch; ++xc) {
            const UInt_t di = TSnRecoChanOffsets::IndexFor(ch,xc);
            const TSnInterp1DWvData* cl = 
               dynamic_cast<const TSnInterp1DWvData*>(fCorl->At(di));
            if (cl!=0) {
               if ( GetMinimizerName().CompareTo("Genetic")==0 ) {
                  mini.SetLimitedVariable(di, GetParName(ch,xc).Data(),
                                          cl->FindXAtMaximum(), 0.01,
                                          -NSnChanCorl::kMaxDt,
                                          NSnChanCorl::kMaxDt);
               } else {
                  mini.SetVariable(di, GetParName(ch,xc).Data(),
                                   cl->FindXAtMaximum(), 0.01);
               }
            } else {
               SendError(kAbortAnalysis, "Process",
                         "Could not get correlation function for "
                         "(ch=%hhu, xc=%hhu)", ch, xc);
            }
         }
      }
      mini.SetErrorDef(0.5); // because neg. log likelihood
      
      mini.Minimize();
      const Double_t* par = mini.X();
      const Double_t* err = mini.Errors();
      for (UInt_t i=0; i<TSnRecoChanOffsets::kChCmbs; ++i, ++par, ++err) {
         off->SetChanOffset(     i, *par);
         if (mini.IsValidError()) {
            off->SetChanOffsetError(i, *err);
         }
      }
      off->SetFitQual(mini.MinValue());
      
      AddObjThisEvt(off);

   } else {
      SendError(kAbortModule, "Process",
                "Could not get channel correlation functions "
                "named [%s].", fCorlNm.Data());
   }
   
}
