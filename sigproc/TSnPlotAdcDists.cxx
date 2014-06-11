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
   fNRawAdcBins(0), fRawAdcMin(0), fRawAdcMax(0),
   fNFpnSubBins(0), fFpnSubMin(0), fFpnSubMax(0),
   fNFracBins(0), fFracMin(0), fFracMax(0),
   fDoFpnSubPlots(kTRUE), fLowFreqCut(0.200), fNormTimeBins(kTRUE),
   fDoSmpPlots(kFALSE),
   fHdr(0), fAdc(0), fFpn(0),
   fEvtsPerBin(0), fRawAdc(0),
   fRawAdcVsT(0), fFpnSub(0),
   fFpnSubVsT(0), fFpnSubFftVsT(0),
   fFpnSubMaxFrqVsT(0),
   fFpnSubLowPwrVsT(0),
   fRawAdcSmpVsT(0), fFpnSubSmp(0),
   fFpnSubSmpVsT(0) {
   // "default" ctor (do not use)
}

TSnPlotAdcDists::TSnPlotAdcDists(const Char_t* name,
                                 const Int_t ntbins, 
                                 const Double_t tmin, const Double_t tmax,
                                 const Char_t* plottag, const Char_t* plotlbl) :
   TAModule(name, "make diagnostic ADC distributions"),
   fPlotTag(plottag), fPlotLbl(plotlbl),
   fNtimeBins(ntbins), fTimeMin(tmin), fTimeMax(tmax),
   fNRawAdcBins(2048), fRawAdcMin(-1), fRawAdcMax(4095),
   fNFpnSubBins(1201), fFpnSubMin(-1201), fFpnSubMax(1201),
   fNFracBins(100), fFracMin(0), fFracMax(1),
   fDoFpnSubPlots(kTRUE), fLowFreqCut(0.200), fNormTimeBins(kTRUE),
   fDoSmpPlots(kFALSE),
   fHdr(0), fAdc(0), fFpn(0),
   fEvtsPerBin(0),
   fRawAdc(new TObjArray(NSnConstants::kNchans)),
   fRawAdcVsT(new TObjArray(NSnConstants::kNchans)),
   fFpnSub(new TObjArray(NSnConstants::kNchans)),
   fFpnSubVsT(new TObjArray(NSnConstants::kNchans)),
   fFpnSubFftVsT(new TObjArray(NSnConstants::kNchans)),
   fFpnSubMaxFrqVsT(new TObjArray(NSnConstants::kNchans)),
   fFpnSubLowPwrVsT(new TObjArray(NSnConstants::kNchans)),
   fRawAdcSmpVsT(new TObjArray(NSnConstants::kNchans)),
   fFpnSubSmpVsT(new TObjArray(NSnConstants::kNchans)) {
   // normal ctor
   
   fRawAdc->SetName(GetRawAdc());
   fRawAdc->SetOwner(kTRUE);
   fRawAdcVsT->SetName(GetRawAdcVsT());
   fRawAdcVsT->SetOwner(kTRUE);
   fFpnSub->SetName(GetFpnSub());
   fFpnSub->SetOwner(kTRUE);
   fFpnSubVsT->SetName(GetFpnSubVsT());
   fFpnSubVsT->SetOwner(kTRUE);
   fFpnSubFftVsT->SetName(GetFpnSubFftVsT());
   fFpnSubFftVsT->SetOwner(kTRUE);
   fFpnSubMaxFrqVsT->SetName(GetFpnSubMaxFrqVsT());
   fFpnSubMaxFrqVsT->SetOwner(kTRUE);
   fFpnSubLowPwrVsT->SetName(GetFpnSubLowPwrVsT());
   fFpnSubLowPwrVsT->SetOwner(kTRUE);
   fRawAdcSmpVsT->SetName(GetRawAdcSmpVsT());
   fRawAdcSmpVsT->SetOwner(kTRUE);
   fFpnSubSmpVsT->SetName(GetFpnSubSmpVsT());
   fFpnSubSmpVsT->SetOwner(kTRUE);
   
}

