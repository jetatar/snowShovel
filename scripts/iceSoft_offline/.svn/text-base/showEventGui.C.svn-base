// Mainframe macro generated from application: root.exe
// By ROOT version 5.30/02 on 2011-11-16 16:48:39

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include <TGButtonGroup.h>

#include "Riostream.h"

#include <TClass.h>
#include <TDatime.h>
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TIterator.h>
#include <TH1.h>
#include <TList.h>
#include <TMath.h>
#include <TColor.h>
#include <TParameter.h>
#include <TTimeStamp.h>
#include <TSystem.h>
#include <TStyle.h>

TGMainFrame* fMainFrame1172;
TGCheckButton* fCheckButtons[4];
TGTextButton* fTextButton764;
TGLabel* fLabel769, * fTimeLabel, * fTrgLabel;
TRootEmbeddedCanvas* fRootEmbeddedCanvas774,
                   * fRootEmbeddedCanvas794;
TCanvas* c126, * c127;
TGNumberEntry* fNumberEntry844;
TGTextButton* fTextButton980, * fTextButton995;
TGComboBox* fComboBox560, * fComboBox563, * fComboBox565,
          * fComboBox569, * fComboBox571,
          * fComboBox655, * fComboBox660,
          * fComboBox665, * fComboBox670,
          * fComboBox755, * fComboBox760,
          * fComboBox765, * fComboBox770,
          * fComboBox775;
TGTab* fTab560, * fTab650, * fTab660;
TGCompositeFrame* fCompositeFrame563, * fCompositeFrame565,
                * fCompositeFrame567, * fCompositeFrame569,
                * fCompositeFrame571, * fCompositeFrame573,
                * fCompositeFrame655, * fCompositeFrame660,
                * fCompositeFrame665, * fCompositeFrame670,
                * fCompositeFrame755, * fCompositeFrame760,
                * fCompositeFrame765, * fCompositeFrame770,
                * fCompositeFrame775;
TGButtonGroup* fRadioButtonGroup1;
TGRadioButton* fRadioButtons100, * fRadioButtons105, * fRadioButtons110;

const Int_t col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};
Int_t curEvt=0, topEvt=0;
TString curCanv1PlotTag, curCanv2PlotName, curCanv1PlotDir;
TFile* inf;
TDirectory* tsdir=0, * wvdir=0, * dndir=0, * tgdir=0;

enum ETab_t {
   k2D=0, kPed, kADC, kAveFreq, kStops, kStopCands
};

enum EStopTab_t {
   kLoc=0, kLen, kSadc, kOffset, kNcand
};

const Char_t* getTrgStr(const Int_t tb) {
   // very simple at the moment
   if (tb==1) {
      return "Forced Trigger";
   } else if (tb==0) {
      return "Thermal Trigger";
   } else {
      return "Unknown Trigger Type";
   }
}

void drawObj(TObject* h, const Char_t* extraOpt="") {
   if (h!=0) {
      TString opt;
      if ( h->IsA()->InheritsFrom("TH2") ) {
         opt = "colz";
      } else if ( h->IsA()->InheritsFrom("TGraph") ) {
         opt = "ap";
      }
      h->Draw(Form("%s%s",opt.Data(),extraOpt));
      opt = h->GetName();
      const Bool_t logy = opt.BeginsWith("hADCdistOfSample");
      if (gPad!=0) {
         gPad->SetLogy(logy);
      }
   }
}

