#ifndef SNS_TSnEvtGuiSeleFrame
#define SNS_TSnEvtGuiSeleFrame

#include <TGFrame.h>
#include <RQ_OBJECT.h>
class TGTextButton;
class TGCheckButton;
class TGRadioButton;
class TGButtonGroup;
class TGCompositeFrame;
class TGLabel;
class TGNumberEntry;
class TGTextButton;
class TGTab;
class TGComboBox;

#include "NSnConstants.h"
class TSnEvtGuiMainFrame;
class TSnEventHeader;
class TSnEventMetadata;

class TSnEvtGuiSeleFrame : public TGVerticalFrame {
   RQ_OBJECT("TSnEvtGuiSeleFrame");
   
 public:
   enum EButtonIds {
      kChan0Btn=300,
      kChan1Btn=301,
      kChan2Btn=302,
      kChan3Btn=303,
      kWaveBtns=330,
      kWaveAdcBtn=331,
      kWaveFpnSubBtn=332,
      kWaveAmpOutBtn=333,
      kWaveAdcFFTBtn=334,
      kWaveFpnSubFFTBtn=335,
      kWaveAmpOutFFTBtn=336,
      kEvtNumEntry=350,
      kEntryNumEntry=351,
      kEvtPrevBtn=360,
      kEvtNextBtn=361,
      kAdcCombo=370,       // combo order must be same as tab order
      kAdcFFTCombo=371,
      kFpnSubCombo=372,
      kFpnSubFFTCombo=373,
      kAmpOutCombo=374,
      kAmpOutFFTCombo=375
   };

 private:
   TGButtonGroup* fChanBtnGroup;                   // contains channel check btns
   TGCheckButton* fChanBtn[NSnConstants::kNchans]; // channel check buttons
   TGButtonGroup* fWaveBtnGroup;                   // contains wave radio btns
   TGRadioButton* fWaveAdcBtn;                     // ADC waveform radio btn
   TGRadioButton* fWaveFpnSubBtn;                  // FPN sub waveform radio btn
   TGRadioButton* fWaveAmpOutBtn;                  // calib waveform radio btn
   TGRadioButton* fWaveAdcFFTBtn;            // FFT of ADC waveform radio btn
   TGRadioButton* fWaveFpnSubFFTBtn;         // FFT of FPN sub waveform radio btn
   TGRadioButton* fWaveAmpOutFFTBtn;         // FFT of calib waveform radio btn
   TGCompositeFrame* fEvtLabelFrame;         // contains event number lbl & entry
   TGLabel*       fEvtLabel;                 // event number label
   TGLabel*       fEntryLabel;               // consecutive entry label
   TGLabel*       fEvtRangeLbl;              // event range label
   TGLabel*       fEntryRangeLbl;            // entry range label
   TGNumberEntry* fEvtNumEntry;              // event number input
   TGNumberEntry* fEntryNumEntry;            // consec entry number input
   TGCompositeFrame* fEvtNavBtnCont;         // holds the nav buttons
   TGButtonGroup* fEvtNavBtns;               // contains event navig. buttons
   TGTextButton*  fEvtPrevBtn;               // previous event button
   TGTextButton*  fEvtNextBtn;               // previous event button
   TGCompositeFrame* fLabelFrame;            // contains metadata labels
   TGLabel*       fTimeLabel;                // event time label
   TGLabel*       fTrgLabel;                 // trigger type label
   TGLabel*       fRunLabel;                 // run/seq label
   TGTab*         fMiscTabs;                 // tab container
   TGCompositeFrame* fAdcTab;                // adc 2d tab
   TGCompositeFrame* fAdcFFTTab;             // adc fft 2d tab
   TGCompositeFrame* fFpnSubTab;             // FpnSub 2d tab
   TGCompositeFrame* fFpnSubFFTTab;          // FpnSub fft 2d tab
   TGCompositeFrame* fAmpOutTab;             // AmpOut 2d tab
   TGCompositeFrame* fAmpOutFFTTab;          // AmpOut fft 2d tab
   TGComboBox*    fAdcCombo;                 // Adc 2d list
   TGComboBox*    fAdcFFTCombo;              // Adc fft 2d list
   TGComboBox*    fFpnSubCombo;              // FpnSub 2d list
   TGComboBox*    fFpnSubFFTCombo;           // FpnSub fft 2d list
   TGComboBox*    fAmpOutCombo;              // AmpOut 2d list
   TGComboBox*    fAmpOutFFTCombo;           // AmpOut fft 2d list
   
   static
   void SetTextBtnProps(TGTextButton* b);
   static
   void SetLabelTxtProps(TGLabel* l);

 public:
   TSnEvtGuiSeleFrame() {}
   TSnEvtGuiSeleFrame(TSnEvtGuiMainFrame* p,
                      const UInt_t w=60,
                      const UInt_t h=60,
                      const UInt_t options = kChildFrame,
                      const Pixel_t back = GetDefaultFrameBackground());
   virtual ~TSnEvtGuiSeleFrame();
   
   TGButtonGroup* GetChanButtons() { return fChanBtnGroup; }
   TGCheckButton* GetChanButton(const Int_t ch);
   TGButtonGroup* GetWaveButtons() { return fWaveBtnGroup; }
   TGComboBox*    GetAdcCombo() { return fAdcCombo; }
   TGComboBox*    GetAdcFFTCombo() { return fAdcFFTCombo; }
   TGComboBox*    GetFpnSubCombo() { return fFpnSubCombo; }
   TGComboBox*    GetFpnSubFFTCombo() { return fFpnSubFFTCombo; }
   TGComboBox*    GetAmpOutCombo() { return fAmpOutCombo; }
   TGComboBox*    GetAmpOutFFTCombo() { return fAmpOutFFTCombo; }
   TGTab*         GetMiscTabs() { return fMiscTabs; }
   
   const Char_t*  GetMiscPlotName();
   
   Int_t GetEventNum() const;
   Int_t GetEntryNum() const;

   void DisableTab(const Int_t comboid);
   void EnableTab(const Int_t comboid);
   void DisableWave(const Int_t bid);
   void EnableWave(const Int_t bid);
   void DisableChan(const Int_t ch);
   void EnableChan(const Int_t ch);
   void SetChanColor(const Int_t ch, const Int_t col);

   void SetEventLimits(const Double_t min, const Double_t max);
   void SetEntryLimits(const Double_t min, const Double_t max);
   
   void SetEventNum(const Long_t event, const Double_t maxevt);
   void SetEntryNum(const Long_t entry, const Double_t maxentry);
   void SetEvtEntNums(const Long_t event,
                      const Long_t entry,
                      const Double_t maxevt,
                      const Double_t maxentry);

   void UpdateLabels(const TSnEventHeader* hdr,
                     const TSnEventMetadata* meta);
   
   ClassDef(TSnEvtGuiSeleFrame, 1); // contains the selection stuff in the event gui
};

#endif // SNS_TSnEvtGuiSeleFrame
