#include "TSnEvtGuiSeleFrame.h"

#include <TColor.h>
#include <TString.h>
#include <TList.h>
#include <TMath.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGTableLayout.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGTab.h>
#include <TGComboBox.h>
#include <TGListBox.h>
#include <TGLayout.h>

#include "TSnEvtGuiMainFrame.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

ClassImp(TSnEvtGuiSeleFrame);

TSnEvtGuiSeleFrame::TSnEvtGuiSeleFrame(TSnEvtGuiMainFrame* p,
                                       const UInt_t w,
                                       const UInt_t h,
                                       const UInt_t options,
                                       const Pixel_t back) :
   TGVerticalFrame(p, w, h, options, back),
   fChanBtnGroup(0), fWaveBtnGroup(0), fWaveAdcBtn(0), fWaveFpnSubBtn(0),
   fWaveAmpOutBtn(0), fWaveAdcFFTBtn(0), fWaveFpnSubFFTBtn(0),
   fWaveAmpOutFFTBtn(0), fEvtLabelFrame(0), fEvtLabel(0),
   fEntryLabel(0), fEvtRangeLbl(0), fEntryRangeLbl(0),
   fEvtNumEntry(0), fEntryNumEntry(0),
   fEvtNavBtnCont(0), fEvtNavBtns(0), fEvtPrevBtn(0), fEvtNextBtn(0),
   fLabelFrame(0), fTimeLabel(0), fTrgLabel(0), fRunLabel(0),
   fMiscTabs(0), fAdcTab(0), fAdcFFTTab(0), fFpnSubTab(0), fFpnSubFFTTab(0),
   fAmpOutTab(0), fAmpOutFFTTab(0), fAdcCombo(0), fAdcFFTCombo(0),
   fFpnSubCombo(0), fFpnSubFFTCombo(0), fAmpOutCombo(0), fAmpOutFFTCombo(0) {
   memset(fChanBtn, 0, sizeof(fChanBtn));
   if (p!=0) {
      
      const UInt_t fullwidth = 300;
      
      TGLayoutHints* tlh = new TGLayoutHints(kLHintsCenterX|kLHintsTop,2,2,2,2);
      TGLayoutHints* wlh = new TGLayoutHints(kLHintsExpandX|kLHintsTop,2,2,2,2);
      TGLayoutHints* xlh = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,
                                             2,2,2,2);
      
      //
      // channel selection
      //
      fChanBtnGroup = new TGButtonGroup(this, "", 
                                        kFixedSize | kChildFrame);
      fChanBtnGroup->Connect("Clicked(Int_t)","TSnEvtGuiMainFrame",p,
                             "ChangeWaveChans(Int_t)");

      // calc num rows/cols for channel buttons
      const UInt_t sqrt = static_cast<UInt_t>(
         TMath::Sqrt(NSnConstants::kNchans));
      const UInt_t a = static_cast<UInt_t>(
         sqrt + ( (NSnConstants::kNchans % sqrt) > 0 ? 1 : 0 ) );
      const UInt_t b = static_cast<UInt_t>( NSnConstants::kNchans / sqrt );
      const UInt_t chcols = TMath::Max(a,b);
      const UInt_t chrows = TMath::Min(a,b);

      fChanBtnGroup->SetLayoutManager(new TGTableLayout(fChanBtnGroup,
                                                        chrows, chcols));
      
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         fChanBtn[ch] = new TGCheckButton(fChanBtnGroup,
                                          Form("Channel &%hhu",ch),
                                          kChan0Btn+ch);
         fChanBtn[ch]->SetTextJustify(36);
         fChanBtn[ch]->SetMargins(0,0,0,0);
         fChanBtn[ch]->SetWrapLength(-1);
         fChanBtn[ch]->SetOn();
         const UInt_t col = static_cast<UInt_t>(ch/chcols);
         const UInt_t row = static_cast<UInt_t>(ch%chcols);
         fChanBtnGroup->SetLayoutHints(
            new TGTableLayoutHints(col, col+1, row, row+1,
                                   kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
            fChanBtn[ch]);
            
      }
      fChanBtnGroup->Resize(fullwidth,25*chrows);
      AddFrame(fChanBtnGroup, tlh);
      
      //
      // waveform / fft selection
      //
      fWaveBtnGroup = new TGButtonGroup(this, "", 
                                        kFixedSize | kChildFrame);
      fWaveBtnGroup->Connect("Clicked(Int_t)","TSnEvtGuiMainFrame",p,
                             "ChangeWaveType(Int_t)");
      fWaveBtnGroup->SetLayoutManager(new TGTableLayout(fWaveBtnGroup, 2, 3));

      fWaveAdcBtn = new TGRadioButton(fWaveBtnGroup, "Raw ADCs", kWaveAdcBtn);
      SetTextBtnProps(fWaveAdcBtn);
      fWaveAdcBtn->SetOn();
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(0, 1, 0, 1,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveAdcBtn);

      fWaveFpnSubBtn = new TGRadioButton(fWaveBtnGroup, "FPN Sub",
                                         kWaveFpnSubBtn);
      SetTextBtnProps(fWaveFpnSubBtn);
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(1, 2, 0, 1,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveFpnSubBtn);

      fWaveAmpOutBtn = new TGRadioButton(fWaveBtnGroup, "Amp Out",
                                         kWaveAmpOutBtn);
      SetTextBtnProps(fWaveAmpOutBtn);
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(2, 3, 0, 1,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveAmpOutBtn);
      
      fWaveAdcFFTBtn = new TGRadioButton(fWaveBtnGroup, "ADC FFT",
                                         kWaveAdcFFTBtn);
      SetTextBtnProps(fWaveAdcFFTBtn);
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(0, 1, 1, 2,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveAdcFFTBtn);

      fWaveFpnSubFFTBtn = new TGRadioButton(fWaveBtnGroup, "FPN Sub FFT",
                                            kWaveFpnSubFFTBtn);
      SetTextBtnProps(fWaveFpnSubFFTBtn);
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(1, 2, 1, 2,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveFpnSubFFTBtn);
      
      fWaveAmpOutFFTBtn = new TGRadioButton(fWaveBtnGroup, "Amp Out FFT",
                                            kWaveAmpOutFFTBtn);
      SetTextBtnProps(fWaveAmpOutFFTBtn);
      fWaveBtnGroup->SetLayoutHints(
         new TGTableLayoutHints(2, 3, 1, 2,
                                kLHintsCenterX|kLHintsExpandX|kLHintsExpandY),
         fWaveAmpOutFFTBtn);

      AddFrame(fWaveBtnGroup, tlh);
      fWaveBtnGroup->Resize(fullwidth, 70);
      
      //
      // event number and input
      //
      fEvtLabelFrame = new TGCompositeFrame(this);
      fEvtLabelFrame->SetLayoutManager(new TGTableLayout(fEvtLabelFrame,
                                                         3, 2));
      fEvtLabel = new TGLabel(fEvtLabelFrame,"DAQ Event Num");
      SetLabelTxtProps(fEvtLabel);
      fEvtLabelFrame->AddFrame(fEvtLabel,
                               new TGTableLayoutHints(0, 1, 0, 1,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      fEntryLabel = new TGLabel(fEvtLabelFrame,"Consec. Entry");
      SetLabelTxtProps(fEntryLabel);
      fEvtLabelFrame->AddFrame(fEntryLabel,
                               new TGTableLayoutHints(1, 2, 0, 1,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      fEvtNumEntry = new TGNumberEntry(fEvtLabelFrame, 0.0,
                                       20, kEvtNumEntry);
      fEvtLabelFrame->AddFrame(fEvtNumEntry,
                               new TGTableLayoutHints(0, 1, 1, 2,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      fEvtNumEntry->Connect("ValueSet(Long_t)","TSnEvtGuiMainFrame",p,
                            "ChangeEvtNum(Long_t)");
      fEntryNumEntry = new TGNumberEntry(fEvtLabelFrame, 0.0,
                                         20, kEntryNumEntry);
      fEvtLabelFrame->AddFrame(fEntryNumEntry,
                               new TGTableLayoutHints(1, 2, 1, 2,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      fEntryNumEntry->Connect("ValueSet(Long_t)","TSnEvtGuiMainFrame",p,
                              "ChangeEntryNum(Long_t)");
      fEvtRangeLbl = new TGLabel(fEvtLabelFrame,"### / ###");
      SetLabelTxtProps(fEvtRangeLbl);
      fEvtLabelFrame->AddFrame(fEvtRangeLbl,
                               new TGTableLayoutHints(0, 1, 2, 3,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      fEntryRangeLbl = new TGLabel(fEvtLabelFrame,"### / ###");
      SetLabelTxtProps(fEntryRangeLbl);
      fEvtLabelFrame->AddFrame(fEntryRangeLbl,
                               new TGTableLayoutHints(1, 2, 2, 3,
                                 kLHintsCenterX|kLHintsExpandX|kLHintsExpandY));
      AddFrame(fEvtLabelFrame, tlh);
      fEvtLabelFrame->Resize(fullwidth, 80);
      
      //
      // increment evt buttons
      //
      fEvtNavBtnCont = new TGCompositeFrame(this, 1,1,
                                            kChildFrame | kFixedSize);
      fEvtNavBtns = new TGButtonGroup(fEvtNavBtnCont,1,2,0,
         kChildFrame | kFixedSize);
      fEvtNavBtns->SetLayoutManager(new TGTableLayout(fEvtNavBtns,1,2));
      fEvtPrevBtn = new TGTextButton(fEvtNavBtns, "<< &Prev. Event",
                                     kEvtPrevBtn);
      SetTextBtnProps(fEvtPrevBtn);
      fEvtNavBtns->SetLayoutHints(
         new TGTableLayoutHints(0,1,0,1,
                                kLHintsFillX | kLHintsFillY
                                | kLHintsExpandX | kLHintsExpandY
                                ,2,2,2,2), fEvtPrevBtn);
      fEvtNextBtn = new TGTextButton(fEvtNavBtns, "&Next Event >>",
                                     kEvtNextBtn);
      SetTextBtnProps(fEvtNextBtn);
      fEvtNavBtns->SetLayoutHints(
         new TGTableLayoutHints(1,2,0,1,
                                kLHintsFillX | kLHintsFillY
                                | kLHintsExpandX | kLHintsExpandY
                                ,2,2,2,2), fEvtNextBtn);
      fEvtNavBtnCont->AddFrame(fEvtNavBtns, xlh);
      //fEvtNavBtns->Resize(fullwidth, 100);
      fEvtNavBtns->Connect("Clicked(Int_t)","TSnEvtGuiMainFrame",p,
                           "IncrementEvent(Int_t)");
      AddFrame(fEvtNavBtnCont, tlh);
      fEvtNavBtnCont->Resize(fullwidth, 100);
      
      //
      // time, trg, run labels
      //
      fLabelFrame = new TGCompositeFrame(this, 1,1,
                                         kChildFrame | kFixedSize);
      
      fTimeLabel = new TGLabel(fLabelFrame, "YYYY-MM-DD HH:MM:SS , DT=####ms");
      SetLabelTxtProps(fTimeLabel);
      fLabelFrame->AddFrame(fTimeLabel, wlh);
      
      fTrgLabel = new TGLabel(fLabelFrame, "Trigger Type");
      SetLabelTxtProps(fTrgLabel);
      fLabelFrame->AddFrame(fTrgLabel, wlh);
      
      fRunLabel = new TGLabel(fLabelFrame, "Run, Seq");
      SetLabelTxtProps(fRunLabel);
      fLabelFrame->AddFrame(fRunLabel, wlh);
      
      AddFrame(fLabelFrame, tlh);
      fLabelFrame->Resize(fullwidth, 75);
      
      //
      // misc tabs
      //
      fMiscTabs = new TGTab(this);
      fMiscTabs->ChangeOptions(kChildFrame | kFixedWidth | kFitHeight);
      fMiscTabs->Connect("Selected(Int_t)","TSnEvtGuiMainFrame",p,
                         "ChangeTab(Int_t)");
      fAdcTab = fMiscTabs->AddTab("ADC");
      fAdcFFTTab = fMiscTabs->AddTab("ADC FFT");
      fFpnSubTab = fMiscTabs->AddTab("FPN Sub");
      fFpnSubFFTTab = fMiscTabs->AddTab("FPN Sub FFT");
      fAmpOutTab = fMiscTabs->AddTab("Amp Out");
      fAmpOutFFTTab = fMiscTabs->AddTab("Amp Out FFT");
      fMiscTabs->SetTab(0);
      AddFrame(fMiscTabs, new TGLayoutHints(kLHintsCenterX|kLHintsTop
                                            |kLHintsExpandY,
                                            2,2,2,2));
      fMiscTabs->Resize(fullwidth, 200);
      //
      // combo boxes inside tabs
      //
      TGLayoutHints* clh = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY
                                             |kLHintsExpandX,
                                             2,2,2,2);
      fAdcCombo = new TGComboBox(fAdcTab, kAdcCombo);
      fAdcCombo->Select(0);
      fAdcCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                         "SelMiscPlot(const Char_t*)");
      fAdcCombo->Resize(fullwidth, 20);
      fAdcTab->AddFrame(fAdcCombo, clh);
      fAdcFFTCombo = new TGComboBox(fAdcFFTTab, kAdcFFTCombo);
      fAdcFFTCombo->Select(0);
      fAdcFFTCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                            "SelMiscPlot(const Char_t*)");
      fAdcFFTCombo->Resize(fullwidth, 20);
      fAdcFFTTab->AddFrame(fAdcFFTCombo, clh);
      fFpnSubCombo = new TGComboBox(fFpnSubTab, kFpnSubCombo);
      fFpnSubCombo->Select(0);
      fFpnSubCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                            "SelMiscPlot(const Char_t*)");
      fFpnSubCombo->Resize(fullwidth, 20);
      fFpnSubTab->AddFrame(fFpnSubCombo, clh);
      fFpnSubFFTCombo = new TGComboBox(fFpnSubFFTTab, kFpnSubFFTCombo);
      fFpnSubFFTCombo->Select(0);
      fFpnSubFFTCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                               "SelMiscPlot(const Char_t*)");
      fFpnSubFFTCombo->Resize(fullwidth, 20);
      fFpnSubFFTTab->AddFrame(fFpnSubFFTCombo, clh);
      fAmpOutCombo = new TGComboBox(fAmpOutTab, kAmpOutCombo);
      fAmpOutCombo->Select(0);
      fAmpOutCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                            "SelMiscPlot(const Char_t*)");
      fAmpOutCombo->Resize(fullwidth, 20);
      fAmpOutTab->AddFrame(fAmpOutCombo, clh);
      fAmpOutFFTCombo = new TGComboBox(fAmpOutFFTTab, kAmpOutFFTCombo);
      fAmpOutFFTCombo->Select(0);
      fAmpOutFFTCombo->Connect("Selected(char*)","TSnEvtGuiMainFrame",p,
                               "SelMiscPlot(const Char_t*)");
      fAmpOutFFTCombo->Resize(fullwidth, 20);
      fAmpOutFFTTab->AddFrame(fAmpOutFFTCombo, clh);
      
      
      MapSubwindows();
      Resize(w,h);
      MapWindow();
      Resize(w,h);
      Layout();
   }
}

TSnEvtGuiSeleFrame::~TSnEvtGuiSeleFrame() {
   DeleteWindow();
}

void TSnEvtGuiSeleFrame::SetTextBtnProps(TGTextButton* b) {
   if (b!=0) {
      b->SetTextJustify(36);
      b->SetMargins(0,0,0,0);
      b->SetWrapLength(-1);
   }
}

void TSnEvtGuiSeleFrame::SetLabelTxtProps(TGLabel* l) {
   if (l!=0) {
      l->SetTextJustify(kTextCenterX | kTextCenterY);
      l->SetMargins(0,0,0,0);
      l->SetWrapLength(-1);
   }
}

void TSnEvtGuiSeleFrame::DisableTab(const Int_t comboid) {
   if (fMiscTabs!=0) {
      const Int_t id = comboid - kAdcCombo;
      if ( fMiscTabs->IsEnabled(id) ) {
         fMiscTabs->SetEnabled(id, kFALSE);
      }
   }
}

void TSnEvtGuiSeleFrame::EnableTab(const Int_t comboid) {
   if (fMiscTabs!=0) {
      const Int_t id = comboid - kAdcCombo;
      if ( fMiscTabs->IsEnabled(id)==kFALSE ) {
         fMiscTabs->SetEnabled(id, kTRUE);
      }
   }
}

void TSnEvtGuiSeleFrame::DisableWave(const Int_t bid) {
   TGButtonGroup* btns = GetWaveButtons();
   if (btns!=0) {
      TGRadioButton* b = 
         dynamic_cast<TGRadioButton*>(btns->GetButton(bid));
      if (b!=0) {
         b->SetState(kButtonUp,kFALSE);
         b->SetEnabled(kFALSE);
      } else {
         Error("DisableWave", "Could not get button with id [%d]", bid);
      }
   }
}

void TSnEvtGuiSeleFrame::EnableWave(const Int_t bid) {
   TGButtonGroup* btns = GetWaveButtons();
   if (btns!=0) {
      TGRadioButton* b = 
         dynamic_cast<TGRadioButton*>(btns->GetButton(bid));
      if (b!=0) {
         if (b->IsEnabled()==kFALSE) {
            b->SetEnabled(kTRUE);
         }
      } else {
         Error("EnableWave", "Could not get button with id [%d]", bid);
      }
   }
}

void TSnEvtGuiSeleFrame::DisableChan(const Int_t ch) {
   if ( (ch>=0) && (ch<NSnConstants::kNchans) ) {
      if (fChanBtn[ch]!=0) {
         //fChanBtn[ch]->SetOn(kFALSE);
         fChanBtn[ch]->SetState(kButtonUp,kFALSE);
         fChanBtn[ch]->SetEnabled(kFALSE);
      }
   }
}

void TSnEvtGuiSeleFrame::EnableChan(const Int_t ch) {
   if ( (ch>=0) && (ch<NSnConstants::kNchans) ) {
      if (fChanBtn[ch]!=0) {
         if (fChanBtn[ch]->IsEnabled()==kFALSE) {
            fChanBtn[ch]->SetEnabled(kTRUE);
         }
      }
   }
}

void TSnEvtGuiSeleFrame::SetChanColor(const Int_t ch, const Int_t col) {
   if (fChanBtn[ch]!=0) {
      fChanBtn[ch]->SetForegroundColor(TColor::Number2Pixel(col));
   }
}

TGCheckButton* TSnEvtGuiSeleFrame::GetChanButton(const Int_t ch) {
   if ( (ch>=0) && (ch<NSnConstants::kNchans) ) {
      return fChanBtn[ch];
   }
   return 0;
}

void TSnEvtGuiSeleFrame::SetEventNum(const Long_t event,
                                     const Double_t maxevt) {
   if (fEvtNumEntry!=0) {
      fEvtNumEntry->SetNumber(event);
   }
   if (fEvtRangeLbl!=0) {
      fEvtRangeLbl->SetText(Form("%ld / %lld", event,
                                 static_cast<Long64_t>(maxevt)));
      if (fEvtLabelFrame!=0) {
         fEvtLabelFrame->Layout();
      }
   }
}

void TSnEvtGuiSeleFrame::SetEntryNum(const Long_t entry,
                                     const Double_t maxentry) {
   if (fEntryNumEntry!=0) {
      fEntryNumEntry->SetNumber(entry);
   }
   if (fEntryRangeLbl!=0) {
      fEntryRangeLbl->SetText(Form("%ld / %lld", entry,
                                   static_cast<Long64_t>(maxentry)));
      if (fEvtLabelFrame!=0) {
         fEvtLabelFrame->Layout();
      }
   }
}

void TSnEvtGuiSeleFrame::SetEvtEntNums(const Long_t event,
                                       const Long_t entry,
                                       const Double_t maxevt,
                                       const Double_t maxentry) {
   SetEventNum(event, maxevt);
   SetEntryNum(entry, maxentry);
}

Int_t TSnEvtGuiSeleFrame::GetEventNum() const {
   return fEvtNumEntry->GetIntNumber();
}

Int_t TSnEvtGuiSeleFrame::GetEntryNum() const {
   return fEntryNumEntry->GetIntNumber();
}

void TSnEvtGuiSeleFrame::SetEventLimits(const Double_t min, const Double_t max) {
   fEvtNumEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, min, max);
}

void TSnEvtGuiSeleFrame::SetEntryLimits(const Double_t min, const Double_t max) {
   fEntryNumEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, min, max);
}

void TSnEvtGuiSeleFrame::UpdateLabels(const TSnEventHeader* hdr,
                                      const TSnEventMetadata* meta) {
   if (hdr!=0) {
      fTimeLabel->SetText(Form("%s , DT=%dms",
                               hdr->GetTime().AsString("s"),
                               hdr->GetDTms()));
      if (hdr->IsForced()) {
         fTrgLabel->SetText("Forced Trigger");
         fTrgLabel->SetForegroundColor(TColor::Number2Pixel(kAzure-6));
      } else if (hdr->IsThermal()) {
         fTrgLabel->SetText("Thermal Trigger");
         fTrgLabel->SetForegroundColor(TColor::Number2Pixel(kBlack));
      } else {
         fTrgLabel->SetText("Unknown Trigger");
         fTrgLabel->SetForegroundColor(TColor::Number2Pixel(kGray+2));
      }
      if (hdr->IsAdcToMBFlagged()) {
         fTrgLabel->SetText(Form("%s, ADC->MB Flagged!",
                                 fTrgLabel->GetText()->Data()));
         fTrgLabel->SetForegroundColor(TColor::Number2Pixel(kRed+1));
      }
   } else {
      fTimeLabel->SetText("Event Time Unavailable");
      fTrgLabel->SetText("Trigger Info Unavailable");
   }
   
   if (meta!=0) {
      fRunLabel->SetText(Form("Run=%u, Seq=%hu",
                              meta->GetRunNum(), meta->GetSeqNum()));
   } else {
      fRunLabel->SetText("Run, Seq Unavailable");
   }
   
   if (fLabelFrame!=0) {
      fLabelFrame->Layout();
   }
}

const Char_t* TSnEvtGuiSeleFrame::GetMiscPlotName() {
   if (fMiscTabs!=0) {
      TGCompositeFrame* cont = fMiscTabs->GetCurrentContainer();
      if (cont!=0) {
         TList* eles = cont->GetList();
         if (1==eles->GetEntries()) {
            TGFrameElement* ele = dynamic_cast<TGFrameElement*>(eles->At(0));
            if (ele!=0) {
               TGComboBox* cbox = dynamic_cast<TGComboBox*>(
                  ele->fFrame);
               if (cbox!=0) {
                  TGLBEntry* sel = cbox->GetSelectedEntry();
                  if (sel!=0) {
                     return sel->GetTitle();
                  }
               }
            }
         }
      }
   }
   return 0;
}

