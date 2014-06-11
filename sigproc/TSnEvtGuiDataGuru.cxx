#include "TSnEvtGuiDataGuru.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <THashTable.h>
#include <TObjArray.h>
#include <TClass.h>
#include <TCanvas.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>

#include "TSnRawTreeMaker.h"
#include "TSnPlotWaveformsMod.h"
#include "TSnEvtGuiSeleFrame.h"
#include "NSnConstants.h"
#include "TSnMath.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

ClassImp(TSnEvtGuiDataGuru);

TSnEvtGuiDataGuru::TSnEvtGuiDataGuru(TTree* tree, TFile* plotf) :
   fTree(tree), fPlotf(plotf), fEvt(0), 
   fWave(TSnPlotWaveformsMod::GetAdcVsEvtName()),
   fWaveSources(new THashTable),  fMiscPlots(new THashTable),
   fEntryVsEvt(0),
   fMinEvt(0), fMaxEvt(0), fMinEntry(0), fMaxEntry(0),
   fEvtHdr(0), fEvtMeta(0) {

   fWaveChansOn.resize(NSnConstants::kNchans);
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      fWaveChansOn.at(ch) = true;
   }
   
   fChanColors.resize(NSnConstants::kNchans);
   fChanColors.at(0) = kOrange+7;
   fChanColors.at(1) = kAzure-2;
   fChanColors.at(2) = kViolet-1;
   fChanColors.at(3) = kGreen+2;
   
   fWaveHists.reserve(NSnConstants::kNchans);
   
   fWaveSources->SetOwner(kTRUE);
   fMiscPlots->SetOwner(kTRUE);

   if (fPlotf!=0) {
      fEntryVsEvt = dynamic_cast<TGraph*>(
         fPlotf->Get(TSnPlotWaveformsMod::GetEntryVsEvtName()));
      if (fEntryVsEvt!=0) {
         fMinEvt = TMath::MinElement(fEntryVsEvt->GetN(),
                                     fEntryVsEvt->GetX());
         fMaxEvt = TMath::MaxElement(fEntryVsEvt->GetN(),
                                     fEntryVsEvt->GetX());
         fMinEntry = TMath::MinElement(fEntryVsEvt->GetN(),
                                       fEntryVsEvt->GetY());
         fMaxEntry = TMath::MaxElement(fEntryVsEvt->GetN(),
                                       fEntryVsEvt->GetY());
      }
   }
   
   if (fTree!=0) {
      fTree->BuildIndex(Form("%s.fNum",TSnRawTreeMaker::kEHdBrNm));
      fEvtHdr = new TSnEventHeader;
      fEvtMeta = new TSnEventMetadata;
      fTree->SetBranchAddress(TSnRawTreeMaker::kEHdBrNm, &fEvtHdr);
      fTree->SetBranchAddress(TSnRawTreeMaker::kEMtBrNm, &fEvtMeta);
   }
   
}

TSnEvtGuiDataGuru::~TSnEvtGuiDataGuru() {
   delete fWaveSources;
   delete fMiscPlots;
}

void TSnEvtGuiDataGuru::ChangeWaveChans(TCanvas* c,
                                        TSnEvtGuiSeleFrame* seleFrame,
                                        const Int_t ch) {
   if ( (c!=0) && (ch>=0) && (ch<NSnConstants::kNchans) ) {
      if (seleFrame!=0) {
         TGCheckButton* b = seleFrame->GetChanButton(ch);
         if (b!=0) {
            SetWaveChanOn(ch, b->IsOn());
            RedrawWaveChans(c, seleFrame);
         }
      }
   }
}

void TSnEvtGuiDataGuru::ClearWaves(TCanvas* c) {
   std::vector<TH1*>::iterator h, hend=fWaveHists.end();
   for (h=fWaveHists.begin(); h!=hend; ++h) {
      delete *h;
   }
   fWaveHists.clear();
   if (c!=0) {
      c->Clear();
   }
}

TObject* TSnEvtGuiDataGuru::LoadMiscPlot(const Char_t* name) {
   TObject* o(0);
   if (fWaveSources!=0) {
      // if it's in fWaveSources, don't put it in fMiscPlots
      // or it'll get deleted twice
      o = fWaveSources->FindObject(name);
   }
   if (o==0) {
      if ((fMiscPlots!=0) && (fPlotf!=0)) {
         o = fPlotf->Get(name);
         if (o!=0) {
            fMiscPlots->Add(o);
            if (o->IsA()->InheritsFrom(TH1::Class())) {
               TH1* h = static_cast<TH1*>(o);
               h->SetDirectory(0);
               if (o->IsA()->InheritsFrom(TH2::Class())) {
                  // skip that first event that's often screwed up
                  h->GetXaxis()->SetRange(2, h->GetNbinsX());
               }
            }
         }
      }
   }
   return o;
}