void redraw() {
   if (inf!=0) {
      // update event label
      TString evlbl = Form("Event %d / %d", curEvt, topEvt);
      if (dndir!=0) {
         TParameter<Int_t>* den = dynamic_cast< TParameter<Int_t>* >(
            dndir->Get(Form("DAQEvtNum_ev%06d",curEvt)));
         evlbl += Form("  (DAQ Evt#%u)",static_cast<UInt_t>(den->GetVal()));
         delete den;
      }
      fLabel769->SetText(evlbl);
      fLabel769->SetLayoutBroken(kTRUE);
      // update time label
      if (tsdir!=0) {
         TTimeStamp* ts = 0;
         tsdir->GetObject(Form("Time_ev%06d",curEvt),ts);
         if (ts!=0) {
            fTimeLabel->SetText(Form("%s", ts->AsString()));
         }
         delete ts;
      }
      fTimeLabel->SetLayoutBroken(kTRUE);
      // update trigger label
      if (tgdir!=0) {
         TParameter<Int_t>* trg = dynamic_cast< TParameter<Int_t>* >(
            tgdir->Get(Form("TrigBit_ev%06d",curEvt)));
         if (trg!=0) {
            fTrgLabel->SetText(getTrgStr(trg->GetVal()));
         }
      } else {
         fTrgLabel->SetText("Trigger Type N/A");
      }
      fTrgLabel->SetLayoutBroken(kTRUE);
      
      gSystem->ProcessEvents();
      
      // update canvas 1
      c126->cd();
      c126->Clear();
      Bool_t first=kTRUE;
      Double_t max,min;
      TH1* h, * hf=0;
      if (wvdir!=0) {
         for (Int_t i=0; i<4; i++) {
            h = dynamic_cast<TH1*>(
               wvdir->Get(Form("hWaveform_ch%02d_ev%06d",i,curEvt)) );
            if (h!=0) {
               if (h->GetEntries()>0) {
                  if (fCheckButtons[i]->IsEnabled()==kFALSE) {
                     fCheckButtons[i]->SetEnabled(kTRUE);
                  }
                  if (fCheckButtons[i]->IsDown()) {
                     if (curCanv1PlotDir.CompareTo("Waveforms")!=0) {
                        // if not Waveforms, need to get the desired histogram
                        TDirectory* hdir = 
                           inf->GetDirectory(curCanv1PlotDir.Data());
                        if (hdir!=0) {
                           delete h; h=0;
                           h = dynamic_cast<TH1*>(
                              hdir->Get(Form("%s_ch%02d_ev%06d",
                                        curCanv1PlotTag.Data(),i,curEvt)));
                        }
                     }
                     if (h!=0) {
                        h->SetLineColor(col[i]);
                        drawObj(h, (first) ? "" : "same" );
                        if (first) {
                           max = h->GetMaximum();
                           min = h->GetMinimum();
                           hf  = h;
                        } else {
                           max = TMath::Max(max, h->GetMaximum());
                           min = TMath::Min(min, h->GetMinimum());
                        }
                        first = kFALSE;
                     }
                  }
               } else {
                  // skip & de-select empty channels (w/o emitting)
                  fCheckButtons[i]->SetState(kButtonUp,kFALSE);
                  fCheckButtons[i]->SetEnabled(kFALSE);
               }
            }
         }
         if (hf!=0) {
            hf->SetMaximum(max*1.02);
            hf->SetMinimum(min*0.98);
         }
      }
      c126->Update();

      // update canvas 2
      c127->cd();
      c127->Clear();
      drawObj( inf->FindObjectAny(curCanv2PlotName.Data()) );
      c127->Update();
   }
}

void setEvtDisp(const Int_t id) {
   if (id==1) {
      curCanv1PlotTag = "hWaveform";
      curCanv1PlotDir = "Waveforms";
   } else if (id==2) {
      curCanv1PlotTag = "hShiftedPSWvfms";
      curCanv1PlotDir = "ShiftedPSWvfms";
   } else if (id==3) {
      curCanv1PlotTag = "hWvFreq";
      curCanv1PlotDir = "WvFrequencies";
   } else {
      Fatal("setEvtDisp","Unknown id [%d]",id);
   }
   redraw();
}

void canv2combo(const Char_t* text) {
   curCanv2PlotName = text;
   redraw();
}

void canv2combo(const TGLBEntry* sel) {
   if (sel!=0) {
      canv2combo(sel->GetTitle());
   }
}

void stopTab(const Int_t tb) {
   if        (tb==kLoc) {
      canv2combo(fComboBox655->GetSelectedEntry());
   } else if (tb==kLen) {
      canv2combo(fComboBox660->GetSelectedEntry());
   } else if (tb==kSadc) {
      canv2combo(fComboBox665->GetSelectedEntry());
   } else if (tb==kOffset) {
      canv2combo(fComboBox670->GetSelectedEntry());
   }
}

