#ifndef SNS_TSnPlotAdcDists
#define SNS_TSnPlotAdcDists

#include "TString.h"
class TObjArray;
class TH1F;
class TH2;

#include "TAModule.h"
class TSnEventHeader;
class TSnRawWaveform;
class TSnCalWvData;

class TSnPlotAdcDists : public TAModule {
 private:
   TString       fPlotTag;       // a string to put in hist titles (i.e. describing the trigger or event selection)
   TString       fPlotLbl;       // a short label to add to the hist title (e.g. "Frc" for forced trigger)
   TString       fDataBrNm;      // name of the data branch (default: TSnRawTreeMaker::kEWvBrNm)
   Int_t         fNtimeBins;     // number of bins along time axis
   Double_t      fTimeMin;       // minimum of time axis (unix time seconds)
   Double_t      fTimeMax;       // maximum of time axis (unix time seconds)
   Int_t         fNAdcBins;      // number of bins along adc axis (def: 2048)
   Float_t       fAdcMin;        // minimum of adc axis (def: -1)
   Float_t       fAdcMax;        // maximum of adc axis (def: 4095)
   Int_t         fNFracBins;     // number of fraction bins (def: 100)
   Float_t       fFracMin;       // minimum of fraction axis (def: 0)
   Float_t       fFracMax;       // minimum of fraction axis (def: 1)
   Bool_t        fDoFFTPlots;    // whether to make FFT plots (def: true, but not done for raw data)
   Float_t       fLowFreqCut;    // inclusive frequency defining the highest "low freq" bin (def: 200MHz)
   Bool_t        fNormTimeBins;  // whether to normalize time bins or not (def: true)
   Bool_t        fDoSmpPlots;    // whether to make plots for each sample (def: false; this takes a lot of RAM!)
   Bool_t        fIsRaw;         // whether to read a TSnRawWaveform or a TSnCalWvData (def: true)
   
   // event objects
   TSnEventHeader* fHdr;         //! the event header
   TSnRawWaveform* fAdc;         //! the raw data
   TSnCalWvData*   fFpn;         //! the Cal/FPN sub data
   
   // output plots
   TH1F*      fEvtsPerBin;       //! number of events in each time bin
   TObjArray* fhAdc;             //! [chan] amplitude dist 
   TObjArray* fhAdcVsT;          //! [chan] amplitude dist vs time 
   TObjArray* fhFftVsT;          //! [chan] FFT vs time 
   TObjArray* fhMaxFrqVsT;       //! [chan] max freq vs time 
   TObjArray* fhLowPwrVsT;       //! [chan] low power ratio vs time 
   TObjArray* fhSmpVsT;          //! [chan][samp] sample dist vs time 
   TObjArray* fhAmpl;            //! [chan] abs val of max sample in wvfm
   TObjArray* fhAmplVsT;         //! [chan] abs val of max sample in wvfm vs time
   TObjArray* fhMean;            //! [chan] mean of waveform
   TObjArray* fhMeanVsT;         //! [chan] mean of waveform vs time
   TObjArray* fhRms;             //! [chan] rms of waveform
   TObjArray* fhRmsVsT;          //! [chan] rms of waveform vs time
   
   
   void NormalizeTimeColumns(TH2& h);

 protected:
   virtual void SlaveBegin();
   virtual void Process();
   virtual void Terminate();

 public:
   TSnPlotAdcDists();
   TSnPlotAdcDists(const Char_t* name,
                   const Int_t ntbins, 
                   const Double_t tmin, const Double_t tmax,
                   const Char_t* plottag, const Char_t* plotlbl,
                   const Char_t* dataBranchName=NULL);
   virtual ~TSnPlotAdcDists();
   
   TString  GetPlotTag() const          { return fPlotTag.Data(); }
   TString  GetPlotLbl() const          { return fPlotLbl.Data(); }
   TString  GetDataBrName() const       { return fDataBrNm; }
   Int_t    GetNtimeBins() const        { return fNtimeBins; }
   Double_t GetTimeMin() const          { return fTimeMin; }
   Double_t GetTimeMax() const          { return fTimeMax; }
   Int_t    GetNAdcBins() const         { return fNAdcBins; }
   Float_t  GetAdcMin() const           { return fAdcMin; }
   Float_t  GetAdcMax() const           { return fAdcMax; }
   Int_t    GetNFracBins() const        { return fNFracBins; }
   Float_t  GetFracMin() const          { return fFracMin; }
   Float_t  GetFracMax() const          { return fFracMax; }
   Float_t  GetLowFreqCut() const       { return fLowFreqCut; }
   Bool_t   IsMakingFFTPlots() const    { return fDoFFTPlots; }
   Bool_t   IsNormalizingTimeBins() const { return fNormTimeBins; }
   Bool_t   IsMakingSamplePlots() const { return fDoSmpPlots; }
   Bool_t   IsDataBrRaw() const         { return fIsRaw; }
   Bool_t   CanMakeFFTPlots() const    
      { return fDoFFTPlots && (fIsRaw==kFALSE); }
   
   void     SetPlotTag(const Char_t* t)  { fPlotTag = t; }
   void     SetPlotLbl(const Char_t* l)  { fPlotLbl = l; }
   void     SetDataBrName(const Char_t* n);
   void     SetNtimeBins(const Int_t nb) { fNtimeBins = nb; }
   void     SetTimeMin(const Double_t t) { fTimeMin = t; }
   void     SetTimeMax(const Double_t t) { fTimeMax = t; }
   void     SetNAdcBins(const Int_t n) { fNAdcBins = n; }
   void     SetAdcMin(const Int_t m) { fAdcMin = m; }
   void     SetAdcMax(const Int_t m) { fAdcMax = m; }
   void     SetLowFreqCut(const Float_t c) { fLowFreqCut = c; }
   void     SetMakeFFTPlots(const Bool_t m) { fDoFFTPlots = m; }
   void     SetNormalizeTimeBins(const Bool_t n) { fNormTimeBins = n; }
   void     SetMakeSamplePlots(const Bool_t s) { fDoSmpPlots = s; }
   void     SetIsDataBrRaw(const Bool_t r) { fIsRaw = r; }
   
   // the names of the plots are not settable (at least for now)
   static
   const Char_t* GetEvtsPerBinName() { return "hEvtsPerBin"; }
   static
   const Char_t* GetAdc() { return "hAdc"; }
   static
   const Char_t* GetAdcVsT() { return "hAdcVsT"; }
   static
   const Char_t* GetFftVsT() { return "hFftVsT"; }
   static
   const Char_t* GetMaxFrqVsT() { return "hMaxFrqVsT"; }
   static
   const Char_t* GetLowPwrVsT() { return "hLowPwrVsT"; }
   static
   const Char_t* GetSmpVsT() { return "hSmpVsT"; }
   static
   const Char_t* GetAmpl() { return "hAmpl"; }
   static
   const Char_t* GetAmplVsT() { return "hAmplVsT"; }
   static
   const Char_t* GetMean() { return "hMean"; }
   static
   const Char_t* GetMeanVsT() { return "hMeanVsT"; }
   static
   const Char_t* GetRms() { return "hRms"; }
   static
   const Char_t* GetRmsVsT() { return "hRmsVsT"; }
   
   
   ClassDef(TSnPlotAdcDists, 2); // make diagnostic ADC distributions
};

#endif // SNS_TSnPlotAdcDists