TH2* TSnEvtGuiDataGuru::Load2DHist(const Char_t* name) {
   if ((fWaveSources!=0) && (fPlotf!=0)) {
      TObject* o = fWaveSources->FindObject(name);
      if (o!=0) {
         return dynamic_cast<TH2*>(o);
      } else {
         TH2* h2 = dynamic_cast<TH2*>(fPlotf->Get(name));
         if (h2!=0) {
            // take it out of misc if it's in there (avoid double deletion)
            TObject* x = fMiscPlots->FindObject(name);
            if (x!=0) {
               fMiscPlots->Remove(x);
            }
            fWaveSources->Add(h2);
            h2->SetDirectory(0);
            // skip that first event that's often screwed up
            h2->GetXaxis()->SetRange(2, h2->GetNbinsX());
            return h2;
         } else {
            Warning("Load2DHist","Could not find TH2 named [%s] in [%s]",
                    name, fPlotf->GetName());
         }
      }
   }
   return 0;
}

void TSnEvtGuiDataGuru::ClearMisc(TCanvas* c) {
   if (c!=0) {
      c->Clear();
   }
}

void TSnEvtGuiDataGuru::RedrawMisc(TCanvas* c,
                                   TSnEvtGuiSeleFrame* seleFrame) {
   if ((c!=0) && (fPlotf!=0) && (seleFrame!=0)) {
      ClearMisc(c);
      c->cd();
      TString hn(seleFrame->GetMiscPlotName());
      TObject* o = LoadMiscPlot(hn.Data());
      if (o!=0) {
         if (o->IsA()->InheritsFrom(TH2::Class())) {
            TH2* h = static_cast<TH2*>(o);
            h->Draw("colz");
         } else if (o->IsA()->InheritsFrom(TGraph::Class())) {
            TGraph* h = static_cast<TGraph*>(o);
            h->Draw("ap");
         } else {
            o->Draw();
         }
      }
      c->Update();
   }
}


void TSnEvtGuiDataGuru::RedrawWaveChans(TCanvas* c,
                                        TSnEvtGuiSeleFrame* seleFrame) {
   if ((c!=0) && (fPlotf!=0)) {
      ClearWaves(c);
      c->cd();
      std::vector<bool>::const_iterator chOn, end=fWaveChansOn.end();
      std::vector<Int_t>::const_iterator col = fChanColors.begin();
      Int_t ch=0;
      Bool_t first=kTRUE;
      Double_t min(0), max(0);
      TH1D* h, * hf(0);
      TH2* h2;
      TString hn, ht;
      Int_t e=-1;
      for (chOn=fWaveChansOn.begin(); chOn!=end; ++chOn, ++col, ++ch) {
         hn = Form("%s_ch%d",fWave.Data(), ch);
         h2 = Load2DHist(hn.Data());
         if (h2!=0) {
            if (e<0) {
               e = h2->GetXaxis()->FindBin(fEvt);
            }
            h = h2->ProjectionY(Form("%s_ev%d",hn.Data(),fEvt),e,e);
            h->SetDirectory(0);
            // check if this channel has a waveform. can't use GetEntries
            // because that returns the integral cast to an integer for a
            // projection, which will be 0 sometimes. instead, check if
            // the waveform is not totally flat at 0.
            if ( (h->GetMaximum()!=0) && (h->GetMaximum()!=h->GetMinimum()) ) {
               fWaveHists.push_back(h); // so it can be deleted later
               h->SetLineColor(*col);
               //h->SetFillColor(*col);
               //h->SetFillStyle(3003);
               // TODO: get errors
               ht = h->GetTitle();
               if (ht.Contains("vs")) {
                  ht  = ht(0, ht.Index("vs"));
                  ht += Form("(event %d)", fEvt);
               }
               h->SetTitle(ht.Data());
               if (*chOn) {
                  if (first) {
                     min = h->GetMinimum();
                     max = h->GetMaximum();
                     h->Draw();
                     hf = h;
                     first = kFALSE;
                  } else {
                     min = TMath::Min(min, h->GetMinimum());
                     max = TMath::Max(max, h->GetMaximum());
                     h->Draw("same");
                  }
               }
               EnableChan(ch, seleFrame);
            } else {
               /*
               Printf("disable! ch=%d, fEvt=%d, e=%d, entries=%g",
                      ch, fEvt, e, h->GetEntries());
               */
               delete h;
               DisableChan(ch, seleFrame);
            }
         }
      }
      if (hf!=0) {
         hf->SetMaximum(max*1.02);
         hf->SetMinimum(min*0.98);
      }
      c->Update();
      UpdateLabels(seleFrame);
   }
}

