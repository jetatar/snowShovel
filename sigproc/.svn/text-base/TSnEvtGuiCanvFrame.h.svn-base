#ifndef SNS_TSnEvtGuiCanvFrame
#define SNS_TSnEvtGuiCanvFrame

#include <TGFrame.h>
#include <RQ_OBJECT.h>
class TRootEmbeddedCanvas;
class TString;
class TCanvas;
class TGTextButton;

class TSnEvtGuiCanvFrame : public TGVerticalFrame {
   RQ_OBJECT("TSnEvtGuiCanvFrame");
   
 public:
   enum EButtonIds {
      kSaveWaveBtn = 110,
      kSaveMiscBtn = 115
   };

 private:
   TRootEmbeddedCanvas* fWaveCanv; // the single event waveform/fft canvas
   TGHorizontalFrame*  fSaveCont; // the save button container
   TRootEmbeddedCanvas* fMiscCanv; // the canvas for multi event plots
   
   TGTextButton*        fSaveWvBtn; // save the waveform canvas
   TGTextButton*        fSaveMsBtn; // save the multi event canvas
   
   void MakeSaveDialog(TRootEmbeddedCanvas* canv);

 public:
   TSnEvtGuiCanvFrame() : fWaveCanv(0), fSaveCont(0), fMiscCanv(0),
                          fSaveWvBtn(0), fSaveMsBtn(0) {}
   TSnEvtGuiCanvFrame(const TGWindow* p,
                      const UInt_t w=60,
                      const UInt_t h=60,
                      const UInt_t options = kChildFrame,
                      const Pixel_t back = GetDefaultFrameBackground());
   virtual ~TSnEvtGuiCanvFrame();
   
   TRootEmbeddedCanvas* GetWaveEmbedCanv() { return fWaveCanv; }
   TCanvas*             GetWaveCanvas();
   TCanvas*             GetMiscCanvas();
   
   void HandleButtons(Int_t id=-1);
   
   ClassDef(TSnEvtGuiCanvFrame, 1); // contains the canvases in the event gui
};

#endif // SNS_TSnEvtGuiCanvFrame
