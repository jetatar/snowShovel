#include "TSnPlotAdcDists.h"

#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

#include "NSnConstants.h"
#include "TSnRawTreeMaker.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnEventHeader.h"
#include "TSnRawWaveform.h"
#include "TSnCalWvData.h"
#include "TSnSpectral.h"

ClassImp(TSnPlotAdcDists);

TSnPlotAdcDists::TSnPlotAdcDists() :
   fNtimeBins(0), fTimeMin(0), fTimeMax(0),
   fNAdcBins(2048), fAdcMin(-1), fAdcMax(4095),
   fNFracBins(100), fFracMin(0), fFracMax(1),
   fDoFFTPlots(kTRUE), fLowFreqCut(0.200), fNormTimeBins(kTRUE),
   fDoSmpPlots(kFALSE), fIsRaw(kTRUE),
   fHdr(0), fAdc(0), fFpn(0),
   fEvtsPerBin(0), fhAdc(0), fhAdcVsT(0),
   fhFftVsT(0), fhMaxFrqVsT(0), fhLowPwrVsT(0),
   fhSmpVsT(0),
   fhAmpl(0), fhAmplVsT(0),
   fhMean(0), fhMeanVsT(0),
   fhRms(0), fhRmsVsT(0) {
   // "default" ctor (do not use)
}

TSnPlotAdcDists::TSnPlotAdcDists(const Char_t* name,
                                 const Int_t ntbins, 
                                 const Double_t tmin, const Double_t tmax,
                                 const Char_t* plottag, const Char_t* plotlbl,
                                 const Char_t* dataBranchName) :
   TAModule(name, "make diagnostic ADC distributions"),
   fPlotTag(plottag), fPlotLbl(plotlbl),
   fDataBrNm(dataBranchName),
   fNtimeBins(ntbins), fTimeMin(tmin), fTimeMax(tmax),
   fNAdcBins(2048), fAdcMin(-1), fAdcMax(4095),
   fNFracBins(100), fFracMin(0), fFracMax(1),
   fDoFFTPlots(kTRUE), fLowFreqCut(0.200), fNormTimeBins(kTRUE),
   fDoSmpPlots(kFALSE), fIsRaw(kTRUE),
   fHdr(0), fAdc(0), fFpn(0),
   fEvtsPerBin(0),
   fhAdc(new TObjArray(NSnConstants::kNchans)),
   fhAdcVsT(new TObjArray(NSnConstants::kNchans)),
   fhFftVsT(new TObjArray(NSnConstants::kNchans)),
   fhMaxFrqVsT(new TObjArray(NSnConstants::kNchans)),
   fhLowPwrVsT(new TObjArray(NSnConstants::kNchans)),
   fhSmpVsT(new TObjArray(NSnConstants::kNchans)),
   fhAmpl(new TObjArray(NSnConstants::kNchans)),
   fhAmplVsT(new TObjArray(NSnConstants::kNchans)),
   fhMean(new TObjArray(NSnConstants::kNchans)),
   fhMeanVsT(new TObjArray(NSnConstants::kNchans)),
   fhRms(new TObjArray(NSnConstants::kNchans)),
   fhRmsVsT(new TObjArray(NSnConstants::kNchans)) {
   // normal ctor
   
   fhAdc->SetName(GetAdc());
   fhAdc->SetOwner(kTRUE);
   fhAdcVsT->SetName(GetAdcVsT());
   fhAdcVsT->SetOwner(kTRUE);
   fhFftVsT->SetName(GetFftVsT());
   fhFftVsT->SetOwner(kTRUE);
   fhMaxFrqVsT->SetName(GetMaxFrqVsT());
   fhMaxFrqVsT->SetOwner(kTRUE);
   fhLowPwrVsT->SetName(GetLowPwrVsT());
   fhLowPwrVsT->SetOwner(kTRUE);
   fhSmpVsT->SetName(GetSmpVsT());
   fhSmpVsT->SetOwner(kTRUE);
   fhAmpl->SetName(GetAmpl());
   fhAmpl->SetOwner(kTRUE);
   fhMean->SetName(GetMean());
   fhMean->SetOwner(kTRUE);
   fhRms->SetName(GetRms());
   fhRms->SetOwner(kTRUE);
   fhAmplVsT->SetName(GetAmplVsT());
   fhAmplVsT->SetOwner(kTRUE);
   fhMeanVsT->SetName(GetMeanVsT());
   fhMeanVsT->SetOwner(kTRUE);
   fhRmsVsT->SetName(GetRmsVsT());
   fhRmsVsT->SetOwner(kTRUE);
   
   SetDataBrName( (dataBranchName==NULL) ?
                  TSnRawTreeMaker::kEWvBrNm : dataBranchName );
   
}