void TSnEvtGuiDataGuru::DisableChan(const Int_t ch,
                                    TSnEvtGuiSeleFrame* seleFrame) {
   fWaveChansOn.at(ch) = false;
   if (seleFrame!=0) {
      seleFrame->DisableChan(ch);
   }
}

void TSnEvtGuiDataGuru::EnableChan(const Int_t ch,
                                   TSnEvtGuiSeleFrame* seleFrame) {
   if (seleFrame!=0) {
      seleFrame->EnableChan(ch);
   }
}

void TSnEvtGuiDataGuru::ChangeWaveType(TCanvas* c,
                                       TSnEvtGuiSeleFrame* seleFrame,
                                       const Int_t id) {
   if ((c!=0) && (seleFrame!=0)) {
      Bool_t ok = kTRUE;
      switch (id) {
         case TSnEvtGuiSeleFrame::kWaveAdcBtn:
            fWave = TSnPlotWaveformsMod::GetAdcVsEvtName();
            break;
         case TSnEvtGuiSeleFrame::kWaveFpnSubBtn:
            fWave = TSnPlotWaveformsMod::GetFpnSubVsEvtName();
            break;
         case TSnEvtGuiSeleFrame::kWaveAmpOutBtn:
            fWave = TSnPlotWaveformsMod::GetAmpOutVsEvtName();
            break;
         case TSnEvtGuiSeleFrame::kWaveAdcFFTBtn:
            fWave = TSnPlotWaveformsMod::GetAdcFFTVsEvtName();
            break;
         case TSnEvtGuiSeleFrame::kWaveFpnSubFFTBtn:
            fWave = TSnPlotWaveformsMod::GetFpnSubFFTVsEvtName();
            break;
         case TSnEvtGuiSeleFrame::kWaveAmpOutFFTBtn:
            fWave = TSnPlotWaveformsMod::GetAmpOutFFTVsEvtName();
            break;
         default:
            ok = kFALSE;
            break;
      };
      if (ok) {
         RedrawWaveChans(c, seleFrame);
      } else {
         Error("ChangeWaveType",
               "Unexpected widget id [%d].",id);
      }
   }
}

Bool_t TSnEvtGuiDataGuru::GetEventNum(const Int_t entry,
                                      Int_t& event) {
   // we use the entry vs event graph and ASSUME a 1 to 1 mapping
   // (this assumption can be broken if more than one run have been
   //  chained together)
   
   Bool_t ok = kFALSE;
   event = 0;
   if (fEntryVsEvt!=0) {
      const Double_t dent = static_cast<Double_t>(entry);
      Double_t devt;
      ok = TSnMath::SearchOneToOneMapping(fEntryVsEvt->GetN(),
                                          fEntryVsEvt->GetY(),
                                          fEntryVsEvt->GetX(),
                                          dent, devt);
      if (ok) {
         event = static_cast<Int_t>(devt);
      } else {
         Error("GetEventNum",
               "Could not find event number corresponding to entry [%d]",
               entry);
      }
   } else {
      Error("GetEventNum",
            "Entry vs event graph is null");
   }
   return ok;
}

Bool_t TSnEvtGuiDataGuru::GetEntryNum(const Int_t event,
                                      Int_t& entry) {
   // we use the entry vs event graph and ASSUME a 1 to 1 mapping
   // (this assumption can be broken if more than one run have been
   //  chained together)
   
   Bool_t ok = kFALSE;
   entry = 0;
   if (fEntryVsEvt!=0) {
      const Double_t devt = static_cast<Double_t>(event);
      Double_t dent;
      ok = TSnMath::SearchOneToOneMapping(fEntryVsEvt->GetN(),
                                          fEntryVsEvt->GetX(),
                                          fEntryVsEvt->GetY(),
                                          devt, dent);
      if (ok) {
         entry = static_cast<Int_t>(dent);
      } else {
         Error("GetEntryNum",
               "Could not find entry number corresponding to event [%d]",
               event);
      }
   } else {
      Error("GetEntryNum",
            "Entry vs event graph is null");
   }
   return ok;
}

