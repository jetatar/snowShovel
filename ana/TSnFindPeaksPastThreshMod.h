#ifndef SNS_TSnFindPeaksPastThreshMod
#define SNS_TSnFindPeaksPastThreshMod

#include <TString.h>
class TGraph;

#include "TAModule.h"

class TSnInterp1DLstSqConvo;

class TSnFindPeaksPastThreshMod : public TAModule {
 public:
   static const UInt_t kDebugPlots;    // verb level to make debug plots
   static const UInt_t kDebugPeakFind; // verb level for peak find prints

 private:
   TString    fIntpArNm; // name of TObjArray containing the interpolated data
   TString    fDervArNm; // name of TObjArray containing the interpolated derivative of the data
   TString    fPkArNm;   // name of resulting TObjArray of TGraph's of peak positions to add to the event (default: DataPeaks)
   TString    fCrsArNm;  // name of resulting TObjArray of TGraph's of threshold crossings to add to the event (default: DataThrCrs)
   Float_t    fHiThresh; // threshold on high side (default: 60)
   Float_t    fLoThresh; // threshold on low side (default: -60)
   Bool_t     fDoHiThr;  // whether to look for peaks above high threshold (default: true)
   Bool_t     fDoLoThr;  // whether to look for peaks below low threshold (default: false)


   void         CheckPeak(const Double_t x, const Double_t y,
                          const Double_t ax, const Double_t ay,
                          const Double_t thresh,
                          Double_t& ris, Double_t& fal,
                          Bool_t& isok,
                          const TSnInterp1DLstSqConvo* dat,
                          const TSnInterp1DLstSqConvo* der,
                          TGraph* gCrs, TGraph* gPeaks) const;
   
 protected:
   virtual void Begin();
   virtual void Process();

 public:
   TSnFindPeaksPastThreshMod() : fHiThresh(60.0), fLoThresh(-60.0),
                                 fDoHiThr(kTRUE), fDoLoThr(kFALSE) {
      // default ctor
   }
   TSnFindPeaksPastThreshMod(const Char_t* name,
                             const Char_t* intpArNm,
                             const Char_t* derivArNm,
                             const Char_t* peakGraphArNm="DataPeaks",
                             const Char_t* crossGraphArNm="DataThrCrs",
                             const Float_t hiThr=60.0,
                             const Float_t loThr=-60.0,
                             const Bool_t  doHiThr=kTRUE,
                             const Bool_t  doLoThr=kFALSE) :
      TAModule(name, "Find peaks in a least square convolution fit"),
      fIntpArNm(intpArNm), fDervArNm(derivArNm),
      fPkArNm(peakGraphArNm), fCrsArNm(crossGraphArNm),
      fHiThresh(hiThr), fLoThresh(loThr), 
      fDoHiThr(doHiThr), fDoLoThr(doLoThr) {
      // normal ctor
   }
   virtual ~TSnFindPeaksPastThreshMod() {}
   
   TString GetInterpDataArrayName() const { return fIntpArNm; }
   TString GetDerivDataArrayName() const { return fDervArNm; }
   TString GetPeaksArrayName() const { return fPkArNm; }
   TString GetThreshCrossArrayName() const { return fCrsArNm; }
   Float_t GetHighThreshold() const { return fHiThresh; }
   Float_t GetLowThreshold() const { return fLoThresh; }
   Bool_t  IsFindingHighSidePeaks() const { return fDoHiThr; }
   Bool_t  IsFindingLowSidePeaks() const { return fDoLoThr; }
   
   
   void    SetInterpDataArrayName(const Char_t* n) { fIntpArNm = n; }
   void    SetDerivDataArrayName(const Char_t* n) { fDervArNm = n; }
   void    SetPeakArrayName(const Char_t* n) { fPkArNm = n; }
   void    SetThreshCrossArrayName(const Char_t* n) { fCrsArNm = n; }
   void    SetHighThreshold(const Float_t t) { fHiThresh = t; }
   void    SetLowThreshold(const Float_t t) { fLoThresh = t; }
   void    SetFindHighSidePeaks(const Bool_t x) { fDoHiThr = x; }
   void    SetFindLowSidePeaks(const Bool_t x) { fDoLoThr = x; }
   
   
   
   ClassDef(TSnFindPeaksPastThreshMod, 1); // find position of peaks past a threshold (high or low). uses TSnInterp1DLstSqConvo.
};

#endif // SNS_TSnFindPeaksPastThreshMod
