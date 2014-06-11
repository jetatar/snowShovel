#ifndef SNS_TSnPlotWaveformsMod
#define SNS_TSnPlotWaveformsMod

class TObjArray;
class TH1F;
class TH2F;
class TGraph;

#include "TAModule.h"
class TSnEventHeader;
class TSnRawWaveform;
class TSnCalWvData;

class TSnPlotWaveformsMod : public TAModule {
 private:
   // parameters (plot names currently fixed)
   Int_t      fNevtBins;         //  number of bins on the event axis
   Float_t    fEvtMin;           //  min event number of the event axis
   Float_t    fEvtMax;           //  max event number of the event axis
   Bool_t     fDoAdcPlots;       //  whether to make raw waveform plots (def: true)
   Bool_t     fDoFpnSubPlots;    //  whether to make FPN sub plots (def: true)
   Bool_t     fDoAmpOutPlots;    //  whether to make calibrated plots (def: true)

   // event objects
   TSnEventHeader* fHdr;         //! the event header
   TSnRawWaveform* fAdc;         //! the raw data
   TSnCalWvData*   fFpnSub;      //! the FPN sub data
   TSnCalWvData*   fAmpOut;      //! the calibrated data
   
   // output plots ...
   
   // made during Process
   TH1F*      fEvtsPerBin;       //! number of events in each event bin
   TObjArray* fAdcVsEvt;         //! [chan] raw waveform vs event for each chan
   TObjArray* fFpnSubVsEvt;      //! [chan] fpn sub waveform vs event for each chan
   TObjArray* fFpnSubErrVsEvt;   //! [chan] error on fpn sub vs event for each chan
   TObjArray* fAmpOutVsEvt;      //! [chan] calib waveform vs event for each chan
   TObjArray* fAmpOutErrVsEvt;   //! [chan] error on calib vs event for each chan

   // made at the end in terminate
   TObjArray* fAdcFFTVsEvt;      //! [chan] FFT of raw waveform vs event for each chan
   TObjArray* fFpnSubFFTVsEvt;   //! [chan] FFT of fpn sub waveform vs event for each chan
   TObjArray* fAmpOutFFTVsEvt;   //! [chan] FFT of calib waveform vs event for each chan
   TObjArray* fAdcAveFFT;        //! [chan] average FFT of raw waveform for each chan
   TObjArray* fFpnSubAveFFT;     //! [chan] average FFT of FPN sub waveform for each chan
   TObjArray* fAmpOutAveFFT;     //! [chan] average FFT of calib waveform for each chan
   TGraph*    fEntryVsEvt;       //! maps from event number to entry number
   TObjArray* fAdcVsEntry;       //! [chan] raw waveform vs entry for each chan
   TObjArray* fFpnSubVsEntry;    //! [chan] fpn sub waveform vs entry for each chan
   TObjArray* fFpnSubErrVsEntry; //! [chan] error on fpn sub vs entry for each chan
   TObjArray* fAmpOutVsEntry;    //! [chan] calib waveform vs entry for each chan
   TObjArray* fAmpOutErrVsEntry; //! [chan] error on calib vs entry for each chan
   TObjArray* fAdcFFTVsEntry;    //! [chan] FFT of raw waveform vs entry for each chan
   TObjArray* fFpnSubFFTVsEntry; //! [chan] FFT of fpn sub waveform vs entry for each chan
   TObjArray* fAmpOutFFTVsEntry; //! [chan] FFT of calib waveform vs entry for each chan


   void FillFFTHists(TH2F& hfftVsEvt, TH2F& hfftVsEntry,
                     TH1F& hfftAve,
                     const Double_t evtnum,
                     const Int_t entry,
                     Float_t* const data);

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   virtual void Terminate();

 public:
   TSnPlotWaveformsMod();
   TSnPlotWaveformsMod(const Char_t* name,
                       const Int_t nevtbins,
                       const Float_t evtmin, const Float_t evtmax);
   virtual ~TSnPlotWaveformsMod();
   
   Int_t     GetNevtBins() const { return fNevtBins; }
   Float_t   GetEvtMin() const { return fEvtMin; }
   Float_t   GetEvtMax() const { return fEvtMax; }
   Bool_t    IsMakingAdcPlots() const { return fDoAdcPlots; }
   Bool_t    IsMakingFpnSubPlots() const { return fDoFpnSubPlots; }
   Bool_t    IsMakingAmpOutPlots() const { return fDoAmpOutPlots; }
   
   void      SetNevtBins(const Int_t n) { fNevtBins = n; }
   void      SetEvtMin(const Float_t m) { fEvtMin = m; }
   void      SetEvtMax(const Float_t m) { fEvtMax = m; }
   void      SetMakeAdcPlots(const Bool_t b) { fDoAdcPlots = b; }
   void      SetMakeFpnSubPlots(const Bool_t b) { fDoFpnSubPlots = b; }
   void      SetMakeAmpOutPlots(const Bool_t b) { fDoAmpOutPlots = b; }
   
   // at least for now, the plot names are not settable parameters
   static
   const Char_t* GetEvtsPerBinName() { return "hEvtsPerBin"; }
   static
   const Char_t* GetAdcVsEvtName() { return "hAdcVsEvt"; }
   static
   const Char_t* GetFpnSubVsEvtName() { return "hFpnSubVsEvt"; }
   static
   const Char_t* GetFpnSubErrVsEvtName() { return "hFpnSubErrVsEvt"; }
   static
   const Char_t* GetAmpOutVsEvtName() { return "hAmpOutVsEvt"; }
   static
   const Char_t* GetAmpOutErrVsEvtName() { return "hAmpOutErrVsEvt"; }
   static
   const Char_t* GetAdcFFTVsEvtName() { return "hAdcFFTVsEvt"; }
   static
   const Char_t* GetFpnSubFFTVsEvtName() { return "hFpnSubFFTVsEvt"; }
   static
   const Char_t* GetAmpOutFFTVsEvtName() { return "hAmpOutFFTVsEvt"; }
   static
   const Char_t* GetAdcAveFFTName() { return "hAdcAveFFT"; }
   static
   const Char_t* GetFpnSubAveFFTName() { return "hFpnSubAveFFT"; }
   static
   const Char_t* GetAmpOutAveFFTName() { return "hAmpOutAveFFT"; }
   static
   const Char_t* GetEntryVsEvtName() { return "gEntryVsEvt"; }
   static
   const Char_t* GetAdcVsEntryName() { return "hAdcVsEntry"; }
   static
   const Char_t* GetFpnSubVsEntryName() { return "hFpnSubVsEntry"; }
   static
   const Char_t* GetFpnSubErrVsEntryName() { return "hFpnSubErrVsEntry"; }
   static
   const Char_t* GetAmpOutVsEntryName() { return "hAmpOutVsEntry"; }
   static
   const Char_t* GetAmpOutErrVsEntryName() { return "hAmpOutErrVsEntry"; }
   static
   const Char_t* GetAdcFFTVsEntryName() { return "hAdcFFTVsEntry"; }
   static
   const Char_t* GetFpnSubFFTVsEntryName() { return "hFpnSubFFTVsEntry"; }
   static
   const Char_t* GetAmpOutFFTVsEntryName() { return "hAmpOutFFTVsEntry"; }
   
   
   ClassDef(TSnPlotWaveformsMod, 1); // make waveform plots
};

#endif // SNS_TSnPlotWaveformsMod