void TSnEvtGuiDataGuru::SetEntryNum(const Int_t entry) {
   // this function does not redraw. that should be done externally
   // (by TSnEvtGuiMainFrame::ChangeEntryNum)
   //
   // we use the entry vs event graph and ASSUME a 1 to 1 mapping
   // (this assumption can be broken if more than one run have been
   //  chained together)
   
   Int_t evt(0);
   if ( GetEventNum(entry, evt) ) {
      fEvt = evt;
   }
   
}

void TSnEvtGuiDataGuru::UpdateLabels(TSnEvtGuiSeleFrame* seleFrame) {
   if (fTree!=0) {
      const Long64_t tent = fTree->GetEntryNumberWithIndex(fEvt);
      if (tent>-1) {
         fTree->GetEntry(tent);
         if (seleFrame!=0) {
            seleFrame->UpdateLabels(fEvtHdr, fEvtMeta);
         }
      } else {
         Error("UpdateLabels",
               "Could not get entry in tree [%s] for event [%d]",
               fTree->GetName(), fEvt);
      }
   }
}

TObjArray* TSnEvtGuiDataGuru::NewListOfObjsWithTag(const Char_t* tag) {
   TObjArray* names(0);
   if (fPlotf!=0) {
      TList* keys = fPlotf->GetListOfKeys();
      if (keys!=0) {
         TIter nk(keys);
         TObject* k;
         TString kn;
         while ( (k=nk())!=0 ) {
            kn = k->GetName();
            if (kn.BeginsWith(tag)) {
               if (names==0) {
                  names = new TObjArray;
               }
               names->Add(new TObjString(kn.Data()));
            }
         }
      }
   }
   return names;
}

void TSnEvtGuiDataGuru::ReadAllWaveHists(TSnEvtGuiSeleFrame* seleFrame) {
   if ( (seleFrame!=0) && (fPlotf!=0) ) {
      TString hn;
      TH2* h2;
      TString waveTypes[6] = {
         TSnPlotWaveformsMod::GetAdcVsEvtName(),
         TSnPlotWaveformsMod::GetFpnSubVsEvtName(),
         TSnPlotWaveformsMod::GetAmpOutVsEvtName(),
         TSnPlotWaveformsMod::GetAdcFFTVsEvtName(),
         TSnPlotWaveformsMod::GetFpnSubFFTVsEvtName(),
         TSnPlotWaveformsMod::GetAmpOutFFTVsEvtName()
      };
      Int_t bids[6] = {
         TSnEvtGuiSeleFrame::kWaveAdcBtn,
         TSnEvtGuiSeleFrame::kWaveFpnSubBtn,
         TSnEvtGuiSeleFrame::kWaveAmpOutBtn,
         TSnEvtGuiSeleFrame::kWaveAdcFFTBtn,
         TSnEvtGuiSeleFrame::kWaveFpnSubFFTBtn,
         TSnEvtGuiSeleFrame::kWaveAmpOutFFTBtn
      };
      Int_t tabs[6] = {
         TSnEvtGuiSeleFrame::kAdcCombo,
         TSnEvtGuiSeleFrame::kFpnSubCombo,
         TSnEvtGuiSeleFrame::kAmpOutCombo,
         TSnEvtGuiSeleFrame::kAdcFFTCombo,
         TSnEvtGuiSeleFrame::kFpnSubFFTCombo,
         TSnEvtGuiSeleFrame::kAmpOutFFTCombo
      };
      for (UChar_t t=0; t<6; ++t) {
         Bool_t gotOne=kFALSE;
         for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            hn = Form("%s_ch%d", waveTypes[t].Data(), ch);
            h2 = Load2DHist(hn.Data());
            if (h2!=0) {
               gotOne=kTRUE;
            }
         }
         if (gotOne) {
            seleFrame->EnableWave(bids[t]);
            seleFrame->EnableTab(tabs[t]);
         } else {
            seleFrame->DisableWave(bids[t]);
            seleFrame->DisableTab(tabs[t]);
         }
      }
   }
}

