#ifndef SNS_TSnPlotEvtRateMod
#define SNS_TSnPlotEvtRateMod

#include "TAModule.h"

#include <TString.h>
#include <TTimeStamp.h>
class TGraph;
class TGraphErrors;
class TH1F;
class TH2F;

class TSnEventHeader;
class TSnClockSetInfo;
class TSnEventMetadata;

#include <vector>
#include <utility>

class TSnPlotEvtRateMod : public TAModule {
 private:
   // parameters
   Double_t      fTwin;        // time window: max time bin for rates, calc dt's within this window (s) (default 300s)
   Double_t      fConsDTbw;    // width of consecutive dt bins (s) (default 0.5s)
   Double_t      fCombDTbw;    // width of combinatorial dt bins (s) (default 0.1s)
   TString       fAbsTimeNm;   // name of the absolute event time in the evt objs (default: "AbsEvtTime")
   Bool_t        fSecResOk;    // if true, process events for which the absolute time is not available (default: true)
   Int_t         fNtimeBins;   // number of bins along time axis
   Double_t      fTimeMin;     // minimum of time axis (unix time seconds)
   Double_t      fTimeMax;     // maximum of time axis (unix time seconds)
   TString       fEnumVsTnm;   // name of fEnumVsT graph (default 'gEnumVsT')
   TString       fERvsTnm;     // name of fERvsT graph (default 'gERvsT')
   TString       fERvsEnumNm;  // name of fERvsEnum graph (default 'gERvsEnum')
   TString       fRawDTnm;     // name of fRawDT hist (default 'hRawDT')
   TString       fDTnm;        // name of fDT hist (default 'hConsDT')
   TString       fCombDTnm;    // name of fCombDT hist (default 'hCombDT')
   TString       fDTvsTnm;     // name of fDTvsT hist (default 'hConsDTvsT')
   TString       fDTvsTgrNm;   // name of fDTvsTgr graph (default 'gConsDTvsT')
   TString       fCombDTvsTnm; // name of fCombDTvsT hist (default 'hCombDTvsT')
   TString       fNumEvtsNm;   // name of fNumEvts hist (default 'hNumEvts')
   TString       fLivetimeNm;  // name of fNumEvts hist (default 'hLivetime')
   TString       fEvtNumsNm;   // name of fEvtNums hist (default 'hAveEvtNums')
   TString       fRateVsTnm;   // name of fRateVsT hist (default 'hRateVsT')

   // resulting plots
   TH1F*         fNumEvts;   // number of events in each time bin
   TH1F*         fLivetime;  // livetime in each time bin
   TH1F*         fEvtNums;   // sum of event numbers (for ave) in each bin
   TH1F*         fRateVsT;   // histogram of event rate vs time
   TGraph*       fEnumVsT;   // event number vs time
   TGraphErrors* fERvsT;     // event rate vs time
   TGraphErrors* fERvsEnum;  // event rate vs event number
   TH1F*         fRawDT;     // hist of event's DTms variables (cross check fDT)
   TH1F*         fDT;        // hist of consec-dt's
   TH1F*         fCombDT;    // hist of all dt combinations within the time window
   TH2F*         fDTvsT;     // hist of consec-dt vs time
   TGraph*       fDTvsTgr;   // graph of consec-dt vs time
   TH2F*         fCombDTvsT; // hist of combinatorial dt vs time

   // event data
   TSnEventHeader*   fHdr;      //! event header
   TSnEventMetadata* fMeta;     //! event metadata
   TSnEventMetadata* fPrevMeta; //! metadata of previous event
   TSnClockSetInfo*  fStop;     //! seq stop time (requires TSnConfigTreeLoader)

   // sortable array
   typedef UInt_t                             Num_t;
   typedef Double_t                           AbsTime_t;
   typedef std::pair< Num_t, AbsTime_t >      NumTime_t;
   typedef std::vector< NumTime_t >           VectNT_t;
   VectNT_t               fNTs; //! list of num/times from current file
   
   
   void ProcessTimes();
   
 protected:
   virtual void        Begin();
   virtual void        SlaveBegin();
   virtual void        Process();
   virtual void        SlaveTerminate();
   virtual void        Terminate();

