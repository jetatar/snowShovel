#include "TSnPlotWaveformsMod.h"

#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TObjArray.h>
#include <TString.h>

#include "NSnConstants.h"
#include "TSnRawTreeMaker.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnEventHeader.h"
#include "TSnRawWaveform.h"
#include "TSnCalWvData.h"
#include "TSnSpectral.h"

ClassImp(TSnPlotWaveformsMod);

TSnPlotWaveformsMod::TSnPlotWaveformsMod() :
   fNevtBins(0), fEvtMin(0), fEvtMax(0),
   fDoAdcPlots(kTRUE), fDoFpnSubPlots(kTRUE), fDoAmpOutPlots(kTRUE),
   fHdr(0), fAdc(0), fFpnSub(0), fAmpOut(0),
   fEvtsPerBin(0), fAdcVsEvt(0), fFpnSubVsEvt(0), fFpnSubErrVsEvt(0),
   fAmpOutVsEvt(0), fAmpOutErrVsEvt(0),
   fAdcFFTVsEvt(0), fFpnSubFFTVsEvt(0), fAmpOutFFTVsEvt(0),
   fAdcAveFFT(0), fFpnSubAveFFT(0), fAmpOutAveFFT(0),
   fEntryVsEvt(0), fAdcVsEntry(0), fFpnSubVsEntry(0), fFpnSubErrVsEntry(0),
   fAmpOutVsEntry(0), fAmpOutErrVsEntry(0),
   fAdcFFTVsEntry(0), fFpnSubFFTVsEntry(0), fAmpOutFFTVsEntry(0) {
}