void TSnEvtGuiDataGuru::ReadAllMiscHists(TSnEvtGuiSeleFrame* seleFrame) {
   if ( (seleFrame!=0) && (fPlotf!=0) ) {
      TObjArray* Adc = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAdcVsEvtName());
      TObjArray* FpnSub = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubVsEvtName());
      TObjArray* FpnSubErr = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubErrVsEvtName());
      TObjArray* AmpOut = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutVsEvtName());
      TObjArray* AmpOutErr = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutErrVsEvtName());
      TObjArray* AdcFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAdcFFTVsEvtName());
      TObjArray* FpnSubFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubFFTVsEvtName());
      TObjArray* AmpOutFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutFFTVsEvtName());
      TObjArray* AdcEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAdcVsEntryName());
      TObjArray* FpnSubEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubVsEntryName());
      TObjArray* FpnSubErrEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubErrVsEntryName());
      TObjArray* AmpOutEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutVsEntryName());
      TObjArray* AmpOutErrEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutErrVsEntryName());
      TObjArray* AdcFFTEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAdcFFTVsEntryName());
      TObjArray* FpnSubFFTEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubFFTVsEntryName());
      TObjArray* AmpOutFFTEntry = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutFFTVsEntryName());
      
      TObjArray* AdcAveFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAdcAveFFTName());
      TObjArray* FpnSubAveFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetFpnSubAveFFTName());
      TObjArray* AmpOutAveFFT = NewListOfObjsWithTag(
         TSnPlotWaveformsMod::GetAmpOutAveFFTName());
      
      Int_t nAdc = SendNamesTo(AdcEntry, seleFrame->GetAdcCombo(), 0);
      nAdc += SendNamesTo(Adc, seleFrame->GetAdcCombo(), nAdc);
      Int_t nFpnSub = SendNamesTo(FpnSubEntry, seleFrame->GetFpnSubCombo(), 0);
      nFpnSub += SendNamesTo(FpnSubErrEntry, seleFrame->GetFpnSubCombo(), nFpnSub);
      nFpnSub += SendNamesTo(FpnSub, seleFrame->GetFpnSubCombo(), nFpnSub);
      nFpnSub += SendNamesTo(FpnSubErr, seleFrame->GetFpnSubCombo(), 
                             nFpnSub);
      Int_t nAmpOut = SendNamesTo(AmpOutEntry, seleFrame->GetAmpOutCombo(), 0);
      nAmpOut += SendNamesTo(AmpOutErrEntry, seleFrame->GetAmpOutCombo(), nAmpOut);
      nAmpOut += SendNamesTo(AmpOut, seleFrame->GetAmpOutCombo(), nAmpOut);
      nAmpOut += SendNamesTo(AmpOutErr, seleFrame->GetAmpOutCombo(), 
                             nAmpOut);
      Int_t nAdcFFT = SendNamesTo(AdcAveFFT, seleFrame->GetAdcFFTCombo(), 0);
      nAdcFFT += SendNamesTo(AdcFFTEntry, seleFrame->GetAdcFFTCombo(), nAdcFFT);
      nAdcFFT += SendNamesTo(AdcFFT, seleFrame->GetAdcFFTCombo(), nAdcFFT);
      Int_t nFpnSubFFT = SendNamesTo(FpnSubAveFFT,
                                     seleFrame->GetFpnSubFFTCombo(), 0);
      nFpnSubFFT += SendNamesTo(FpnSubFFTEntry, seleFrame->GetFpnSubFFTCombo(), 
                                nFpnSubFFT);
      nFpnSubFFT += SendNamesTo(FpnSubFFT, seleFrame->GetFpnSubFFTCombo(),
                                nFpnSubFFT);
      Int_t nAmpOutFFT = SendNamesTo(AmpOutAveFFT,
                                     seleFrame->GetAmpOutFFTCombo(), 0);
      nAmpOutFFT += SendNamesTo(AmpOutFFTEntry, seleFrame->GetAmpOutFFTCombo(), 
                                nAmpOutFFT);
      nAmpOutFFT += SendNamesTo(AmpOutFFT, seleFrame->GetAmpOutFFTCombo(),
                                nAmpOutFFT);
      
      // so the first one will draw
      if ( (seleFrame->GetAdcCombo())!=0 ) {
         seleFrame->GetAdcCombo()->Select(0, kTRUE);
      }
   }
}

Int_t TSnEvtGuiDataGuru::SendNamesTo(TObjArray* names,
                                     TGComboBox* combo,
                                     const Int_t start) {
   // will delete names
   //
   // return number of names added
   
   Int_t added(0);
   if (names!=0) {
      TObject* n;
      const Int_t nents = names->GetEntries();
      for (Int_t i=0; i<nents; ++i) {
         n = names->At(i);
         if (combo!=0) {
            combo->AddEntry(n->GetName(), i+start);
            ++added;
         }
      }
      if (combo!=0) {
         combo->Select(0, kFALSE);
      }
      names->SetOwner(kTRUE);
      delete names;
   }
   return added;
}
