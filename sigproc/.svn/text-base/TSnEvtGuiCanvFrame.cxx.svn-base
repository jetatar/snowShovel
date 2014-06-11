#include "TSnEvtGuiCanvFrame.h"

#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>

#include "TSnGCanvasSaveDialog.h"
#include "TSnEvtGuiSeleFrame.h"
#include "NSnConstants.h"

ClassImp(TSnEvtGuiCanvFrame);

TSnEvtGuiCanvFrame::TSnEvtGuiCanvFrame(const TGWindow* p,
                                       const UInt_t w,
                                       const UInt_t h,
                                       const UInt_t options,
                                       const Pixel_t back) :
   TGVerticalFrame(p, w, h, options, back),
   fWaveCanv(0), fSaveCont(0), fMiscCanv(0),
   fSaveWvBtn(0), fSaveMsBtn(0) {
   
   if (p!=0) {
      fWaveCanv = new TRootEmbeddedCanvas("canvWave", this, 300, 200);
      fSaveCont = new TGHorizontalFrame(this, 300, 60);
      fMiscCanv = new TRootEmbeddedCanvas("miscWave", this, 300, 200);

      fSaveWvBtn = new TGTextButton(fSaveCont, "Save &Upper Canvas",
                                    kSaveWaveBtn);
      fSaveMsBtn = new TGTextButton(fSaveCont, "Save &Lower Canvas",
                                    kSaveMiscBtn);
   
      fSaveWvBtn->Connect("Clicked()", "TSnEvtGuiCanvFrame", this,
                          "HandleButtons()");
      fSaveMsBtn->Connect("Clicked()", "TSnEvtGuiCanvFrame", this,
                          "HandleButtons()");

      TGLayoutHints* bl = new TGLayoutHints(kLHintsTop | kLHintsLeft
                                            | kLHintsExpandX,
                                            5, 5, 5, 5);
   
      AddFrame(fWaveCanv, new TGLayoutHints(kLHintsTop | kLHintsLeft |
                                            kLHintsExpandX | kLHintsExpandY,
                                            5, 5, 5, 5));
      AddFrame(fSaveCont, bl);
      AddFrame(fMiscCanv, new TGLayoutHints(kLHintsBottom | kLHintsLeft |
                                            kLHintsExpandX | kLHintsExpandY,
                                            5, 5, 5, 5));
   
   
      fSaveCont->AddFrame(fSaveWvBtn, bl);
      fSaveCont->AddFrame(fSaveMsBtn, bl);
   
      MapSubwindows();
      Resize(w,h);
      MapWindow();
      Resize(w,h);
      Layout();
   }
}

TSnEvtGuiCanvFrame::~TSnEvtGuiCanvFrame() {
   DeleteWindow();
}

void TSnEvtGuiCanvFrame::HandleButtons(Int_t id) {
   if (-1==id) {
      TGButton* btn = reinterpret_cast<TGButton*>(gTQSender);
      if (btn!=0) {
         id = btn->WidgetId();
      }
   }
      
   if (-1==id) { // still
      Error("HandleButtons","Could not get button event sender.");
   } else {
      switch (id) {
         case kSaveWaveBtn:
            return MakeSaveDialog(fWaveCanv);
         case kSaveMiscBtn:
            return MakeSaveDialog(fMiscCanv);
         default:
            break;
      };
      Error("HandleButtons","Unknown button id [%d]",id);
   }
}

void TSnEvtGuiCanvFrame::MakeSaveDialog(TRootEmbeddedCanvas* canv) {
   
   new TSnGCanvasSaveDialog(canv);
   
}

TCanvas* TSnEvtGuiCanvFrame::GetWaveCanvas() {
   return (fWaveCanv!=0) ? (fWaveCanv->GetCanvas()) : 0;
}

TCanvas* TSnEvtGuiCanvFrame::GetMiscCanvas() {
   return (fMiscCanv!=0) ? (fMiscCanv->GetCanvas()) : 0;
}