TSnPlotAdcDists::~TSnPlotAdcDists() {
   delete fEvtsPerBin;
   delete fhAdc;
   delete fhAdcVsT;
   delete fhFftVsT;
   delete fhMaxFrqVsT;
   delete fhLowPwrVsT;
   delete fhSmpVsT;
   delete fhAmpl;
   delete fhMean;
   delete fhRms;
}

void TSnPlotAdcDists::SetDataBrName(const Char_t* n) {
   // set the data branch name
   // if the name is TSnRawTreeMaker::kEWvBrNm, assume raw data
   // otherwise, assume cal data
   fDataBrNm = n;
   fIsRaw = (fDataBrNm.CompareTo(TSnRawTreeMaker::kEWvBrNm)==0);
}

void TSnPlotAdcDists::SlaveBegin() {
   
   TString hn;
   hn = Form("%s%s",GetEvtsPerBinName(),fPlotLbl.Data());
   fEvtsPerBin = new TH1F(hn.Data(),
                          "events per time bin;date, time (UTC)",
                          fNtimeBins, fTimeMin, fTimeMax);
   
   TH1* h;
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      
      hn = Form("%s%s_ch%d",GetAdc(),fPlotLbl.Data(),ch);
      h = new TH1F(hn.Data(), 
                   Form("%s raw ADC dist (chan %d);ADC",fPlotTag.Data(),ch),
                   fNAdcBins, fAdcMin, fAdcMax);
      fhAdc->AddAt(h, ch);
      
      hn = Form("%s%s_ch%d",GetAdcVsT(),fPlotLbl.Data(),ch);
      h = new TH2F(hn.Data(), 
                   Form("%s raw ADC dist vs time (chan %d);"
                        "date, time (UTC);ADC",fPlotTag.Data(),ch),
                   fNtimeBins, fTimeMin, fTimeMax,
                   fNAdcBins, fAdcMin, fAdcMax);
      fhAdcVsT->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetAmpl(),fPlotLbl.Data(),ch);
      h = new TH1F(hn.Data(),
                   Form("%s abs amplitude (chan %d);abs(amplitude)",
                        fPlotTag.Data(),ch),
                   fNAdcBins, fAdcMin, fAdcMax);
      fhAmpl->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetAmplVsT(),fPlotLbl.Data(),ch);
      h = new TH2F(hn.Data(), 
                   Form("%s abs amplitdue vs time (chan %d);"
                        "date, time (UTC);abs(amplitude)",fPlotTag.Data(),ch),
                   fNtimeBins, fTimeMin, fTimeMax,
                   fNAdcBins, fAdcMin, fAdcMax);
      fhAmplVsT->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetMean(),fPlotLbl.Data(),ch);
      h = new TH1F(hn.Data(),
                   Form("%s baseline (chan %d);abs(meanitude)",
                        fPlotTag.Data(),ch),
                   fNAdcBins, fAdcMin, fAdcMax);
      fhMean->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetMeanVsT(),fPlotLbl.Data(),ch);
      h = new TH2F(hn.Data(), 
                   Form("%s baseline vs time (chan %d);"
                        "date, time (UTC);abs(meanitude)",fPlotTag.Data(),ch),
                   fNtimeBins, fTimeMin, fTimeMax,
                   fNAdcBins, fAdcMin, fAdcMax);
      fhMeanVsT->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetRms(),fPlotLbl.Data(),ch);
      h = new TH1F(hn.Data(),
                   Form("%s RMS (chan %d);abs(rmsitude)",
                        fPlotTag.Data(),ch),
                   fNAdcBins, fAdcMin, fAdcMax);
      fhRms->AddAt(h, ch);

      hn = Form("%s%s_ch%d",GetRmsVsT(),fPlotLbl.Data(),ch);
      h = new TH2F(hn.Data(), 
                   Form("%s RMS vs time (chan %d);"
                        "date, time (UTC);abs(rmsitude)",fPlotTag.Data(),ch),
                   fNtimeBins, fTimeMin, fTimeMax,
                   fNAdcBins, fAdcMin, fAdcMax);
      fhRmsVsT->AddAt(h, ch);
      

      if (CanMakeFFTPlots()) {
         // FFT binning
         Int_t fbins(0);
         Double_t fmin(0), fmax(0);
         TSnSpectral::GetRealFFTFreqBins(NSnConstants::kNsamps,
                                         1.0 / NSnConstants::kSampRate,
                                         fbins, fmin, fmax);
         
         hn = Form("%s%s_ch%d",GetFftVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub FFT vs time (chan %d);"
                           "date, time (UTC);frequency (GHz)",
                           fPlotTag.Data(),ch),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fbins, fmin, fmax);
         fhFftVsT->AddAt(h, ch);
         
         hn = Form("%s%s_ch%d",GetMaxFrqVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub max freq vs time (chan %d);"
                           "date, time (UTC);freqency (GHz)",
                           fPlotTag.Data(),ch),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fbins, fmin, fmax);
         fhMaxFrqVsT->AddAt(h, ch);
         
         hn = Form("%s%s_ch%d",GetLowPwrVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub fraction in low freq vs time, "
                           "(chan %d);date, time (UTC);fraction FFT < %2.2g",
                           fPlotTag.Data(),ch,fLowFreqCut),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fNFracBins, fFracMin, fFracMax);
         fhLowPwrVsT->AddAt(h, ch);
         
      } // fft
      
      // samples on this channel
      TObjArray* chSmpVsT = new TObjArray(NSnConstants::kNsamps);
      chSmpVsT->SetOwner(kTRUE);
      fhSmpVsT->AddAt(chSmpVsT, ch);
      
      if (fDoSmpPlots) {
         for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
            hn = Form("%s%s_ch%d_sm%d",
                      GetSmpVsT(),fPlotLbl.Data(),ch,sm);
            h = new TH2F(hn.Data(), 
                         Form("%s raw ADC dist vs time "
                              "(chan %d, samp %d);"
                              "date, time (UTC);ADC",
                              fPlotTag.Data(),ch,sm),
                         fNtimeBins, fTimeMin, fTimeMax,
                         fNAdcBins, fAdcMin, fAdcMax);
            chSmpVsT->AddAt(h, sm);
      
         } // sm
      }
      
   } // ch
   
   
   // add to output
   
   AddOutput(fEvtsPerBin);

   AddOutput(fhAdc);
   AddOutput(fhAdcVsT);
   AddOutput(fhAmpl);
   AddOutput(fhAmplVsT);
   AddOutput(fhMean);
   AddOutput(fhMeanVsT);
   AddOutput(fhRms);
   AddOutput(fhRmsVsT);
   
   if (CanMakeFFTPlots()) {
      AddOutput(fhFftVsT);
      AddOutput(fhMaxFrqVsT);
      AddOutput(fhLowPwrVsT);
   }
   
   if (fDoSmpPlots) {
      AddOutput(fhSmpVsT);
   }
   
   // request branches
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   if (fIsRaw) {
      ReqBranch(fDataBrNm.Data(), fAdc);
   } else {
      ReqBranch(fDataBrNm.Data(), fFpn);
   }
   
}

