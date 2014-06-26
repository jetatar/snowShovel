#ifndef SNS_TSnRejectBadClocksMod
#define SNS_TSnRejectBadClocksMod

class TTimeStamp;

#include <TAModule.h>

class TSnEventHeader;

class TSnRejectBadClocksMod : public TAModule {
 public:
   typedef std::pair<Double_t, Double_t> TimeWin_t;
   typedef std::vector< TimeWin_t >      VecTimeWin_t;
   
   static const UInt_t kPrintRejected; // verbosity level at which to print why events are rejected
   
 private:
   VecTimeWin_t  fTimeWins;    // time windows on which to cut. a vector of pairs, where each pair is the min/max of unix time.nanosecond (default: keep events in Jan 2010 - Jan 2017)
   Bool_t        fRejOutliers; // if true, reject events outside all time window(s). if false, reject events inside any one time windows(s). events at the edge are counted as inside. (default: true)

   // event objs
   TSnEventHeader* fHdr;   //! event header

 protected:
   virtual void  SlaveBegin();
   virtual void  Process();

   void          CheckTime(const Double_t t);

 public:
   TSnRejectBadClocksMod() : 
      fRejOutliers(kTRUE),
      fHdr(0) {}
   TSnRejectBadClocksMod(const Char_t* name,
                         const Bool_t addDefaultTwin=kTRUE,
                         const Bool_t rejectOutliers=kTRUE) :
      TAModule(name, "Reject events with unreasonable times"),
      fRejOutliers(rejectOutliers),
      fHdr(0) {
      // normal constructor
      if (addDefaultTwin) {
         AddTimeWindow(1262304000.0, 1483228800.0);
      }
   }
   virtual ~TSnRejectBadClocksMod() {}

   virtual void     Print(Option_t* option = "") const;
   
   UInt_t           GetNumTimeWins() const { return fTimeWins.size(); }
   const TimeWin_t& GetTimeWin(const UInt_t i) const { return fTimeWins.at(i); }
         TimeWin_t& GetTimeWin(const UInt_t i)       { return fTimeWins.at(i); }
   Bool_t           IsRejectingOutsideAllTimeWins() const 
      { return fRejOutliers; }
   Bool_t           IsRejectingInsideAnyTimeWin() const
      { return !fRejOutliers; }

   Double_t         GetMinTime(const UInt_t i) const
      { return fTimeWins.at(i).first; }
   Double_t         GetMaxTime(const UInt_t i) const
      { return fTimeWins.at(i).second; }
   
   
   void             ChangeMinTime(const UInt_t i, const Double_t t) 
      { fTimeWins.at(i).first = t; }
   void             ChangeMaxTime(const UInt_t i, const Double_t t)
      { fTimeWins.at(i).second = t; }
   void             SetRejectOutsideAllTimeWins() { fRejOutliers = kTRUE; }
   void             SetRejectInsideAnyTimeWin() { fRejOutliers = kFALSE; }
   
   void             AddTimeWindow(const TimeWin_t& twin)
      { fTimeWins.push_back(twin); }
   void             AddTimeWindow(const Double_t min, const Double_t max)
      { AddTimeWindow( TimeWin_t(min, max) ); }
   
   
   ClassDef(TSnRejectBadClocksMod,2); // reject events with unreasonable times
};

#endif // SNS_TSnRejectBadClocksMod
