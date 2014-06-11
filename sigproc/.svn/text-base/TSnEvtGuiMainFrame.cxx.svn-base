#include "TSnEvtGuiMainFrame.h"

#include <TApplication.h>
#include <TGClient.h>
#include <TGTab.h>

#include "TSnEvtGuiCanvFrame.h"
#include "TSnEvtGuiSeleFrame.h"
#include "TSnEvtGuiDataGuru.h"

ClassImp(TSnEvtGuiMainFrame);

TSnEvtGuiMainFrame::TSnEvtGuiMainFrame(TTree* tree,
                                       TFile* pfile,
                                       const UInt_t w,
                                       const UInt_t h) :
   TGMainFrame(gClient->GetRoot(), w, h, kHorizontalFrame),
   fCanvFrame(0), fSeleFrame(0) {

   fCanvFrame = new TSnEvtGuiCanvFrame(this);
   
   TGLayoutHints* fl = new TGLayoutHints(kLHintsTop | kLHintsLeft 
                                         | kLHintsExpandY | kLHintsExpandX,
                                         2,2,2,2);
   
   AddFrame(fCanvFrame, fl);

   fSeleFrame = new TSnEvtGuiSeleFrame(this);
   AddFrame(fSeleFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft 
                                          | kLHintsExpandY | kFixedWidth,
                                          2,2,2,2));

   fDataGuru = new TSnEvtGuiDataGuru(tree, pfile);
      
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      fSeleFrame->SetChanColor(ch, fDataGuru->GetColorOfChan(ch));
   }
   
   fSeleFrame->SetEventLimits(fDataGuru->GetMinEvtNum(),
                              fDataGuru->GetMaxEvtNum()-1);
   fSeleFrame->SetEntryLimits(fDataGuru->GetMinEntryNum(),
                              fDataGuru->GetMaxEntryNum()-1);
   
   fDataGuru->ReadAllWaveHists(fSeleFrame);
   fDataGuru->ReadAllMiscHists(fSeleFrame);

   fDataGuru->RedrawMisc(fCanvFrame->GetMiscCanvas(),
                         fSeleFrame);
   ChangeEntryNum(0);
   
   Connect("CloseWindow()", "TSnEvtGuiMainFrame", this,
           "CloseWindow()");
   
   // use hierarchical cleaning
   SetCleanup(kDeepCleanup);
   
   SetWindowName("Event Gui");
   MapSubwindows();
   
   Resize(w,h);
   MapWindow();
   Resize(w,h);
   Layout();
}

TSnEvtGuiMainFrame::~TSnEvtGuiMainFrame() {
   DeleteWindow();
}

void TSnEvtGuiMainFrame::CloseWindow() {
   gApplication->Terminate();
}

void TSnEvtGuiMainFrame::ChangeWaveChans(Int_t id) {
   // redraw the waveform canvas
   if ((fDataGuru!=0) && (fCanvFrame!=0) && (fSeleFrame!=0)) {
      fDataGuru->ChangeWaveChans(fCanvFrame->GetWaveCanvas(),
                                 fSeleFrame,
                                 id - TSnEvtGuiSeleFrame::kChan0Btn);

   }
}

void TSnEvtGuiMainFrame::RedrawWaveCanv() {
   if ((fDataGuru!=0) && (fCanvFrame!=0) && (fSeleFrame!=0)) {
      fDataGuru->RedrawWaveChans(fCanvFrame->GetWaveCanvas(),
                                 fSeleFrame);
   }   
}

void TSnEvtGuiMainFrame::ChangeWaveType(Int_t id) {
   if ((fDataGuru!=0) && (fCanvFrame!=0) && (fSeleFrame!=0)) {
      fDataGuru->ChangeWaveType(fCanvFrame->GetWaveCanvas(),
                                fSeleFrame,
                                id);
   }   
}

void TSnEvtGuiMainFrame::ChangeEvtNum(Long_t) {
   // the number sent in is not the number in the number entry box, annoyingly
   if ( (fDataGuru!=0) && (fSeleFrame!=0) ) {
      const Int_t e = fSeleFrame->GetEventNum();
      if ( (e>=fDataGuru->GetMinEvtNum()) && (e<fDataGuru->GetMaxEvtNum()) ) {
         Int_t ent(0);
         const Int_t evt(e);
         if ( fDataGuru->GetEntryNum(evt, ent) ) {
            fDataGuru->SetEvtNum(evt);
            fSeleFrame->SetEvtEntNums(evt, ent,
                                      fDataGuru->GetMaxEvtNum(),
                                      fDataGuru->GetMaxEntryNum());
            RedrawWaveCanv();
         }
      }
   }
}

void TSnEvtGuiMainFrame::ChangeEntryNum(Long_t) {
   // the number sent in is not the number in the number entry box, annoyingly
   if ( (fDataGuru!=0) && (fSeleFrame!=0) ) {
      const Int_t entry = fSeleFrame->GetEntryNum();
      if ( (entry>=fDataGuru->GetMinEntryNum()) 
           && (entry<fDataGuru->GetMaxEntryNum()) ) {
         Int_t evt(0);
         const Int_t ent(entry);
         if (fDataGuru->GetEventNum(ent, evt)) {
            fDataGuru->SetEntryNum(ent);
            fSeleFrame->SetEvtEntNums(evt, ent,
                                      fDataGuru->GetMaxEvtNum(),
                                      fDataGuru->GetMaxEntryNum());
            RedrawWaveCanv();
         }
      }
   }
}

void TSnEvtGuiMainFrame::IncrementEvent(Int_t id) {
   if ((fSeleFrame!=0) && (fDataGuru!=0)) {
      Bool_t ok = kTRUE;
      Int_t newevt(fSeleFrame->GetEventNum());
      switch (id) {
         case TSnEvtGuiSeleFrame::kEvtPrevBtn:
            --newevt;
            break;
         case TSnEvtGuiSeleFrame::kEvtNextBtn:
            ++newevt;
            break;
         default:
            ok = kFALSE;
            break;
      }
      if (ok) {
         if ( (newevt>=fDataGuru->GetMinEvtNum())
              && (newevt<fDataGuru->GetMaxEvtNum()) ) {
            fSeleFrame->SetEventNum(newevt, fDataGuru->GetMaxEvtNum());
            ChangeEvtNum(1);
         }
      } else {
         Error("IncrementEvent","Unexpceted button id [%d]",id);
      }
   }
}

void TSnEvtGuiMainFrame::ChangeTab(Int_t id) {
   if (fSeleFrame!=0) {
      SelMiscPlot("");
   }
}

void TSnEvtGuiMainFrame::SelMiscPlot(const Char_t* nm) {
   if ((fDataGuru!=0) && (fCanvFrame!=0) && (fSeleFrame!=0)) {
      fDataGuru->RedrawMisc(fCanvFrame->GetMiscCanvas(),
                            fSeleFrame);
   }
}

