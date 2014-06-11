#include "TSnFindWaveformEnvelopeMod.h"

#include <TGraph.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TSystem.h>

#include "TSnSpectral.h"
#include "TSnPlotBrowser.h"
#include "NSnConstants.h"
#include "TSnCalWvData.h"

ClassImp(TSnFindWaveformEnvelopeMod);

const UInt_t TSnFindWaveformEnvelopeMod::kDebugEnv = 100;

void TSnFindWaveformEnvelopeMod::SlaveBegin() {
   ReqCalData(fDatBrNm.Data(), fDat);
}

void TSnFindWaveformEnvelopeMod::Process() {
   LoadCalData(fDatBrNm.Data());
   if (fDat!=0) {
      
      TSnCalWvData* env = new TSnCalWvData(fEnvDatNm.Data(),
                                           Form("envelope of [%s]",
                                                fDatBrNm.Data()));
      env->CopyDataFrom(*fDat);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         TSnSpectral::EnvelopeReal(env->GetData(ch),
                                   NSnConstants::kNsamps);
      }
      AddObjThisEvt(env);

      if (GetVerbosity()>=kDebugEnv) {
         Info("Process","Drawing envelope debug stuff. "
              "Double-click canvas to show next event.");
         TObjArray graphs; // to delete the graphs at the end
         graphs.SetOwner(kTRUE);
         TCanvas* c1 = new TCanvas(Form("%s_c1",GetName()),"envelope",600,500);
         UInt_t c(1),r(1);
         TSnPlotBrowser::CalcCanvDivs(c,r,NSnConstants::kNchans);
         c1->Divide(c,r);
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            c1->cd(ch+1);
            TGraph* gwv = fDat->NewGraphForChan(ch, kTRUE);
            TGraph* gen = env->NewGraphForChan(ch, kTRUE);
            graphs.Add(gwv);
            graphs.Add(gen);
            gwv->SetLineColor(kBlack);
            gwv->SetLineWidth(1);
            gwv->SetMarkerColor(kBlack);
            gwv->SetMarkerStyle(7);
            gen->SetLineColor(kOrange+7);
            gen->SetMarkerColor(kOrange+7);
            gen->SetMarkerStyle(7);
            gwv->Draw("apl");
            gen->Draw("pl");
         }
         TObject* o(0);
         while ( (o = c1->WaitPrimitive())!=0 ) {
            gSystem->ProcessEvents();
         }
         delete c1;
      }
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not load data [%s]", fDatBrNm.Data());
   }
}