TSnPlotAdcDists::~TSnPlotAdcDists() {
   delete fEvtsPerBin;
   delete fRawAdc;
   delete fRawAdcVsT;
   delete fFpnSub;
   delete fFpnSubVsT;
   delete fFpnSubFftVsT;
   delete fFpnSubMaxFrqVsT;
   delete fFpnSubLowPwrVsT;
   delete fRawAdcSmpVsT;
   delete fFpnSubSmpVsT;
}

void TSnPlotAdcDists::SlaveBegin() {
   
   TString hn;
   hn = Form("%s%s",GetEvtsPerBinName(),fPlotLbl.Data());
   fEvtsPerBin = new TH1F(hn.Data(),
                          "events per time bin;date, time (UTC)",
                          fNtimeBins, fTimeMin, fTimeMax);
   
   TH1* h;
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      
      hn = Form("%s%s_ch%d",GetRawAdc(),fPlotLbl.Data(),ch);
      h = new TH1F(hn.Data(), 
                   Form("%s raw ADC dist (chan %d);ADC",fPlotTag.Data(),ch),
                   fNRawAdcBins, fRawAdcMin, fRawAdcMax);
      fRawAdc->AddAt(h, ch);
      
      hn = Form("%s%s_ch%d",GetRawAdcVsT(),fPlotLbl.Data(),ch);
      h = new TH2F(hn.Data(), 
                   Form("%s raw ADC dist vs time (chan %d);"
                        "date, time (UTC);ADC",fPlotTag.Data(),ch),
                   fNtimeBins, fTimeMin, fTimeMax,
                   fNRawAdcBins, fRawAdcMin, fRawAdcMax);
      fRawAdcVsT->AddAt(h, ch);

      if (fDoFpnSubPlots) {
         hn = Form("%s%s_ch%d",GetFpnSub(),fPlotLbl.Data(),ch);
         h = new TH1F(hn.Data(), 
                      Form("%s FPN sub dist (chan %d);ADC-FPN",
                           fPlotTag.Data(),ch),
                      fNFpnSubBins, fFpnSubMin, fFpnSubMax);
         fFpnSub->AddAt(h, ch);
      
         hn = Form("%s%s_ch%d",GetFpnSubVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(), 
                      Form("%s FPN sub dist vs time (chan %d);"
                           "date, time (UTC);ADC-FPN",fPlotTag.Data(),ch),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fNFpnSubBins, fFpnSubMin, fFpnSubMax);
         fFpnSubVsT->AddAt(h, ch);
         

         // FFT binning
         Int_t fbins(0);
         Double_t fmin(0), fmax(0);
         TSnSpectral::GetRealFFTFreqBins(NSnConstants::kNsamps,
                                         1.0 / NSnConstants::kSampRate,
                                         fbins, fmin, fmax);
         
         hn = Form("%s%s_ch%d",GetFpnSubFftVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub FFT vs time (chan %d);"
                           "date, time (UTC);frequency (GHz)",
                           fPlotTag.Data(),ch),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fbins, fmin, fmax);
         fFpnSubFftVsT->AddAt(h, ch);
         
         hn = Form("%s%s_ch%d",GetFpnSubMaxFrqVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub max freq vs time (chan %d);"
                           "date, time (UTC);freqency (GHz)",
                           fPlotTag.Data(),ch),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fbins, fmin, fmax);
         fFpnSubMaxFrqVsT->AddAt(h, ch);
         
         hn = Form("%s%s_ch%d",GetFpnSubLowPwrVsT(),fPlotLbl.Data(),ch);
         h = new TH2F(hn.Data(),
                      Form("%s FPN sub fraction in low freq vs time, "
                           "(chan %d);date, time (UTC);fraction FFT < %2.2g",
                           fPlotTag.Data(),ch,fLowFreqCut),
                      fNtimeBins, fTimeMin, fTimeMax,
                      fNFracBins, fFracMin, fFracMax);
         fFpnSubLowPwrVsT->AddAt(h, ch);
         
      } // fpn
      
      // samples on this channel
      TObjArray* chRawAdcSmpVsT = new TObjArray(NSnConstants::kNsamps);
      chRawAdcSmpVsT->SetOwner(kTRUE);
      fRawAdcSmpVsT->AddAt(chRawAdcSmpVsT, ch);

      TObjArray* chFpnSubSmpVsT = new TObjArray(NSnConstants::kNsamps);
      chFpnSubSmpVsT->SetOwner(kTRUE);
      fFpnSubSmpVsT->AddAt(chFpnSubSmpVsT, ch);

      if (fDoSmpPlots) {
         for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
            hn = Form("%s%s_ch%d_sm%d",
                      GetRawAdcSmpVsT(),fPlotLbl.Data(),ch,sm);
            h = new TH2F(hn.Data(), 
                         Form("%s raw ADC dist vs time "
                              "(chan %d, samp %d);"
                              "date, time (UTC);ADC",
                              fPlotTag.Data(),ch,sm),
                         fNtimeBins, fTimeMin, fTimeMax,
                         fNRawAdcBins, fRawAdcMin, fRawAdcMax);
            chRawAdcSmpVsT->AddAt(h, sm);
      
            if (fDoFpnSubPlots) {
               hn = Form("%s%s_ch%d_sm%d",
                         GetFpnSubSmpVsT(),fPlotLbl.Data(),ch,sm);
               h = new TH2F(hn.Data(), 
                            Form("%s FPN sub dist vs time "
                                 "(chan %d, samp %d);"
                                 "date, time (UTC);ADC-FPN",
                                 fPlotTag.Data(),ch,sm),
                            fNtimeBins, fTimeMin, fTimeMax,
                            fNFpnSubBins, fFpnSubMin, fFpnSubMax);
               chFpnSubSmpVsT->AddAt(h, sm);
            } // fpn
         
         } // sm
      }
      
   } // ch
   
   
   // add to output
   
   AddOutput(fEvtsPerBin);

   AddOutput(fRawAdc);
   AddOutput(fRawAdcVsT);
   if (fDoFpnSubPlots) {
      AddOutput(fFpnSub);
      AddOutput(fFpnSubVsT);
      AddOutput(fFpnSubFftVsT);
      AddOutput(fFpnSubMaxFrqVsT);
      AddOutput(fFpnSubLowPwrVsT);
   }
   
   if (fDoSmpPlots) {
      AddOutput(fRawAdcSmpVsT);
      if (fDoFpnSubPlots) {
         AddOutput(fFpnSubSmpVsT);
      }
   }
   
   // request branches
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
   ReqBranch(TSnRawTreeMaker::kEWvBrNm, fAdc);
   if (fDoFpnSubPlots) {
      ReqBranch(TSnSaveCalibDataMod::kFPNSubDatBrNm, fFpn);
   }
   
}

