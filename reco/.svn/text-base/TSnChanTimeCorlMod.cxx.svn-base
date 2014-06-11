#include "TSnChanTimeCorlMod.h"

#include <TObjArray.h>
#include <TGraph.h>

#include "TSnSpectral.h"
#include "TSnRecoChanOffsets.h"
#include "TSnCalWvData.h"
#include "TSnInterp1DSpline3.h"

ClassImp(TSnChanTimeCorlMod);

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

      TString hn;
      TGraph* g;
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (Int_t xc=0; xc<ch; ++xc) {
            g = TSnSpectral::NewCorrelateGraph( fDat->GetData(ch),
                                                fDat->GetData(xc),
                                                NSnConstants::kNsamps,
                                                kTRUE );
            // put in units of time
            Double_t* x = g->GetX();
            for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++x) {
               *x /= NSnConstants::kSampRate;
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
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not get data [%s]",
                fWvDatNm.Data());
   }   
}