void TSnPlotAdcDists::Process() {

   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (fHdr==0) {
      return SendError(kAbortModule, "Process", "No event header.");
   }
   
   LoadBranch(fDataBrNm.Data());
   if (fIsRaw) {
      if (fAdc==0) {
         return SendError(kAbortModule, "Process", "No raw data [%s].",
                          fDataBrNm.Data());
      }
   } else {
      if (fFpn==0) {
         return SendError(kAbortModule, "Process", "No cal data [%s].",
                          fDataBrNm.Data());
      }
   }
   
   const UInt_t time = static_cast<UInt_t>(fHdr->GetUnixTime());
   
   fEvtsPerBin->Fill(time);
   
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      const UShort_t* adc(0);
      const Float_t*  fpn(0);
      if (fIsRaw) {
         adc = static_cast<const TSnRawWaveform*>(fAdc)->GetWvData(ch);
      } else {
         fpn = fFpn->GetData(ch);
      }
      
      TH1F* hAdc = dynamic_cast<TH1F*>(fhAdc->At(ch));
      TH2F* hAdcVsT = dynamic_cast<TH2F*>(fhAdcVsT->At(ch));
      TH1F* hAmpl = dynamic_cast<TH1F*>(fhAmpl->At(ch));
      TH2F* hAmplVsT = dynamic_cast<TH2F*>(fhAmplVsT->At(ch));
      TH1F* hMean = dynamic_cast<TH1F*>(fhMean->At(ch));
      TH2F* hMeanVsT = dynamic_cast<TH2F*>(fhMeanVsT->At(ch));
      TH1F* hRms = dynamic_cast<TH1F*>(fhRms->At(ch));
      TH2F* hRmsVsT = dynamic_cast<TH2F*>(fhRmsVsT->At(ch));
      TObjArray* chSmpVsT(0);
      if (fDoSmpPlots) {
         chSmpVsT = dynamic_cast<TObjArray*>(fhSmpVsT->At(ch));
      }

      if (CanMakeFFTPlots()) {
         TH2F* hFftVsT(0);
         TH2F* hMaxFrqVsT(0);
         TH2F* hLowPwrVsT(0);
         hFftVsT = dynamic_cast<TH2F*>(fhFftVsT->At(ch));
         hMaxFrqVsT = dynamic_cast<TH2F*>(fhMaxFrqVsT->At(ch));
         hLowPwrVsT = dynamic_cast<TH2F*>(fhLowPwrVsT->At(ch));
         
         // get the FFT
         TGraph* g = TSnSpectral::NewRealFFTGraph(fpn, NSnConstants::kNsamps,
                          static_cast<Float_t>(1.0 / NSnConstants::kSampRate));
         Double_t* frq = g->GetX(), * amp = g->GetY();
         const Int_t fbins = g->GetN();
         
         // find the bin with fLowFreqCut
         Int_t lowb(0);
         for (Int_t i=0; i<fbins; ++i, ++frq, ++amp) {
            if (*frq > fLowFreqCut) {
               lowb = i;
               break;
            }
         }
         // fill hists
         Double_t tlow(0);
         Double_t tamp(0);
         frq = g->GetX(); amp = g->GetY();
         Double_t maxamp = *amp;
         Double_t maxfrq = *frq;
         for (Int_t i=0; i<fbins; ++i, ++frq, ++amp) {
            hFftVsT->Fill(time, *frq, *amp);
            if (*amp > maxamp) {
               maxamp = *amp;
               maxfrq = *frq;
            }
            if (i<lowb) {
               tlow += *amp;
            }
            tamp += *amp;
         }
         Double_t lowr(0);
         if (tamp>0) {
            lowr = tlow / tamp;
         }
         hLowPwrVsT->Fill(time, lowr);
         hMaxFrqVsT->Fill(time, maxfrq);
         
         delete g;
      }

      if (fIsRaw) {
         UShort_t rawave(0), rawrms(0);
         TSnMath::GetAveRMSFast(adc, NSnConstants::kNsamps, rawave, rawrms);
         hMean->Fill(rawave);
         hMeanVsT->Fill(time, rawave);
         hRms->Fill(rawrms);
         hRmsVsT->Fill(time, rawrms);
      } else {
         Float_t calave(0), calrms(0);
         TSnMath::GetAveRMSFast(fpn, NSnConstants::kNsamps, calave, calrms);
         hMean->Fill(calave);
         hMeanVsT->Fill(time, calave);
         hRms->Fill(calrms);
         hRmsVsT->Fill(time, calrms);
     }
      
      Float_t d(0);
      Float_t ampl = TMath::Abs( (fIsRaw) ? *adc : *fpn );
      for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++adc, ++fpn) {
         d = (fIsRaw) ? static_cast<Float_t>(*adc) : *fpn;
         hAdc->Fill(d);
         hAdcVsT->Fill(time, d);
         if (fDoSmpPlots) {
            TH2F* hSmpVsT = dynamic_cast<TH2F*>(chSmpVsT->At(sm));
            hSmpVsT->Fill(time, d);
         }
         if (TMath::Abs(d)>ampl) {
            ampl = TMath::Abs(d);
         }
      }
      hAmpl->Fill(ampl);
      hAmplVsT->Fill(time, ampl);
      
   }
   
}

