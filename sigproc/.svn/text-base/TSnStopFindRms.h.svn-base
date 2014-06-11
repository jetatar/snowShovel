#ifndef SNS_TSnStopFindRms
#define SNS_TSnStopFindRms

#include <TString.h>

#include "TAModule.h"

class TSnCalWvData;
class TSnStopPosnSet;

class TSnStopFindRms : public TAModule {
 private:
   TString       fDatBrNm;  // name of the input branch containing the data (fpn sub or amp out)
   TString       fStopNm;   // name of the output stop position object (default: StopPositions)
   UShort_t      fStopWid;  // width of each stop in number of samples (default: 6)
   UShort_t      fSrchWid;  // width of each stop search window in number of samples. must be <=fStopWid. if !=fStopWid, the stop in each channel is allowed to shift independently by (fStopWid-fSrchWid) (default: 4)
   UShort_t      fJitter;   // max number of samples the stop may differ between channels (default: 3)
   
   // event data
   TSnCalWvData* fDat; //! the data
   

   Bool_t GetRmsInStop(const UChar_t ch,
                       const UChar_t startSample,
                       const UChar_t smpwid,
                       Float_t& rms) const;
   Bool_t FindStopInRange(TSnStopPosnSet& stops,
                          Float_t& minAveRms,
                          const UChar_t chan,
                          const UChar_t beginSmp,
                          const UChar_t endSmp) const;
   void FindStopByRms(TSnStopPosnSet& stops) const;
   
 protected:
   virtual void SlaveBegin();
   virtual void Process();

 public:
   TSnStopFindRms() : fStopWid(6), fSrchWid(4), fJitter(3), fDat(0) {}
   TSnStopFindRms(const Char_t* name,
                  const Char_t* datBrNm,
                  const UShort_t jitter=3,
                  const Char_t* stopNm="StopPositions",
                  const UShort_t stopWid=6,
                  const UShort_t searchWid=4) :
      TAModule(name, "find stops using low rms sample groups"),
      fDatBrNm(datBrNm), fStopNm(stopNm), 
      fStopWid(stopWid), fSrchWid(searchWid),
      fJitter(jitter),
      fDat(0) {
      // normal ctor
   }
   virtual ~TSnStopFindRms() {}
   
   
   TString  GetDataBranchName() const { return fDatBrNm; }
   TString  GetStopPosnName() const { return fStopNm; }
   UShort_t GetStopWidth() const { return fStopWid; }
   UShort_t GetSearchWidth() const { return fSrchWid; }
   UShort_t GetMaxJitter() const { return fJitter; }
   
   void     SetDataBranchName(const Char_t* n) { fDatBrNm = n; }
   void     SetStopPosnName(const Char_t* n) { fStopNm = n; }
   void     SetStopWidth(const UShort_t s) { fStopWid = s; }
   void     SetSearchWidth(const UShort_t s) { fSrchWid = s; }
   void     SetMaxJitter(const UShort_t j) { fJitter = j; }
   
   ClassDef(TSnStopFindRms, 2); // try to find the stop by looking for low RMS in known slot locations
};

#endif // SNS_TSnStopFindRms
