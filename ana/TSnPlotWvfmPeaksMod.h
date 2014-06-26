#ifndef SNS_TSnPlotWvfmPeaksMod
#define SNS_TSnPlotWvfmPeaksMod

#include <TString.h>
class TObjArray;
class TNtuple;

#include "TAModule.h"

class TSnPlotWvfmPeaksMod : public TAModule {
 private:
   TString    fPkArNm;   // name of resulting TObjArray of TGraph's of peak positions to add to the event (default: DataPeaks)
   TString    fPlotSuf;  // suffix to append to name of plots (default: nothing)
   TString    fAmpUnits; // the amplitude units to use (ADC or mV) (no default)
   Bool_t     fVsTime;   // whether to make the xaxis time (true) or sample (false) (default: false)
   Bool_t     fMakePeakTree; // whether to make the peak properites ntuple (default: false)
   
   // binning
   Int_t      fNpkbins;  // num peak number bins
   Float_t    fNpkmin;   // peak number min
   Float_t    fNpkmax;   // peak number max
   Int_t      fAmpbins;  // num amplitude bins
   Float_t    fAmpmin;   // amplitude min
   Float_t    fAmpmax;   // amplitude max
   Int_t      fdAmpbins; // num amplitude diff bins
   Float_t    fdAmpmin;  // amplitude diff min
   Float_t    fdAmpmax;  // amplitude diff max
   Int_t      fAmpRatbins;// num ratio bins
   Float_t    fAmpRatmin;// ratio min
   Float_t    fAmpRatmax;// ratio max
   Int_t      fPdPnSlpBins; // peak dist vs peak num slope bins
   Float_t    fPdPnSlpMin;  // peak dist vs peak num slope min
   Float_t    fPdPnSlpMax;  // peak dist vs peak num slope max
   
   // output plot storage
   TObjArray* fNpeaks;           //!
   TObjArray* fPkDistVsPkNum;    //!
   TObjArray* fPkDistVsPosn;     //!
   TObjArray* fPkAmpVsPosn;      //!
   TObjArray* fPkAmpVsPkNum;     //!
   TObjArray* fPkAmpDiffVsPosn;  //!
   TObjArray* fPkAmpDiffVsPkNum; //!
   TObjArray* fPkAmpRatioVsPosn; //!
   TObjArray* fPkAmpRatioVsPkNum;  //!
   TObjArray* fPkDistVsPkNumSlope; //!
   TObjArray* fPkDistVsPkNumSlopeVsNpks; //!
   TNtuple*   fNt;               //!
   
   void         MakePlots();
   void         CleanArrays();
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();
   virtual void SlaveTerminate();
   
 public:
   TSnPlotWvfmPeaksMod() : 
      fVsTime(kFALSE), fMakePeakTree(kFALSE),
      fNpkbins(25), fNpkmin(-0.5), fNpkmax(24.5),
      fAmpbins(140), fAmpmin(-350.0), fAmpmax(350.0),
      fdAmpbins(140), fdAmpmin(-350.0), fdAmpmax(350.0),
      fAmpRatbins(100), fAmpRatmin(0.0), fAmpRatmax(5.0),
      fPdPnSlpBins(301), fPdPnSlpMin(-150.5), fPdPnSlpMax(150.5),
      fNpeaks(0), fPkDistVsPkNum(0), fPkDistVsPosn(0),
      fPkAmpVsPosn(0), fPkAmpVsPkNum(0), 
      fPkAmpDiffVsPosn(0), fPkAmpDiffVsPkNum(0),
      fPkAmpRatioVsPosn(0), fPkAmpRatioVsPkNum(0),
      fPkDistVsPkNumSlope(0), fPkDistVsPkNumSlopeVsNpks(0), fNt(0) {
      // default ctor
   }
   TSnPlotWvfmPeaksMod(const Char_t* name,
                       const Char_t* amplitudeUnits,
                       const Bool_t  vsTime=kFALSE,
                       const Char_t* peakArrayName="DataPeaks",
                       const Char_t* plotSuffix=NULL) :
      TAModule(name, "make plots given result of TSnFindPeaksPastThreshMod"),
      fPkArNm(peakArrayName), fPlotSuf(plotSuffix),
      fAmpUnits(amplitudeUnits), fVsTime(vsTime),
      fMakePeakTree(kFALSE),
      fNpkbins(25), fNpkmin(-0.5), fNpkmax(24.5),
      fAmpbins(140), fAmpmin(-350.0), fAmpmax(350.0),
      fdAmpbins(140), fdAmpmin(-350.0), fdAmpmax(350.0),
      fAmpRatbins(100), fAmpRatmin(0.0), fAmpRatmax(5.0),
      fPdPnSlpBins(301), fPdPnSlpMin(-150.5), fPdPnSlpMax(150.5),
      fNpeaks(0), fPkDistVsPkNum(0), fPkDistVsPosn(0),
      fPkAmpVsPosn(0), fPkAmpVsPkNum(0), 
      fPkAmpDiffVsPosn(0), fPkAmpDiffVsPkNum(0),
      fPkAmpRatioVsPosn(0), fPkAmpRatioVsPkNum(0),
      fPkDistVsPkNumSlope(0), fPkDistVsPkNumSlopeVsNpks(0), fNt(0) {
      // normal ctor
   }
   virtual ~TSnPlotWvfmPeaksMod();
   
