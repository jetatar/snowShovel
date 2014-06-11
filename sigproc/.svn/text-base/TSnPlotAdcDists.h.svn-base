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
   Int_t         fNtimeBins;     // number of bins along time axis
   Double_t      fTimeMin;       // minimum of time axis (unix time seconds)
   Double_t      fTimeMax;       // maximum of time axis (unix time seconds)
   Int_t         fNRawAdcBins;   // number of bins along adc axis (def: 2048)
   Float_t       fRawAdcMin;     // minimum of adc axis (def: -1)
   Float_t       fRawAdcMax;     // maximum of adc axis (def: 4095)
   Int_t         fNFpnSubBins;   // number of bins along fpn sub axis (def: 1001)
   Float_t       fFpnSubMin;     // minimum of fpn sub axis (def: -1001)
   Float_t       fFpnSubMax;     // maximum of fpn sub axis (def:  1001)
   Int_t         fNFracBins;     // number of fraction bins (def: 100)
   Float_t       fFracMin;       // minimum of fraction axis (def: 0)
   Float_t       fFracMax;       // minimum of fraction axis (def: 1)
   Bool_t        fDoFpnSubPlots; // whether to make FPN sub plots (def: true)
   Float_t       fLowFreqCut;    // inclusive frequency defining the highest "low freq" bin (def: 200MHz)
   Bool_t        fNormTimeBins;  // whether to normalize time bins or not (def: true)
   Bool_t        fDoSmpPlots;    // whether to make plots for each sample (def: false; this takes a lot of RAM!)
   
   // event objects
   TSnEventHeader* fHdr;         //! the event header
   TSnRawWaveform* fAdc;         //! the raw data
   TSnCalWvData*   fFpn;         //! the FPN sub data
   
   // output plots
   TH1F*      fEvtsPerBin;       //! number of events in each time bin
   TObjArray* fRawAdc;           //! [chan] raw dist 
   TObjArray* fRawAdcVsT;        //! [chan] raw dist vs time 
   TObjArray* fFpnSub;           //! [chan] fpnsub dist 
   TObjArray* fFpnSubVsT;        //! [chan] fpnsub dist vs time 
   TObjArray* fFpnSubFftVsT;     //! [chan] fpnsub FFT vs time 
   TObjArray* fFpnSubMaxFrqVsT;  //! [chan] fpnsub max freq vs time 
   TObjArray* fFpnSubLowPwrVsT;  //! [chan] fpnsub low power ratio vs time 
   TObjArray* fRawAdcSmp;        //! [chan][samp] raw dist 
   TObjArray* fRawAdcSmpVsT;     //! [chan][samp] raw dist vs time 
   TObjArray* fFpnSubSmp;        //! [chan][samp] fpnsub dist 
   TObjArray* fFpnSubSmpVsT;     //! [chan][samp] fpnsub dist vs time 

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
                   const Char_t* plottag, const Char_t* plotlbl);
   virtual ~TSnPlotAdcDists();
   
   const Char_t* GetPlotTag() const     { return fPlotTag.Data(); }
   const Char_t* GetPlotLbl() const     { return fPlotLbl.Data(); }
   Int_t    GetNtimeBins() const        { return fNtimeBins; }
   Double_t GetTimeMin() const          { return fTimeMin; }
   Double_t GetTimeMax() const          { return fTimeMax; }
   Int_t    GetNRawAdcBins() const      { return fNRawAdcBins; }
   Float_t  GetRawAdcMin() const        { return fRawAdcMin; }
   Float_t  GetRawAdcMax() const        { return fRawAdcMax; }
   Int_t    GetNFpnSubBins() const      { return fNFpnSubBins; }
   Float_t  GetFpnSubMin() const        { return fFpnSubMin; }
   Float_t  GetFpnSubMax() const        { return fFpnSubMax; }
   Int_t    GetNFracBins() const        { return fNFracBins; }
   Float_t  GetFracMin() const          { return fFracMin; }
   Float_t  GetFracMax() const          { return fFracMax; }
   Float_t  GetLowFreqCut() const       { return fLowFreqCut; }
   Bool_t   IsMakingFpnSubPlots() const { return fDoFpnSubPlots; }
   Bool_t   IsNormalizingTimeBins() const { return fNormTimeBins; }
   Bool_t   IsMakingSamplePlots() const { return fDoSmpPlots; }
   
   void     SetPlotTag(const Char_t* t)  { fPlotTag = t; }
   void     SetPlotLbl(const Char_t* l)  { fPlotLbl = l; }
   void     SetNtimeBins(const Int_t nb) { fNtimeBins = nb; }
   void     SetTimeMin(const Double_t t) { fTimeMin = t; }
   void     SetTimeMax(const Double_t t) { fTimeMax = t; }
   void     SetNRawAdcBins(const Int_t n) { fNRawAdcBins = n; }
   void     SetRawAdcMin(const Int_t m) { fRawAdcMin = m; }
   void     SetRawAdcMax(const Int_t m) { fRawAdcMax = m; }
   void     SetNFpnSubBins(const Int_t n) { fNFpnSubBins = n; }
   void     SetFpnSubMin(const Int_t m) { fFpnSubMin = m; }
   void     SetFpnSubMax(const Int_t m) { fFpnSubMax = m; }
   void     SetLowFreqCut(const Float_t c) { fLowFreqCut = c; }
   void     SetMakeFpnSubPlots(const Bool_t m) { fDoFpnSubPlots = m; }
   void     SetNormalizeTimeBins(const Bool_t n) { fNormTimeBins = n; }
   void     SetMakeSamplePlots(const Bool_t s) { fDoSmpPlots = s; }
   
   // the names of the plots are not settable (at least for now)
   static
   const Char_t* GetEvtsPerBinName() { return "hEvtsPerBin"; }
   static
   const Char_t* GetRawAdc() { return "hRawAdc"; }
   static
   const Char_t* GetRawAdcVsT() { return "hRawAdcVsT"; }
   static
   const Char_t* GetFpnSub() { return "hFpnSub"; }
   static
   const Char_t* GetFpnSubVsT() { return "hFpnSubVsT"; }
   static
   const Char_t* GetFpnSubFftVsT() { return "hFpnSubFftVsT"; }
   static
   const Char_t* GetFpnSubMaxFrqVsT() { return "hFpnSubMaxFrqVsT"; }
   static
   const Char_t* GetFpnSubLowPwrVsT() { return "hFpnSubLowPwrVsT"; }
   static
   const Char_t* GetRawAdcSmp() { return "hRawAdcSmp"; }
   static
   const Char_t* GetRawAdcSmpVsT() { return "hRawAdcSmpVsT"; }
   static
   const Char_t* GetFpnSubSmp() { return "hFpnSubSmp"; }
   static
   const Char_t* GetFpnSubSmpVsT() { return "hFpnSubSmpVsT"; }
   
   
   
   ClassDef(TSnPlotAdcDists, 1); // make diagnostic ADC distributions
};

#endif // SNS_TSnPlotAdcDists