void TSnPlotAdcDists::Process() {
   LoadBranch(TSnRawTreeMaker::kEHdBrNm);
   if (fHdr==0) {
      return SendError(kAbortModule, "Process", "No event header.");
   }

   LoadBranch(TSnRawTreeMaker::kEWvBrNm);
   if (fAdc==0) {
      return SendError(kAbortModule, "Process", "No raw data.");
   }

   if (fDoFpnSubPlots) {
      LoadBranch(TSnSaveCalibDataMod::kFPNSubDatBrNm);
      if (fFpn==0) {
         return SendError(kAbortModule, "Process", "No FPN sub data.");
      }
   }
   
   const UInt_t time = static_cast<UInt_t>(fHdr->GetUnixTime());
   
   fEvtsPerBin->Fill(time);
   
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      const UShort_t* adc = static_cast<const TSnRawWaveform*>(fAdc)
         ->GetWvData(ch);
      const Float_t*  fpn(0);
      if (fDoFpnSubPlots) {
         fpn = fFpn->GetData(ch);
      }
      
      TH1F* hRawAdc = dynamic_cast<TH1F*>(fRawAdc->At(ch));
      TH2F* hRawAdcVsT = dynamic_cast<TH2F*>(fRawAdcVsT->At(ch));
      TObjArray* chRawAdcSmpVsT(0);
      if (fDoSmpPlots) {
         chRawAdcSmpVsT = dynamic_cast<TObjArray*>(fRawAdcSmpVsT->At(ch));
      }

      TH1F* hFpnSub(0);
      TH2F* hFpnSubVsT(0);
      TObjArray* chFpnSubSmpVsT(0);
      TH2F* hFpnSubFftVsT(0);
      TH2F* hFpnSubMaxFrqVsT(0);
      TH2F* hFpnSubLowPwrVsT(0);
      if (fDoFpnSubPlots) {
         hFpnSub = dynamic_cast<TH1F*>(fFpnSub->At(ch));
         hFpnSubVsT = dynamic_cast<TH2F*>(fFpnSubVsT->At(ch));
         if (fDoSmpPlots) {
            chFpnSubSmpVsT = dynamic_cast<TObjArray*>(fFpnSubSmpVsT->At(ch));
         }
         hFpnSubFftVsT = dynamic_cast<TH2F*>(fFpnSubFftVsT->At(ch));
         hFpnSubMaxFrqVsT = dynamic_cast<TH2F*>(fFpnSubMaxFrqVsT->At(ch));
         hFpnSubLowPwrVsT = dynamic_cast<TH2F*>(fFpnSubLowPwrVsT->At(ch));
         
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
            hFpnSubFftVsT->Fill(time, *frq, *amp);
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
         hFpnSubLowPwrVsT->Fill(time, lowr);
         hFpnSubMaxFrqVsT->Fill(time, maxfrq);
         
         delete g;
      }
      
      for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++adc, ++fpn) {
         hRawAdc->Fill(*adc);
         hRawAdcVsT->Fill(time, *adc);
         if (fDoSmpPlots) {
            TH2F* hRawAdcSmpVsT = dynamic_cast<TH2F*>(chRawAdcSmpVsT->At(sm));
            hRawAdcSmpVsT->Fill(time, *adc);
         }
         
         if (fDoFpnSubPlots) {
            hFpnSub->Fill(*fpn);
            hFpnSubVsT->Fill(time, *fpn);
            if (fDoSmpPlots) {
               TH2F* hFpnSubSmpVsT = 
                  dynamic_cast<TH2F*>(chFpnSubSmpVsT->At(sm));
               hFpnSubSmpVsT->Fill(time, *fpn);
            }
         }
      }
      
   }
   
}

void TSnPlotAdcDists::Terminate() {
   
   if (fNormTimeBins) {
      
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         NormalizeTimeColumns( *dynamic_cast<TH2*>(fRawAdcVsT->At(ch)) );
         
         if (fDoFpnSubPlots) {
            NormalizeTimeColumns( *dynamic_cast<TH2*>(fFpnSubVsT->At(ch)) );
            NormalizeTimeColumns( *dynamic_cast<TH2*>(fFpnSubFftVsT->At(ch)) );
            NormalizeTimeColumns( 
               *dynamic_cast<TH2*>(fFpnSubMaxFrqVsT->At(ch)) );
            NormalizeTimeColumns( 
               *dynamic_cast<TH2*>(fFpnSubLowPwrVsT->At(ch)) );
         }
         
         if (fDoSmpPlots) {
            for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
               NormalizeTimeColumns( *dynamic_cast<TH2*>(
                                        fRawAdcSmpVsT->At(ch)) );
               if (fDoFpnSubPlots) {
                  NormalizeTimeColumns( *dynamic_cast<TH2*>(
                                           fFpnSubSmpVsT->At(ch)) );
               }
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