void TSnPlotAdcDists::Terminate() {
   
   if (fNormTimeBins) {
      
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         NormalizeTimeColumns( *dynamic_cast<TH2*>(fhAdcVsT->At(ch)) );
         NormalizeTimeColumns( *dynamic_cast<TH2*>(fhAmplVsT->At(ch)) );
         NormalizeTimeColumns( *dynamic_cast<TH2*>(fhMeanVsT->At(ch)) );
         NormalizeTimeColumns( *dynamic_cast<TH2*>(fhRmsVsT->At(ch)) );
         
         if (CanMakeFFTPlots()) {
            NormalizeTimeColumns( *dynamic_cast<TH2*>(fhFftVsT->At(ch)) );
            NormalizeTimeColumns( *dynamic_cast<TH2*>(fhMaxFrqVsT->At(ch)) );
            NormalizeTimeColumns( *dynamic_cast<TH2*>(fhLowPwrVsT->At(ch)) );
         }
         
         if (fDoSmpPlots) {
            for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
               NormalizeTimeColumns( *dynamic_cast<TH2*>(fhSmpVsT->At(ch)) );
            }
         }
            
      }
   }
   
}

void TSnPlotAdcDists::NormalizeTimeColumns(TH2& h) {
   for (Int_t x=fEvtsPerBin->GetNbinsX(); x>0; --x) {
      const Double_t n = fEvtsPerBin->GetBinContent(x);
      for (Int_t y=h.GetNbinsY(); y>0; --y) {
         if (n>0) {
            const Double_t c = h.GetCellContent(x,y);
            h.SetCellContent(x,y,c/n);
         } else {
            h.SetCellContent(x,y,0);
         }
      }
   }
}