TSnPlotWaveformsMod::TSnPlotWaveformsMod(const Char_t* name,
                                         const Int_t nevtbins,
                                         const Float_t evtmin,
                                         const Float_t evtmax) :
   TAModule(name, "make waveform plots"),
   fNevtBins(nevtbins), fEvtMin(evtmin), fEvtMax(evtmax),
   fDoAdcPlots(kTRUE), fDoFpnSubPlots(kTRUE), fDoAmpOutPlots(kTRUE),
   fHdr(0), fAdc(0), fFpnSub(0), fAmpOut(0),
   fEvtsPerBin(0),
   fAdcVsEvt(new TObjArray(NSnConstants::kNchans)),
   fFpnSubVsEvt(new TObjArray(NSnConstants::kNchans)), 
   fFpnSubErrVsEvt(new TObjArray(NSnConstants::kNchans)), 
   fAmpOutVsEvt(new TObjArray(NSnConstants::kNchans)),
   fAmpOutErrVsEvt(new TObjArray(NSnConstants::kNchans)),
   fAdcFFTVsEvt(new TObjArray(NSnConstants::kNchans)),
   fFpnSubFFTVsEvt(new TObjArray(NSnConstants::kNchans)),
   fAmpOutFFTVsEvt(new TObjArray(NSnConstants::kNchans)),
   fAdcAveFFT(new TObjArray(NSnConstants::kNchans)),
   fFpnSubAveFFT(new TObjArray(NSnConstants::kNchans)),
   fAmpOutAveFFT(new TObjArray(NSnConstants::kNchans)),
   fEntryVsEvt(0),
   fAdcVsEntry(new TObjArray(NSnConstants::kNchans)),
   fFpnSubVsEntry(new TObjArray(NSnConstants::kNchans)), 
   fFpnSubErrVsEntry(new TObjArray(NSnConstants::kNchans)), 
   fAmpOutVsEntry(new TObjArray(NSnConstants::kNchans)),
   fAmpOutErrVsEntry(new TObjArray(NSnConstants::kNchans)),
   fAdcFFTVsEntry(new TObjArray(NSnConstants::kNchans)),
   fFpnSubFFTVsEntry(new TObjArray(NSnConstants::kNchans)),
   fAmpOutFFTVsEntry(new TObjArray(NSnConstants::kNchans)) {

   fAdcVsEvt->SetName(GetAdcVsEvtName());
   fFpnSubVsEvt->SetName(GetFpnSubVsEvtName());
   fFpnSubErrVsEvt->SetName(GetFpnSubErrVsEvtName());
   fAmpOutVsEvt->SetName(GetAmpOutVsEvtName());
   fAmpOutErrVsEvt->SetName(GetAmpOutErrVsEvtName());
   fAdcFFTVsEvt->SetName(GetAdcFFTVsEvtName());
   fFpnSubFFTVsEvt->SetName(GetFpnSubFFTVsEvtName());
   fAmpOutFFTVsEvt->SetName(GetAmpOutFFTVsEvtName());
   fAdcAveFFT->SetName(GetAdcAveFFTName());
   fFpnSubAveFFT->SetName(GetFpnSubAveFFTName());
   fAmpOutAveFFT->SetName(GetAmpOutAveFFTName());
   fAdcVsEntry->SetName(GetAdcVsEntryName());
   fFpnSubVsEntry->SetName(GetFpnSubVsEntryName());
   fFpnSubErrVsEntry->SetName(GetFpnSubErrVsEntryName());
   fAmpOutVsEntry->SetName(GetAmpOutVsEntryName());
   fAmpOutErrVsEntry->SetName(GetAmpOutErrVsEntryName());
   fAdcFFTVsEntry->SetName(GetAdcFFTVsEntryName());
   fFpnSubFFTVsEntry->SetName(GetFpnSubFFTVsEntryName());
   fAmpOutFFTVsEntry->SetName(GetAmpOutFFTVsEntryName());

   fAdcVsEvt->SetOwner(kTRUE);
   fFpnSubVsEvt->SetOwner(kTRUE);
   fFpnSubErrVsEvt->SetOwner(kTRUE);
   fAmpOutVsEvt->SetOwner(kTRUE);
   fAmpOutErrVsEvt->SetOwner(kTRUE);
   fAdcFFTVsEvt->SetOwner(kTRUE);
   fFpnSubFFTVsEvt->SetOwner(kTRUE);
   fAmpOutFFTVsEvt->SetOwner(kTRUE);
   fAdcAveFFT->SetOwner(kTRUE);
   fFpnSubAveFFT->SetOwner(kTRUE);
   fAmpOutAveFFT->SetOwner(kTRUE);
   fAdcVsEntry->SetOwner(kTRUE);
   fFpnSubVsEntry->SetOwner(kTRUE);
   fFpnSubErrVsEntry->SetOwner(kTRUE);
   fAmpOutVsEntry->SetOwner(kTRUE);
   fAmpOutErrVsEntry->SetOwner(kTRUE);
   fAdcFFTVsEntry->SetOwner(kTRUE);
   fFpnSubFFTVsEntry->SetOwner(kTRUE);
   fAmpOutFFTVsEntry->SetOwner(kTRUE);
}

TSnPlotWaveformsMod::~TSnPlotWaveformsMod() {
   delete fEvtsPerBin;
   delete fAdcVsEvt;
   delete fFpnSubVsEvt;
   delete fFpnSubErrVsEvt;
   delete fAmpOutVsEvt;
   delete fAmpOutErrVsEvt;
   delete fAdcFFTVsEvt;
   delete fFpnSubFFTVsEvt;
   delete fAmpOutFFTVsEvt;
   delete fAdcAveFFT;
   delete fFpnSubAveFFT;
   delete fAmpOutAveFFT;
   delete fEntryVsEvt;
   delete fAdcVsEntry;
   delete fFpnSubVsEntry;
   delete fFpnSubErrVsEntry;
   delete fAmpOutVsEntry;
   delete fAmpOutErrVsEntry;
   delete fAdcFFTVsEntry;
   delete fFpnSubFFTVsEntry;
   delete fAmpOutFFTVsEntry;
}