 public:
   TSnPlotEvtRateMod() : fTwin(300), fConsDTbw(0.5), fCombDTbw(0.1),
                         fSecResOk(kTRUE), fNtimeBins(0), fTimeMin(0),
                         fTimeMax(0), fNumEvts(0), fLivetime(0), fEvtNums(0),
                         fRateVsT(0), fEnumVsT(0), fERvsT(0), fERvsEnum(0),
                         fRawDT(0), fDT(0), fCombDT(0), fDTvsT(0), fDTvsTgr(0),
                         fCombDTvsT(0) {}
   TSnPlotEvtRateMod(const Char_t* name);
   virtual ~TSnPlotEvtRateMod();
   
   Double_t      GetTwin() const { return fTwin; }
   Double_t      GetConsDTbw() const { return fConsDTbw; }
   Double_t      GetCombDTbw() const { return fCombDTbw; }
   const Char_t* GetAbsEvtTimeName() const { return fAbsTimeNm; }
   Bool_t        IsSecondResoOk() const { return fSecResOk; }
   Int_t         GetNtimeBins() const { return fNtimeBins; }
   Double_t      GetTimeMin() const { return fTimeMin; }
   Double_t      GetTimeMax() const { return fTimeMax; }
   const Char_t* GetEnumVsTName() const { return fEnumVsTnm.Data(); }
   const Char_t* GetERvsTName() const { return fERvsTnm.Data(); }
   const Char_t* GetERvsEnumName() const { return fERvsEnumNm.Data(); }
   const Char_t* GetRawDTName() const { return fRawDTnm.Data(); }
   const Char_t* GetConsDTName() const { return fDTnm.Data(); }
   const Char_t* GetCombDTName() const { return fCombDTnm.Data(); }
   const Char_t* GetConsDTvsTName() const { return fDTvsTnm.Data(); }
   const Char_t* GetConsDTvsTGraphName() const { return fDTvsTgrNm.Data(); }
   const Char_t* GetCombDTvsTName() const { return fCombDTvsTnm.Data(); }
   const Char_t* GetNumEvtsName() const { return fNumEvtsNm.Data(); }
   const Char_t* GetLivetimeName() const { return fLivetimeNm.Data(); }
   const Char_t* GetAveEvtNumsName() const { return fEvtNumsNm.Data(); }
   const Char_t* GetRateVsTName() const { return fRateVsTnm.Data(); }
   // no need for getters of the output histograms because that's provided
   // by the output list (TAMSelector::GetModOutput or TProof::GetOutput).
   
   void SetTwin(const Double_t t)              { fTwin = t; }
   void SetConsDTbw(const Double_t t)          { fConsDTbw = t; }
   void SetCombDTbw(const Double_t t)          { fCombDTbw = t; }
   void SetAbsEvtTimeName(const Char_t* n)     { fAbsTimeNm = n; }
   void SetSecondResoOk(const Bool_t k)        { fSecResOk = k; }
   void SetNtimeBins(const Int_t n)            { fNtimeBins = n; }
   void SetTimeMin(const Double_t t)           { fTimeMin = t; }
   void SetTimeMax(const Double_t t)           { fTimeMax = t; }
   void SetEnumVsTName(const Char_t* n)        { fEnumVsTnm = n; }
   void SetERvsTName(const Char_t* n)          { fERvsTnm = n; }
   void SetERvsEnumName(const Char_t* n)       { fERvsEnumNm = n; }
   void SetRawDTName(const Char_t* n)          { fRawDTnm = n; }
   void SetConsDTName(const Char_t* n)         { fDTnm = n; }
   void SetCombDTName(const Char_t* n)         { fCombDTnm = n; }
   void SetConsDTvsTName(const Char_t* n)      { fDTvsTnm = n; }
   void SetConsDTvsTGraphName(const Char_t* n) { fDTvsTgrNm = n; }
   void SetCombDTvsTName(const Char_t* n)      { fCombDTvsTnm = n; }
   void SetNumEvtsName(const Char_t* n)        { fNumEvtsNm = n; }
   void SetLivetimeName(const Char_t* n)       { fLivetimeNm = n; }
   void SetEvtNumsName(const Char_t* n)        { fEvtNumsNm = n; }
   void SetRateVsTName(const Char_t* n)        { fRateVsTnm = n; }
   
   // intentionally no setters for the output hists
   
   ClassDef(TSnPlotEvtRateMod, 3); // make event rate and dt plots (no evt sel)

};

#endif // SNS_TSnPlotEvtRateMod