   TString GetPeaksArrayName() const { return fPkArNm; }
   TString GetPlotSuffix() const { return fPlotSuf; }
   TString GetAmplitudeUnits() const { return fAmpUnits; }
   Bool_t  IsUsingTimeAxis() const { return fVsTime; }
   Bool_t  IsMakingPeakPropsTree() const { return fMakePeakTree; }
   Int_t   GetNpeakBins() const { return fNpkbins; }
   Float_t GetNpeakMin() const { return fNpkmin; }
   Float_t GetNpeakMax() const { return fNpkmax; }
   Int_t   GetAmpBins() const { return fAmpbins; }
   Float_t GetAmpMin() const { return fAmpmin; }
   Float_t GetAmpMax() const { return fAmpmax; }
   Int_t   GetDeltaAmpBins() const { return fdAmpbins; }
   Float_t GetDeltaAmpMin() const { return fdAmpmin; }
   Float_t GetDeltaAmpMax() const { return fdAmpmax; }
   Int_t   GetAmpRatioBins() const { return fAmpRatbins; }
   Float_t GetAmpRatioMin() const { return fAmpRatmin; }
   Float_t GetAmpRatioMax() const { return fAmpRatmax; }
   Int_t   GetPkDistVsPkNumSlopeBins() const { return fPdPnSlpBins; }
   Float_t GetPkDistVsPkNumSlopeMin() const { return fPdPnSlpMin; }
   Float_t GetPkDistVsPkNumSlopeMax() const { return fPdPnSlpMax; }
   
   void    SetPeaksArrayName(const Char_t* n) { fPkArNm = n; }
   void    SetPlotSuffix(const Char_t* n) { fPlotSuf = n; }
   void    SetAmplitudeUnits(const Char_t* n) { fAmpUnits = n; }
   void    SetUseTimeAxis(const Bool_t x) { fVsTime = x; }
   void    SetMakePeakPropsTree(const Bool_t x) { fMakePeakTree = x; }
   void    SetNpeakBins(const Int_t n) { fNpkbins = n; }
   void    SetNpeakMin(const Float_t m) { fNpkmin = m; }
   void    SetNpeakMax(const Float_t m) { fNpkmax = m; }
   void    SetAmpBins(const Int_t n) { fAmpbins = n; }
   void    SetAmpMin(const Float_t m) { fAmpmin = m; }
   void    SetAmpMax(const Float_t m) { fAmpmax = m; }
   void    SetDeltaAmpBins(const Int_t n) { fdAmpbins = n; }
   void    SetDeltaAmpMin(const Float_t m) { fdAmpmin = m; }
   void    SetDeltaAmpMax(const Float_t m) { fdAmpmax = m; }
   void    SetAmpRatioBins(const Int_t n) { fAmpRatbins = n; }
   void    SetAmpRatioMin(const Float_t m) { fAmpRatmin = m; }
   void    SetAmpRatioMax(const Float_t m) { fAmpRatmax = m; }
   void    SetPkDistVsPkNumSlopeBins(const Int_t b) { fPdPnSlpBins = b; }
   void    SetPkDistVsPkNumSlopeMin(const Float_t m) { fPdPnSlpMin = m; }
   void    SetPkDistVsPkNumSlopeMax(const Float_t m) { fPdPnSlpMax = m; }
   
   ClassDef(TSnPlotWvfmPeaksMod, 2); // module to make plots given the result of TSnFindPeaksPastThreshMod
};

#endif // SNS_TSnPlotWvfmPeaksMod
