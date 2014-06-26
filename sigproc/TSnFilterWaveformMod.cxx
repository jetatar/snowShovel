#include "TSnFilterWaveformMod.h"

#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TSystem.h>

#include "TSnCalWvData.h"
#include "TSnSpectral.h"
#include "TSnPlotBrowser.h"
#include "NSnConstants.h"

ClassImp(TSnFilterWaveformMod);

const UInt_t TSnFilterWaveformMod::kDebugFilter = 100;

TSnFilterWaveformMod::TSnFilterWaveformMod(const Char_t* name,
                                           const Char_t* datBrNm,
                                           const Char_t* fltDtNm,
                                           const TF1& filter) :
   TSnCalDatModule(name, "filter waveforms module"),
   fDatBrNm(datBrNm),
   fFltDatNm(fltDtNm),
   fFilter(new TF1(filter)),
   fDat(0) {
   // normal ctor
}

TSnFilterWaveformMod::~TSnFilterWaveformMod() {
   delete fFilter;
}

void TSnFilterWaveformMod::SlaveBegin() {
   ReqCalData(fDatBrNm.Data(), fDat);
}

void TSnFilterWaveformMod::Process() {
   if (fFilter!=0) {
      LoadCalData(fDatBrNm.Data());
      if (fDat!=0) {
         
         TSnCalWvData* fltd = new TSnCalWvData(fFltDatNm.Data(),
                                               Form("filtered [%s] with [%s]",
                                                    fDatBrNm.Data(),
                                                    fFilter->GetName()));
         fltd->CopyDataFrom(*fDat);

         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            // get the FFT
            Float_t* const cfft = fltd->GetData(ch);
            TSnSpectral::RealFFT(cfft, NSnConstants::kNsamps, kFALSE);
            
            // apply the filter
            const Float_t fi = NSnConstants::kSampRate /
               static_cast<Float_t>(NSnConstants::kNsamps);
            // freq 0
            *cfft *= fFilter->Eval(0.0);
            // mid freqs
            Float_t* fe = cfft+2;
            Float_t* fo = fe+1;
            Float_t  fr = fi;
            Double_t f(0);
            for (UChar_t i=2; i<NSnConstants::kNsamps; 
                 i+=2, fe+=2, fo+=2, fr+=fi) {
               f = fFilter->Eval(fr);
               *fe *= f;
               *fo *= f;
            }
            // max freq
            cfft[1] *= fFilter->Eval(fr);
            
            // undo the FFT
            TSnSpectral::RealFFT(cfft, NSnConstants::kNsamps, kTRUE);
         }
         
         AddObjThisEvt(fltd);
         
         if (GetVerbosity()>=kDebugFilter) {
            // for debugging: draw the filtered
            Info("Process", "Drawing filter debug stuff. "
                 "Double-click canvas to show next event.");
            TObjArray graphs; // to delete our graphs for us at the end
            graphs.SetOwner(kTRUE);
            TCanvas* c = new TCanvas("cfilt", "cfilt - filter debug",
                                     1200,1000);
            TSnPlotBrowser* pb = new TSnPlotBrowser("filtpb");
            pb->SetCanvas(c);
            for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               // get the FFT
               TGraph* gwv = fDat->NewGraphForChan(ch, kTRUE);
               TGraph* gfl = fltd->NewGraphForChan(ch, kTRUE);
               graphs.Add(gwv);
               graphs.Add(gfl);
               gwv->SetLineColor(kAzure-6);
               gwv->SetMarkerColor(kAzure-6);
               gwv->SetMarkerStyle(7);
               gfl->SetLineColor(kOrange+7);
               gfl->SetMarkerColor(kOrange+7);
               gfl->SetMarkerStyle(7);
               pb->AddPlotToSet("Waveforms", gwv, "apl", ch+1);
               pb->AddPlotToSet("Waveforms", gfl,  "pl", ch+1);
               
               Float_t* const fftwv = new Float_t[NSnConstants::kNsamps];
               Float_t* const fftfl = new Float_t[NSnConstants::kNsamps];
               memcpy(fftwv, fDat->GetData(ch),
                      NSnConstants::kNsamps*sizeof(Float_t));
               memcpy(fftfl, fltd->GetData(ch),
                      NSnConstants::kNsamps*sizeof(Float_t));
               TGraph* gfwv = 
                  TSnSpectral::NewRealFFTGraph(fftwv, NSnConstants::kNsamps,
                          static_cast<Float_t>(1.0 / NSnConstants::kSampRate));
               TGraph* gffl =
                  TSnSpectral::NewRealFFTGraph(fftfl, NSnConstants::kNsamps,
                          static_cast<Float_t>(1.0 / NSnConstants::kSampRate));
               graphs.Add(gfwv);
               graphs.Add(gffl);
               gfwv->SetLineColor(kAzure-6);
               gfwv->SetMarkerColor(kAzure-6);
               gfwv->SetMarkerStyle(7);
               gffl->SetLineColor(kOrange+7);
               gffl->SetMarkerColor(kOrange+7);
               gffl->SetMarkerStyle(7);
               pb->AddPlotToSet("FFTs", gfwv, "apl", ch+1);
               pb->AddPlotToSet("FFTs", gffl,  "pl", ch+1);
               
               delete[] fftwv;
               delete[] fftfl;
            }
            
            pb->AddPlotToSet("Filter", fFilter, 0, 0,
               TSnPlotBrowser::kDefaults | TSnPlotBrowser::kGridx
               | TSnPlotBrowser::kGridy);
            pb->DrawButtonBar();
            pb->DrawPlots("Waveforms");
            
            TObject* o(0);
            while ( (o = c->WaitPrimitive())!=0 ) {
               gSystem->ProcessEvents();
            }
            delete c;
            delete pb;
         }
         
      } else {
         SendError(kAbortModule, "Process",
                   "Could not load data [%s]",
                   fDatBrNm.Data());
      }
   }
}

void TSnFilterWaveformMod::SetFilter(const TF1& f) {
   // copy the filter
   delete fFilter;
   fFilter = static_cast<TF1*>(f.Clone());
}

