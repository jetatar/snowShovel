#ifndef SNS_TSnRunInfo
#define SNS_TSnRunInfo

#include <TNamed.h>
#include <TString.h>

class TSnRunInfo : public TNamed {
 public:
   struct TObsoletePars {
      // old parameters we don't usually want to stream, but we may need
      // for backwards compatibility
      UInt_t         fFirstEvt;
      
      TObsoletePars() : fFirstEvt(0) {}
      virtual ~TObsoletePars() {}
      ClassDef(TSnRunInfo::TObsoletePars, 1); // run info backward compat layer
   };

 private:
   TString        fConfLabel;
   TString        fStationLabel;
   UInt_t         fRun;
   UShort_t       fFirstSeq;
   UInt_t         fEvtsPerSeq;
   
   TSnRunInfo::TObsoletePars fObs;    // backward compatibility layer
   Bool_t                    fHasObs; // if back compat variables set
   
   void ResetName();

   // intentionally private
   const TSnRunInfo::TObsoletePars& GetObsPars() const;
   
 public:
   TSnRunInfo() :
      fRun(0), fFirstSeq(0), fEvtsPerSeq(1000), fHasObs(kFALSE) {}
   TSnRunInfo(const Char_t* conflbl, const Char_t* stnlbl,
              const UInt_t run, const UShort_t firstSeq,
              const UInt_t evtsPerSeq) :
      TNamed(conflbl, "run info"),
      fConfLabel(conflbl), fStationLabel(stnlbl),
      fRun(run), fFirstSeq(firstSeq), fEvtsPerSeq(evtsPerSeq),
      fHasObs(kFALSE) {}
   virtual ~TSnRunInfo() { }
   
   Bool_t         IsValidForDb() const;
   void	          Print(Option_t* option = "") const;
   virtual Bool_t IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const;
   
   const Char_t*  GetConfLabel() const { return fConfLabel.Data(); }
   const Char_t*  GetStationLabel() const { return fStationLabel.Data(); }
   UInt_t         GetRun() const { return fRun; }
   UShort_t       GetFirstSeq() const { return fFirstSeq; }
   UInt_t         GetEvtsPerSeq() const { return fEvtsPerSeq; }
   
   void           SetConfLabel(const Char_t* c) 
      { fConfLabel = c; ResetName(); }
   void           SetStationLabel(const Char_t* s) 
      { fStationLabel = s; ResetName(); }
   void           SetRun(const UInt_t r) { fRun = r; ResetName(); }
   void           SetFirstSeq(const UShort_t e) { fFirstSeq = e; }
   void           SetEvtsPerSeq(const UInt_t s) { fEvtsPerSeq = s; }

   Bool_t         HasObsParams() const { return fHasObs; }
   UInt_t         GetObsFirstEvt() const { return GetObsPars().fFirstEvt; }
   void           SetObsFirstEvt(const UInt_t f);
   
   ClassDef(TSnRunInfo, 6); // run parameters
   
};

#endif // SNS_TSnRunInfo