void stopCandTab(const Int_t tb) {
   if        (tb==kLoc) {
      canv2combo(fComboBox755->GetSelectedEntry());
   } else if (tb==kLen) {
      canv2combo(fComboBox760->GetSelectedEntry());
   } else if (tb==kSadc) {
      canv2combo(fComboBox765->GetSelectedEntry());
   } else if (tb==kOffset) {
      canv2combo(fComboBox770->GetSelectedEntry());
   } else if (tb==kNcand) {
      canv2combo(fComboBox775->GetSelectedEntry());
   }
}

void canv2tab(const Int_t tb) {
   if        (tb==k2D) {
      canv2combo(fComboBox560->GetSelectedEntry());
   } else if (tb==kPed) {
      canv2combo(fComboBox563->GetSelectedEntry());
   } else if (tb==kADC) {
      canv2combo(fComboBox565->GetSelectedEntry());
   } else if (tb==kAveFreq) {
      canv2combo(fComboBox569->GetSelectedEntry());
   } else if (tb==kStops) {
      stopTab(fTab650->GetCurrent());
   } else if (tb==kStopCands) {
      stopCandTab(fTab660->GetCurrent());
   }
}


TObjArray* NewListOfObjsWithTag(const Char_t* tag, const TDirectory* dir) {
   TObjArray* names = 0;
   if (dir!=0) {
      TList* keys = dir->GetListOfKeys();
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

void loadTheseComboEntries(const TObjArray* names, TGComboBox* cbox) {
   if ((cbox!=0) && (names!=0)) {
      cbox->RemoveAll();
      TObject* n;
      const Int_t nents = names->GetEntries();
      for (Int_t i=0; i<nents; i++) {
         n = names->At(i);
         cbox->AddEntry(n->GetName(), i);
      }
   }
}

void loadComboEntries(TGComboBox* cbox, const ETab_t tb) {
   if (cbox!=0) {
      TObjArray* names=0;
      switch (tb) {
         case k2D:
            names = NewListOfObjsWithTag("hSampVsEvent",inf);
            break;
         case kPed:
            names = NewListOfObjsWithTag("gPedAndRmsVsSamp",inf);
            break;
         case kADC:
            names = NewListOfObjsWithTag("hADCdistOfSample",
                                         inf->GetDirectory("ADCdistOfSample"));
            break;
         case kAveFreq:
            names = NewListOfObjsWithTag("hAveFreq",inf);
            break;
         default:
            Error("loadComboEntries","Unknown tab type [%d]",
               static_cast<Int_t>(tb));
      };
      loadTheseComboEntries(names, cbox);
      delete names;
   }
}

void loadStopComboEntries(TGComboBox* cbox, const EStopTab_t tb) {
   if (cbox!=0) {
      TObjArray* names=0;
      switch (tb) {
         case kLoc:
            names = NewListOfObjsWithTag("hStopLoc",
                                         inf->GetDirectory("StopLocs"));
            break;
         case kLen:
            names = NewListOfObjsWithTag("hStopLengths",
                                         inf->GetDirectory("StopLengths"));
            break;
         case kSadc:
            names = NewListOfObjsWithTag("hStopADCs",
                                         inf->GetDirectory("StopADCs"));
            break;
         case kOffset:
            names = NewListOfObjsWithTag("hStopOffsets",
                                         inf->GetDirectory("StopOffsets"));
            break;
         default:
            Error("loadStopComboEntries","Unknown tab type [%d]",
               static_cast<Int_t>(tb));
      };
      loadTheseComboEntries(names, cbox);
      delete names;
   }
}

void loadStopCandComboEntries(TGComboBox* cbox, const EStopTab_t tb) {
   if (cbox!=0) {
      TObjArray* names=0;
      switch (tb) {
         case kLoc:
            names = NewListOfObjsWithTag("hStopLocCand",
                                         inf->GetDirectory("StopLocCands"));
            break;
         case kLen:
            names = NewListOfObjsWithTag("hStopLengthCand",
                                         inf->GetDirectory("StopLengthCands"));
            break;
         case kSadc:
            names = NewListOfObjsWithTag("hStopCandADCs",
                                         inf->GetDirectory("StopCandADCs"));
            break;
         case kOffset:
            names = NewListOfObjsWithTag("hStopCandOffsets",
                                         inf->GetDirectory("StopCandOffsets"));
            break;
         case kNcand:
            names = NewListOfObjsWithTag("hNumStops",
                                         inf->GetDirectory("NumStops"));
            break;
         default:
            Error("loadStopCandComboEntries","Unknown tab type [%d]",
               static_cast<Int_t>(tb));
      };
      loadTheseComboEntries(names, cbox);
      delete names;
   }
}

void numEntryChgd(Long_t) {
   const Int_t e = fNumberEntry844->GetIntNumber();
   if ( (e>-1) && (e<=topEvt) ) {
      curEvt = e;
      redraw();
   }
}

void decreaseCurEvt() {
   const Int_t e = curEvt-1;
   if ( (e>-1) && (e<=topEvt) ) {
      fNumberEntry844->SetNumber(e);
      fNumberEntry844->ValueSet(1);
   }
}

void increaseCurEvt() {
   const Int_t e = curEvt+1;
   if ( (e>-1) && (e<=topEvt) ) {
      fNumberEntry844->SetNumber(e);
      fNumberEntry844->ValueSet(1);
   }
}

void findtopEvt() {
   if (wvdir!=0) {
      TList* keys = wvdir->GetListOfKeys();
      TObject* k;
      TIter nk(keys);
      Int_t ch, ev;
      while ( (k=nk())!=0 ) {
         sscanf(k->GetName(),"hWaveform_ch%02d_ev%06d",&ch,&ev);
         if (ev > topEvt) {
            topEvt = ev;
         }
      }
   }
}

void openFile(const Char_t* infn) {
   inf = TFile::Open(infn);
   if ( (inf==0) || (inf->IsZombie()) ) {
      Error("testGui","Could not open [%s]",infn);
      return;
   } else {
      tsdir = inf->GetDirectory("Timestamps");
      wvdir = inf->GetDirectory("Waveforms");
      dndir = inf->GetDirectory("DAQEvtNums");
      tgdir = inf->GetDirectory("TrigBits");
   }
}

void showEventGui(const Char_t* infn=
                  "/home/cjreed/work/temp/hists.temp.ev_All.dat.gz.root") {
   
   TH1::AddDirectory(kFALSE);
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   
   openFile(infn);
   
   curEvt = 0;
   curCanv1PlotTag = "hWaveform";
   curCanv1PlotDir = "Waveforms";
   
   findtopEvt();

   TString infnstr(infn);
   
   // main frame
   fMainFrame1172 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame1172->SetName("fMainFrame1172");
   fMainFrame1172->SetWindowName(infnstr(infnstr.Last('/')+1,
                                         infnstr.Length()-infnstr.Last('/')-1)
                                         .Data());
   fMainFrame1172->SetLayoutBroken(kTRUE);
   
   fCheckButtons[0] = new TGCheckButton(fMainFrame1172,"Channel 0");
   fCheckButtons[0]->SetTextJustify(36);
   fCheckButtons[0]->SetMargins(0,0,0,0);
   fCheckButtons[0]->SetWrapLength(-1);
   fCheckButtons[0]->SetOn();
   fCheckButtons[0]->Connect("Toggled(Bool_t)", 0,0, "redraw()");
   fMainFrame1172->AddFrame(fCheckButtons[0], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButtons[0]->MoveResize(528,24,112,24);
   
   fCheckButtons[1] = new TGCheckButton(fMainFrame1172,"Channel 1");
   fCheckButtons[1]->SetTextJustify(36);
   fCheckButtons[1]->SetMargins(0,0,0,0);
   fCheckButtons[1]->SetWrapLength(-1);
   fCheckButtons[1]->SetOn();
   fCheckButtons[1]->Connect("Toggled(Bool_t)", 0,0, "redraw()");
   fMainFrame1172->AddFrame(fCheckButtons[1], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButtons[1]->MoveResize(648,24,112,24);
   
   fCheckButtons[2] = new TGCheckButton(fMainFrame1172,"Channel 2");
   fCheckButtons[2]->SetTextJustify(36);
   fCheckButtons[2]->SetMargins(0,0,0,0);
   fCheckButtons[2]->SetWrapLength(-1);
   fCheckButtons[2]->SetOn();
   fCheckButtons[2]->Connect("Toggled(Bool_t)", 0,0, "redraw()");
   fMainFrame1172->AddFrame(fCheckButtons[2], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButtons[2]->MoveResize(528,48,112,24);
   
   fCheckButtons[3] = new TGCheckButton(fMainFrame1172,"Channel 3");
   fCheckButtons[3]->SetTextJustify(36);
   fCheckButtons[3]->SetMargins(0,0,0,0);
   fCheckButtons[3]->SetWrapLength(-1);
   fCheckButtons[3]->SetOn();
   fCheckButtons[3]->Connect("Toggled(Bool_t)", 0,0, "redraw()");
   fMainFrame1172->AddFrame(fCheckButtons[3], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCheckButtons[3]->MoveResize(648,48,112,24);
   
   for (Int_t i=0; i<4; i++) {
      fCheckButtons[i]->SetForegroundColor(TColor::Number2Pixel(col[i]));
      fCheckButtons[i]->SetLayoutBroken(kTRUE);
   }
   
   fRadioButtonGroup1 = new TGHButtonGroup(fMainFrame1172,"");
   fRadioButtonGroup1->Connect("Clicked(Int_t)",0,0,"setEvtDisp(Int_t)");
   fMainFrame1172->AddFrame(fRadioButtonGroup1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRadioButtonGroup1->MoveResize(500,70,320,100);
   
   fRadioButtons100 = new TGRadioButton(fRadioButtonGroup1,"Waveforms");
   fRadioButtons100->SetTextJustify(36);
   fRadioButtons100->SetMargins(0,0,0,0);
   fRadioButtons100->SetWrapLength(-1);
   fRadioButtons100->SetOn();
   fRadioButtons100->MoveResize(5,5,90,24);
   
   fRadioButtons110 = new TGRadioButton(fRadioButtonGroup1,"Calibrated");
   fRadioButtons110->SetTextJustify(36);
   fRadioButtons110->SetMargins(0,0,0,0);
   fRadioButtons110->SetWrapLength(-1);
   fRadioButtons110->MoveResize(195,5,90,24);
   
   fRadioButtons105 = new TGRadioButton(fRadioButtonGroup1,"Frequencies");
   fRadioButtons105->SetTextJustify(36);
   fRadioButtons105->SetMargins(0,0,0,0);
   fRadioButtons105->SetWrapLength(-1);
   fRadioButtons105->MoveResize(100,5,90,24);
   
   fLabel769 = new TGLabel(fMainFrame1172,"Event # / #");
   fLabel769->SetTextJustify(kTextCenterX | kTextCenterY);
   fLabel769->SetMargins(0,0,0,0);
   fLabel769->SetWrapLength(-1);
   fMainFrame1172->AddFrame(fLabel769, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel769->MoveResize(480,110,300,20);
   
   fTimeLabel = new TGLabel(fMainFrame1172,"UTC Time");
   fTimeLabel->SetTextJustify(kTextCenterX | kTextCenterY);
   fTimeLabel->SetMargins(0,0,0,0);
   fTimeLabel->SetWrapLength(-1);
   fMainFrame1172->AddFrame(fTimeLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTimeLabel->MoveResize(475,250,340,20);
   
   fTrgLabel = new TGLabel(fMainFrame1172,"Trigger Type N/A");
   fTrgLabel->SetTextJustify(kTextCenterX | kTextCenterY);
   fTrgLabel->SetMargins(0,0,0,0);
   fTrgLabel->SetWrapLength(-1);
   fMainFrame1172->AddFrame(fTrgLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTrgLabel->MoveResize(475,275,340,20);
   
   fNumberEntry844 = new TGNumberEntry(fMainFrame1172, (Double_t) 0,42,-1,TGNumberFormat::kNESInteger);
   fNumberEntry844->SetName("fNumberEntry844");
   fNumberEntry844->Connect("ValueSet(Long_t)",0,0,"numEntryChgd(Long_t)");
   fNumberEntry844->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, topEvt);
   fMainFrame1172->AddFrame(fNumberEntry844, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry844->MoveResize(504,135,288,20);
   
   fTextButton980 = new TGTextButton(fMainFrame1172,"<< Previous");
   fTextButton980->SetTextJustify(36);
   fTextButton980->SetMargins(0,0,0,0);
   fTextButton980->SetWrapLength(-1);
   fTextButton980->Resize(120,56);
   fTextButton980->Connect("Released()",0,0,"decreaseCurEvt()");
   fMainFrame1172->AddFrame(fTextButton980, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton980->MoveResize(520,178,120,56);
   
   fTextButton995 = new TGTextButton(fMainFrame1172,"Next >>");
   fTextButton995->SetTextJustify(36);
   fTextButton995->SetMargins(0,0,0,0);
   fTextButton995->SetWrapLength(-1);
   fTextButton995->Resize(120,56);
   fTextButton995->Connect("Released()",0,0,"increaseCurEvt()");
   fMainFrame1172->AddFrame(fTextButton995, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton995->MoveResize(656,178,120,56);

   fTextButton764 = new TGTextButton(fMainFrame1172,"Redraw");
   fTextButton764->SetTextJustify(36);
   fTextButton764->SetMargins(0,0,0,0);
   fTextButton764->SetWrapLength(-1);
   fTextButton764->Resize(288,128);
   fTextButton764->Connect("Released()", 0,0, "redraw()");
   fMainFrame1172->AddFrame(fTextButton764, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton764->MoveResize(504,498,288,78);
   
   // embedded canvas
   fRootEmbeddedCanvas774 = new TRootEmbeddedCanvas(0,fMainFrame1172,448,296);
   fRootEmbeddedCanvas774->SetName("fRootEmbeddedCanvas774");
   const Int_t wfRootEmbeddedCanvas774 = fRootEmbeddedCanvas774->GetCanvasWindowId();
   c126 = new TCanvas("c126", 10, 10, wfRootEmbeddedCanvas774);
   fRootEmbeddedCanvas774->AdoptCanvas(c126);
   fMainFrame1172->AddFrame(fRootEmbeddedCanvas774, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRootEmbeddedCanvas774->MoveResize(8,8,448,296);

   // embedded canvas
   fRootEmbeddedCanvas794 = new TRootEmbeddedCanvas(0,fMainFrame1172,448,296);
   fRootEmbeddedCanvas794->SetName("fRootEmbeddedCanvas794");
   const Int_t wfRootEmbeddedCanvas794 = fRootEmbeddedCanvas794->GetCanvasWindowId();
   c127 = new TCanvas("c127", 10, 10, wfRootEmbeddedCanvas794);
   fRootEmbeddedCanvas794->AdoptCanvas(c127);
   fMainFrame1172->AddFrame(fRootEmbeddedCanvas794, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRootEmbeddedCanvas794->MoveResize(8,312,448,296);
   
   
   // tab widget
   fTab560 = new TGTab(fMainFrame1172,400,136);
   fTab560->Connect("Selected(Int_t)",0,0,"canv2tab(Int_t)");

   // container of "Tab1"
   fCompositeFrame563 = fTab560->AddTab("2D Data");
   fCompositeFrame563->SetLayoutManager(new TGVerticalLayout(fCompositeFrame563));

   // container of "Tab2"
   fCompositeFrame565 = fTab560->AddTab("Peds, RMS");
   fCompositeFrame565->SetLayoutManager(new TGVerticalLayout(fCompositeFrame565));
   fCompositeFrame565->SetLayoutBroken(kTRUE);
   
   // container of "Tab3"
   fCompositeFrame567 = fTab560->AddTab("ADCs");
   fCompositeFrame567->SetLayoutManager(new TGVerticalLayout(fCompositeFrame567));
   fCompositeFrame567->SetLayoutBroken(kTRUE);
   
   // container of "Tab4"
   fCompositeFrame569 = fTab560->AddTab("Ave Freqs");
   fCompositeFrame569->SetLayoutManager(new TGVerticalLayout(fCompositeFrame569));
   fCompositeFrame569->SetLayoutBroken(kTRUE);
   
   // container of "Tab5"
   fCompositeFrame571 = fTab560->AddTab("Stops");
   fCompositeFrame571->SetLayoutManager(new TGVerticalLayout(fCompositeFrame571));
   fCompositeFrame571->SetLayoutBroken(kTRUE);
   
   // container of "Tab6"
   fCompositeFrame573 = fTab560->AddTab("Stop Cands");
   fCompositeFrame573->SetLayoutManager(new TGVerticalLayout(fCompositeFrame573));
   fCompositeFrame573->SetLayoutBroken(kTRUE);
   
   fTab560->SetTab(0);
   fMainFrame1172->AddFrame(fTab560, new TGLayoutHints(kLHintsExpandX | kLHintsLeft | kLHintsTop,2,2,2,2));
   fTab560->MoveResize(465,350,355,140);
   
   // tabs inside tab5
   fTab650 = new TGTab(fCompositeFrame571,325,106);
   fTab650->Connect("Selected(Int_t)",0,0,"stopTab(Int_t)");
   
   fCompositeFrame655 = fTab650->AddTab("Location");
   fCompositeFrame655->SetLayoutManager(new TGVerticalLayout(fCompositeFrame655));
   fCompositeFrame655->SetLayoutBroken(kTRUE);
   
   fCompositeFrame660 = fTab650->AddTab("Length");
   fCompositeFrame660->SetLayoutManager(new TGVerticalLayout(fCompositeFrame660));
   fCompositeFrame660->SetLayoutBroken(kTRUE);
   
   fCompositeFrame665 = fTab650->AddTab("Stop ADCs");
   fCompositeFrame665->SetLayoutManager(new TGVerticalLayout(fCompositeFrame665));
   fCompositeFrame665->SetLayoutBroken(kTRUE);
   
   fCompositeFrame670 = fTab650->AddTab("Offsets");
   fCompositeFrame670->SetLayoutManager(new TGVerticalLayout(fCompositeFrame670));
   fCompositeFrame670->SetLayoutBroken(kTRUE);
   
   fTab650->SetTab(0);
   fCompositeFrame571->AddFrame(fTab650, new TGLayoutHints(kLHintsExpandX | kLHintsLeft | kLHintsTop,2,2,2,2));
   fTab650->MoveResize(10,10,325,100);
   
   // tabs inside tab6
   fTab660 = new TGTab(fCompositeFrame573,325,106);
   fTab660->Connect("Selected(Int_t)",0,0,"stopCandTab(Int_t)");
   
   fCompositeFrame755 = fTab660->AddTab("Location");
   fCompositeFrame755->SetLayoutManager(new TGVerticalLayout(fCompositeFrame755));
   fCompositeFrame755->SetLayoutBroken(kTRUE);
   
   fCompositeFrame760 = fTab660->AddTab("Length");
   fCompositeFrame760->SetLayoutManager(new TGVerticalLayout(fCompositeFrame760));
   fCompositeFrame760->SetLayoutBroken(kTRUE);
   
   fCompositeFrame765 = fTab660->AddTab("Stop ADCs");
   fCompositeFrame765->SetLayoutManager(new TGVerticalLayout(fCompositeFrame765));
   fCompositeFrame765->SetLayoutBroken(kTRUE);
   
   fCompositeFrame770 = fTab660->AddTab("Offsets");
   fCompositeFrame770->SetLayoutManager(new TGVerticalLayout(fCompositeFrame770));
   fCompositeFrame770->SetLayoutBroken(kTRUE);
   
   fCompositeFrame775 = fTab660->AddTab("# Candidates");
   fCompositeFrame775->SetLayoutManager(new TGVerticalLayout(fCompositeFrame775));
   fCompositeFrame775->SetLayoutBroken(kTRUE);
   
   fTab660->SetTab(0);
   fCompositeFrame573->AddFrame(fTab660, new TGLayoutHints(kLHintsExpandX | kLHintsLeft | kLHintsTop,2,2,2,2));
   fTab660->MoveResize(10,10,325,100);
   
   
   // combo boxes
   const Int_t cbox[4] = { 40, 45, 250, 20 };
   fComboBox560 = new TGComboBox(fCompositeFrame563,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox560->SetName("fComboBox560");
   loadComboEntries(fComboBox560, k2D);
   fComboBox560->Resize(280,20);
   fComboBox560->Select(0);
   fComboBox560->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame563->AddFrame(fComboBox560, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox560->MoveResize(cbox[0],cbox[1],cbox[2],cbox[3]);
   
   fComboBox563 = new TGComboBox(fCompositeFrame565,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox563->SetName("fComboBox563");
   loadComboEntries(fComboBox563, kPed);
   fComboBox563->Resize(280,20);
   fComboBox563->Select(0);
   fComboBox563->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame563->AddFrame(fComboBox563, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox563->MoveResize(cbox[0],cbox[1],cbox[2],cbox[3]);
   
   fComboBox565 = new TGComboBox(fCompositeFrame567,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox565->SetName("fComboBox565");
   loadComboEntries(fComboBox565, kADC);
   fComboBox565->Resize(280,20);
   fComboBox565->Select(0);
   fComboBox565->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame563->AddFrame(fComboBox565, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox565->MoveResize(cbox[0],cbox[1],cbox[2],cbox[3]);
   
   fComboBox569 = new TGComboBox(fCompositeFrame569,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox569->SetName("fComboBox565");
   loadComboEntries(fComboBox569, kAveFreq);
   fComboBox569->Resize(280,20);
   fComboBox569->Select(0);
   fComboBox569->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame563->AddFrame(fComboBox569, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox569->MoveResize(cbox[0],cbox[1],cbox[2],cbox[3]);
   
   // stop combo boxes
   const Int_t scbox[4] = { 20, 30, 250, 20 };
   fComboBox655 = new TGComboBox(fCompositeFrame655,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox655->SetName("fComboBox655");
   loadStopComboEntries(fComboBox655, kLoc);
   fComboBox655->Resize(280,20);
   fComboBox655->Select(0);
   fComboBox655->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame655->AddFrame(fComboBox655, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox655->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox660 = new TGComboBox(fCompositeFrame660,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox660->SetName("fComboBox660");
   loadStopComboEntries(fComboBox660, kLen);
   fComboBox660->Resize(280,20);
   fComboBox660->Select(0);
   fComboBox660->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame660->AddFrame(fComboBox660, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox660->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox665 = new TGComboBox(fCompositeFrame665,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox665->SetName("fComboBox665");
   loadStopComboEntries(fComboBox665, kSadc);
   fComboBox665->Resize(280,20);
   fComboBox665->Select(0);
   fComboBox665->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame665->AddFrame(fComboBox665, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox665->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox670 = new TGComboBox(fCompositeFrame670,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox670->SetName("fComboBox670");
   loadStopComboEntries(fComboBox670, kOffset);
   fComboBox670->Resize(280,20);
   fComboBox670->Select(0);
   fComboBox670->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame670->AddFrame(fComboBox670, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox670->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   // stop combo boxes
   fComboBox755 = new TGComboBox(fCompositeFrame755,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox755->SetName("fComboBox755");
   loadStopCandComboEntries(fComboBox755, kLoc);
   fComboBox755->Resize(280,20);
   fComboBox755->Select(0);
   fComboBox755->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame755->AddFrame(fComboBox755, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox755->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox760 = new TGComboBox(fCompositeFrame760,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox760->SetName("fComboBox760");
   loadStopCandComboEntries(fComboBox760, kLen);
   fComboBox760->Resize(280,20);
   fComboBox760->Select(0);
   fComboBox760->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame760->AddFrame(fComboBox760, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox760->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox765 = new TGComboBox(fCompositeFrame765,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox765->SetName("fComboBox765");
   loadStopCandComboEntries(fComboBox765, kSadc);
   fComboBox765->Resize(280,20);
   fComboBox765->Select(0);
   fComboBox765->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame765->AddFrame(fComboBox765, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox765->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox770 = new TGComboBox(fCompositeFrame770,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox770->SetName("fComboBox770");
   loadStopCandComboEntries(fComboBox770, kOffset);
   fComboBox770->Resize(280,20);
   fComboBox770->Select(0);
   fComboBox770->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame770->AddFrame(fComboBox770, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox770->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   fComboBox775 = new TGComboBox(fCompositeFrame775,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
   fComboBox775->SetName("fComboBox775");
   loadStopCandComboEntries(fComboBox775, kNcand);
   fComboBox775->Resize(280,20);
   fComboBox775->Select(0);
   fComboBox775->Connect("Selected(char*)",0,0,"canv2combo(Char_t*)");
   fCompositeFrame775->AddFrame(fComboBox775, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fComboBox775->MoveResize(scbox[0],scbox[1],scbox[2],scbox[3]);
   
   // set default
   canv2combo(fComboBox560->GetSelectedEntry()->GetTitle());
   
   fMainFrame1172->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame1172->MapSubwindows();

   fMainFrame1172->Resize(fMainFrame1172->GetDefaultSize());
   fMainFrame1172->MapWindow();
   fMainFrame1172->Resize(827,620);
   fMainFrame1172->Layout();
   
}  