void TSnPlotWaveformsMod::SlaveBegin() {
   // make waveform vs event hists
   
   TString hn;
   
   hn = GetEvtsPerBinName();
   fEvtsPerBin = new TH1F(hn.Data(),
                          "Num events per bin;event number;num events",
                          fNevtBins, fEvtMin, fEvtMax);
   AddOutput(fEvtsPerBin);
   
   TH2F* h;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      
      if (fDoAdcPlots) {
         hn = Form("%s_ch%d", GetAdcVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("raw waveform vs event (chan %d)"
                           ";event number;sample;ADC",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAdcVsEvt->AddAt(h, ch);
      }      

      if (fDoFpnSubPlots) {
         hn = Form("%s_ch%d", GetFpnSubVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FPN sub waveform vs event (chan %d)"
                           ";event number;sample;ADC-FPN",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fFpnSubVsEvt->AddAt(h, ch);

         hn = Form("%s_ch%d", GetFpnSubErrVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FPN sub error vs event (chan %d)"
                           ";event number;sample;FPN err",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fFpnSubErrVsEvt->AddAt(h, ch);
      }
      
      if (fDoAmpOutPlots) {
         hn = Form("%s_ch%d", GetAmpOutVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("calibrated waveform vs event (chan %d)"
                           ";event number;sample;mV",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAmpOutVsEvt->AddAt(h, ch);

         hn = Form("%s_ch%d", GetAmpOutErrVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("calibration error vs event (chan %d)"
                           ";event number;sample;mV",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAmpOutErrVsEvt->AddAt(h, ch);
      }
      
   }
   
   if (fDoAdcPlots) {
      AddOutput(fAdcVsEvt);
      ReqBranch(TSnRawTreeMaker::kEWvBrNm, fAdc);
   }
   if (fDoFpnSubPlots) {
      AddOutput(fFpnSubVsEvt);
      AddOutput(fFpnSubErrVsEvt);
      ReqBranch(TSnSaveCalibDataMod::kFPNSubDatBrNm, fFpnSub);
   }
   if (fDoAmpOutPlots) {
      AddOutput(fAmpOutVsEvt);
      AddOutput(fAmpOutErrVsEvt);
      ReqBranch(TSnSaveCalibDataMod::kAmpOutDatBrNm, fAmpOut);
   }
   
   ReqBranch(TSnRawTreeMaker::kEHdBrNm, fHdr);
}

void TSnPlotWaveformsMod::Process() {

   LoadBranch(TSnRawTreeMaker::kEHdBrNm);

   const UShort_t* adc(0);
   if (fDoAdcPlots) {
      LoadBranch(TSnRawTreeMaker::kEWvBrNm);
      adc = fAdc->GetWvData();
   }
   const Float_t* fpnsub(0);
   const Float_t* fpnsubErr(0);
   if (fDoFpnSubPlots) {
      LoadBranch(TSnSaveCalibDataMod::kFPNSubDatBrNm);
      fpnsub    = fFpnSub->GetData();
      fpnsubErr = fFpnSub->GetError();
   }
   const Float_t* ampout(0);
   const Float_t* ampoutErr(0);
   if (fDoAmpOutPlots) {
      LoadBranch(TSnSaveCalibDataMod::kAmpOutDatBrNm);
      ampout    = fAmpOut->GetData();
      ampoutErr = fAmpOut->GetError();
   }

   fEvtsPerBin->Fill(fHdr->GetEvtNum());
   
   Int_t smp;
   TH2F* hadc(0), * hfpn(0), * hcal(0),
                  * efpn(0), * ecal(0);
   for (UShort_t s=0; s<NSnConstants::kTotSamps; ++s,
                     ++adc, ++fpnsub, ++fpnsubErr, ++ampout, ++ampoutErr) {
      smp = s % NSnConstants::kNsamps;
      if (smp==0) {
         const Int_t ch = static_cast<Int_t>( s / NSnConstants::kNsamps );
         if (fDoAdcPlots) {
            hadc = dynamic_cast<TH2F*>(fAdcVsEvt->At(ch));
         }
         if (fDoFpnSubPlots) {
            hfpn = dynamic_cast<TH2F*>(fFpnSubVsEvt->At(ch));
            efpn = dynamic_cast<TH2F*>(fFpnSubErrVsEvt->At(ch));
         }
         if (fDoAmpOutPlots) {
            hcal = dynamic_cast<TH2F*>(fAmpOutVsEvt->At(ch));
            ecal = dynamic_cast<TH2F*>(fAmpOutErrVsEvt->At(ch));
         }
      }
      
      if (fDoAdcPlots) {
         hadc->Fill(fHdr->GetEvtNum(), smp, *adc);
      }
      if (fDoFpnSubPlots) {
         hfpn->Fill(fHdr->GetEvtNum(), smp, *fpnsub);
         efpn->Fill(fHdr->GetEvtNum(), smp, *fpnsubErr);
      }
      if (fDoAmpOutPlots) {
         hcal->Fill(fHdr->GetEvtNum(), smp, *ampout);
         ecal->Fill(fHdr->GetEvtNum(), smp, *ampoutErr);
      }
                       
   }
   
}

void TSnPlotWaveformsMod::Terminate() {
   // make all the rest of the hists (FFT, vs entry, etc)

   if (GetVerbosity()>5) {
      Info("Terminate","Making FFT and vs entry plots...");
   }
   
   TString hn;
   
   hn = GetEntryVsEvtName();
   fEntryVsEvt = new TGraph;
   fEntryVsEvt->SetName(hn.Data());
   fEntryVsEvt->SetTitle("consec entry vs event number;event number;"
                         "consecutive entry");
   AddOutput(fEntryVsEvt);

   // count non-zero event bins
   Int_t nentries(0);
   for (Int_t e=0; e<fNevtBins; ++e) {
      if ( fEvtsPerBin->GetBinContent(e+1) > 0.5 ) {
         fEntryVsEvt->SetPoint(nentries,
                               fEvtsPerBin->GetBinCenter(e+1), nentries);
         ++nentries;
      }
   }

   // FFT binning
   /*
   const Int_t  nhsmp = (NSnConstants::kNsamps >> 1); // samps/2
   const Float_t   fi = NSnConstants::kSampRate / NSnConstants::kNsamps;
   // put the frequencies at the center of the bins
   const Float_t fmin = -0.5 * fi;
   const Float_t fmax = (nhsmp+0.5) * fi;
   const Int_t  fbins = static_cast<Int_t>((fmax-fmin) / fi);
   */
   Int_t fbins(0);
   Double_t fmin(0), fmax(0);
   TSnSpectral::GetRealFFTFreqBins(NSnConstants::kNsamps,
                                   1.0 / NSnConstants::kSampRate,
                                   fbins, fmin, fmax);
   
   TH1* h;
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {

      if (fDoAdcPlots) {
         hn = Form("%s_ch%d", GetAdcFFTVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of raw waveform vs event (chan %d)"
                           ";event number;freq (GHz)",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      fbins, fmin, fmax);
         fAdcFFTVsEvt->AddAt(h, ch);
         
         hn = Form("%s_ch%d", GetAdcAveFFTName(), ch);
         h = new TH1F(hn.Data(),
                      Form("average FFT of raw waveform (chan %d)"
                           ";freq (GHz)",ch),
                      fbins, fmin, fmax);
         h->Sumw2();
         fAdcAveFFT->AddAt(h, ch);
         
         hn = Form("%s_ch%d", GetAdcVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("raw waveform vs entry (chan %d)"
                           ";consecutive entry;sample",ch),
                      nentries, -0.5, nentries-0.5,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAdcVsEntry->AddAt(h, ch);

         hn = Form("%s_ch%d", GetAdcFFTVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of raw waveform vs entry (chan %d)"
                           ";consecutive entry;freq (GHz)",ch),
                      nentries, -0.5, nentries-0.5,
                      fbins, fmin, fmax);
         fAdcFFTVsEntry->AddAt(h, ch);
      }

      if (fDoFpnSubPlots) {
         hn = Form("%s_ch%d", GetFpnSubFFTVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of FPN sub waveform vs event (chan %d)"
                           ";event number;freq (GHz)",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      fbins, fmin, fmax);
         fFpnSubFFTVsEvt->AddAt(h, ch);

         hn = Form("%s_ch%d", GetFpnSubFFTVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of FPN sub waveform vs entry (chan %d)"
                           ";consecutive entry;freq (GHz)",ch),
                      nentries, -0.5, nentries-0.5,
                      fbins, fmin, fmax);
         fFpnSubFFTVsEntry->AddAt(h, ch);

         hn = Form("%s_ch%d", GetFpnSubAveFFTName(), ch);
         h = new TH1F(hn.Data(),
                      Form("average FFT of FPN sub waveform (chan %d)"
                           ";freq (GHz)",ch),
                      fbins, fmin, fmax);
         h->Sumw2();
         fFpnSubAveFFT->AddAt(h, ch);

         hn = Form("%s_ch%d", GetFpnSubVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FPN sub waveform vs entry (chan %d)"
                           ";consecutive entry;sample",ch),
                      nentries, -0.5, nentries-0.5,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fFpnSubVsEntry->AddAt(h, ch);

         hn = Form("%s_ch%d", GetFpnSubErrVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("Error of FPN sub waveform vs entry (chan %d)"
                           ";consecutive entry;sample",ch),
                      nentries, -0.5, nentries-0.5,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fFpnSubErrVsEntry->AddAt(h, ch);
      }      

      if (fDoAmpOutPlots) {
         hn = Form("%s_ch%d", GetAmpOutFFTVsEvtName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of calib waveform vs event (chan %d)"
                           ";event number;freq (GHz)",ch),
                      fNevtBins, fEvtMin, fEvtMax,
                      fbins, fmin, fmax);
         fAmpOutFFTVsEvt->AddAt(h, ch);

         hn = Form("%s_ch%d", GetAmpOutFFTVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("FFT of calib waveform vs entry (chan %d)"
                           ";consecutive entry;freq (GHz)",ch),
                      nentries, -0.5, nentries-0.5,
                      fbins, fmin, fmax);
         fAmpOutFFTVsEntry->AddAt(h, ch);

         hn = Form("%s_ch%d", GetAmpOutAveFFTName(), ch);
         h = new TH1F(hn.Data(),
                      Form("average FFT of calib waveform (chan %d)"
                           ";freq (GHz)",ch),
                      fbins, fmin, fmax);
         h->Sumw2();
         fAmpOutAveFFT->AddAt(h, ch);
         
         hn = Form("%s_ch%d", GetAmpOutVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("calib waveform vs entry (chan %d)"
                           ";consecutive entry;sample",ch),
                      nentries, -0.5, nentries-0.5,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAmpOutVsEntry->AddAt(h, ch);

         hn = Form("%s_ch%d", GetAmpOutErrVsEntryName(), ch);
         h = new TH2F(hn.Data(),
                      Form("Error of calib waveform vs entry (chan %d)"
                           ";consecutive entry;sample",ch),
                      nentries, -0.5, nentries-0.5,
                      NSnConstants::kNsamps, -0.5,
                      NSnConstants::kNsamps - 0.5);
         fAmpOutErrVsEntry->AddAt(h, ch);
      }
   }

   if (fDoAdcPlots) {
      AddOutput(fAdcFFTVsEvt);
      AddOutput(fAdcAveFFT);
      AddOutput(fAdcVsEntry);
      AddOutput(fAdcFFTVsEntry);
   }
   if (fDoFpnSubPlots) {
      AddOutput(fFpnSubFFTVsEvt);
      AddOutput(fFpnSubFFTVsEntry);
      AddOutput(fFpnSubAveFFT);
      AddOutput(fFpnSubVsEntry);
      AddOutput(fFpnSubErrVsEntry);
   }
   if (fDoAmpOutPlots) {
      AddOutput(fAmpOutFFTVsEvt);
      AddOutput(fAmpOutFFTVsEntry);
      AddOutput(fAmpOutAveFFT);
      AddOutput(fAmpOutVsEntry);
      AddOutput(fAmpOutErrVsEntry);
   }

   // fill the hists
   
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      
      TH2F* hAdcVsEvt = dynamic_cast<TH2F*>(fAdcVsEvt->At(ch));
      TH2F* hAdcVsEntry = dynamic_cast<TH2F*>(fAdcVsEntry->At(ch));
      TH2F* hAdcFFTVsEvt = dynamic_cast<TH2F*>(fAdcFFTVsEvt->At(ch));
      TH2F* hAdcFFTVsEntry = dynamic_cast<TH2F*>(fAdcFFTVsEntry->At(ch));
      TH1F* hAdcAveFFT = dynamic_cast<TH1F*>(fAdcAveFFT->At(ch));
      Float_t* const Adc = (fDoAdcPlots) ? (new Float_t[NSnConstants::kNsamps])
                                         : 0;

      TH2F* hFpnSubVsEvt = dynamic_cast<TH2F*>(fFpnSubVsEvt->At(ch));
      TH2F* hFpnSubVsEntry = dynamic_cast<TH2F*>(fFpnSubVsEntry->At(ch));
      TH2F* hFpnSubErrVsEvt = dynamic_cast<TH2F*>(fFpnSubErrVsEvt->At(ch));
      TH2F* hFpnSubErrVsEntry = dynamic_cast<TH2F*>(fFpnSubErrVsEntry->At(ch));
      TH2F* hFpnSubFFTVsEvt = dynamic_cast<TH2F*>(fFpnSubFFTVsEvt->At(ch));
      TH2F* hFpnSubFFTVsEntry = dynamic_cast<TH2F*>(fFpnSubFFTVsEntry->At(ch));
      TH1F* hFpnSubAveFFT = dynamic_cast<TH1F*>(fFpnSubAveFFT->At(ch));
      Float_t* const FpnSub = (fDoFpnSubPlots) 
         ? (new Float_t[NSnConstants::kNsamps])
         : 0;

      TH2F* hAmpOutVsEvt = dynamic_cast<TH2F*>(fAmpOutVsEvt->At(ch));
      TH2F* hAmpOutVsEntry = dynamic_cast<TH2F*>(fAmpOutVsEntry->At(ch));
      TH2F* hAmpOutErrVsEvt = dynamic_cast<TH2F*>(fAmpOutErrVsEvt->At(ch));
      TH2F* hAmpOutErrVsEntry = dynamic_cast<TH2F*>(fAmpOutErrVsEntry->At(ch));
      TH2F* hAmpOutFFTVsEvt = dynamic_cast<TH2F*>(fAmpOutFFTVsEvt->At(ch));
      TH2F* hAmpOutFFTVsEntry = dynamic_cast<TH2F*>(fAmpOutFFTVsEntry->At(ch));
      TH1F* hAmpOutAveFFT = dynamic_cast<TH1F*>(fAmpOutAveFFT->At(ch));
      Float_t* const AmpOut = (fDoAmpOutPlots) 
         ? (new Float_t[NSnConstants::kNsamps])
         : 0;
      
      const Double_t* evtnum = fEntryVsEvt->GetX();
      for (Int_t e=0; e<nentries; ++e, ++evtnum) {
         
         const Int_t    evtBin    = fEvtsPerBin->FindBin(*evtnum);
         const Double_t evtsInBin = fEvtsPerBin->GetBinContent(evtBin);
         if (evtsInBin<1) {
            SendError(kAbortAnalysis, "Terminate",
                      "[%g] events in bin [%d] for entry [%d].",
                      evtsInBin, evtBin, e);
         }
         
         Float_t* adc    = Adc;
         Float_t* fpnsub = FpnSub;
         Float_t  fpnsuberr(0);
         Float_t* ampout = AmpOut;
         Float_t  ampouterr(0);
         for (UShort_t s=0; s<NSnConstants::kNsamps; ++s, ++adc, ++fpnsub, ++ampout) {
            if (fDoAdcPlots) {
               *adc = hAdcVsEvt->GetBinContent(evtBin, s+1);
               // average by num evts in this bin
               *adc /= evtsInBin;
               hAdcVsEvt->SetBinContent(evtBin, s+1, *adc);
               hAdcVsEntry->Fill(e, s, *adc);
            }
            if (fDoFpnSubPlots) {
               *fpnsub = hFpnSubVsEvt->GetBinContent(evtBin, s+1);
               fpnsuberr = hFpnSubErrVsEvt->GetBinContent(evtBin, s+1);
               // average by num evts in this bin
               *fpnsub /= evtsInBin;
               fpnsuberr /= evtsInBin;
               hFpnSubVsEvt->SetBinContent(evtBin, s+1, *fpnsub);
               hFpnSubVsEntry->Fill(e, s, *fpnsub);
               hFpnSubErrVsEvt->SetBinContent(evtBin, s+1, fpnsuberr);
               hFpnSubErrVsEntry->Fill(e, s, fpnsuberr);
            }
            if (fDoAmpOutPlots) {
               *ampout = hAmpOutVsEvt->GetBinContent(evtBin, s+1);
               ampouterr = hAmpOutErrVsEvt->GetBinContent(evtBin, s+1);
               // average by num evts in this bin
               *ampout /= evtsInBin;
               ampouterr /= evtsInBin;
               hAmpOutVsEvt->SetBinContent(evtBin, s+1, *ampout);
               hAmpOutVsEntry->Fill(e, s, *ampout);
               hAmpOutErrVsEvt->SetBinContent(evtBin, s+1, ampouterr);
               hAmpOutErrVsEntry->Fill(e, s, ampouterr);
            }
         }
         
         // FFT's
         if (fDoAdcPlots) {
            FillFFTHists(*hAdcFFTVsEvt, *hAdcFFTVsEntry, *hAdcAveFFT,
                         *evtnum, e, Adc);
         }
         if (fDoFpnSubPlots) {
            FillFFTHists(*hFpnSubFFTVsEvt, *hFpnSubFFTVsEntry, *hFpnSubAveFFT,
                         *evtnum, e, FpnSub);
         }
         if (fDoAmpOutPlots) {
            FillFFTHists(*hAmpOutFFTVsEvt, *hAmpOutFFTVsEntry, *hAmpOutAveFFT,
                         *evtnum, e, AmpOut);
         }
         
      } // end loop over entries
      
      // average FFT hists
      if (fDoAdcPlots) {
         hAdcAveFFT->Scale(1.0 / static_cast<Float_t>(nentries));
      }
      if (fDoFpnSubPlots) {
         hFpnSubAveFFT->Scale(1.0 / static_cast<Float_t>(nentries));
      }
      if (fDoAmpOutPlots) {
         hAmpOutAveFFT->Scale(1.0 / static_cast<Float_t>(nentries));
      }
      
      // clean up
      delete Adc;
      delete FpnSub;
      delete AmpOut;
      
   } // end loop over channels
   
}

void TSnPlotWaveformsMod::FillFFTHists(TH2F& hfftVsEvt, TH2F& hfftVsEntry,
                                       TH1F& hfftAve,
                                       const Double_t evtnum,
                                       const Int_t entry,
                                       Float_t* const data) {
   TSnSpectral::RealFFT(data, NSnConstants::kNsamps, kFALSE);
   TGraph* g = TSnSpectral::NewGraphFromRealFFT(data,
                                                NSnConstants::kNsamps, 
                        static_cast<Float_t>(1.0/NSnConstants::kSampRate));
   Double_t* gx = g->GetX(), * gy = g->GetY();
   const Int_t fbins = g->GetN();
   for (Int_t i=0; i<fbins; ++i, ++gx, ++gy) {
      hfftVsEvt.Fill(evtnum, *gx, *gy);
      hfftVsEntry.Fill(entry, *gx, *gy);
      hfftAve.Fill(*gx, *gy);
   }
   delete g;
}

