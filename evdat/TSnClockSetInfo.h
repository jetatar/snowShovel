#ifndef SNS_TSnClockSetInfo
#define SNS_TSnClockSetInfo

#include <TObject.h>
#include <TTimeStamp.h>

class TSnEventHeader;

class TSnClockSetInfo : public TObject {
 private:
   TTimeStamp fPrevTime;   // mbed time before the clock was set
   TTimeStamp fSetTime;    // time the mbed clock was set to
   TTimeStamp fCurrTime;   // current time this clock info was built
   Int_t      fUSsinceSet; // us counter since the clock was set until fCurrTime (may have rolled over)
   
   mutable TTimeStamp* fAbsCache;  //! cache the absolute event time
   
 public:
   TSnClockSetInfo() : fUSsinceSet(0), fAbsCache(0) {}
   TSnClockSetInfo(const TSnClockSetInfo& o) :
      fPrevTime(o.fPrevTime),
      fSetTime(o.fSetTime),
      fCurrTime(o.fCurrTime),
      fUSsinceSet(o.fUSsinceSet),
      fAbsCache(0) {
      if (o.fAbsCache!=0) {
         fAbsCache = new TTimeStamp(*(o.fAbsCache));
      }
   }
   virtual ~TSnClockSetInfo() { delete fAbsCache; }
   
   TSnClockSetInfo& operator=(TSnClockSetInfo o) {
      // assign using efficient copy from pass by value
      Swap(*this, o);
      return *this;
   }
   
   static
   void              Swap(TSnClockSetInfo& a, TSnClockSetInfo& b);

   inline
   void ClearAbsCache() { delete fAbsCache; fAbsCache=0; }
   
   virtual void      Print(Option_t* option="") const;
   virtual void      Copy(TObject& object) const;
   
   const TTimeStamp& GetPrevTime() const { return fPrevTime; }
   const TTimeStamp& GetSetTime() const  { return fSetTime; }
   const TTimeStamp& GetCurrTime() const { return fCurrTime; }
   Int_t             GetUSsinceSet() const { return fUSsinceSet; }
   UInt_t            GetUSsinceSetU() const
      { return static_cast<UInt_t>(fUSsinceSet); }

   const TTimeStamp& CalcAbsCurrTime() const {
      if (fAbsCache==0) {
         fAbsCache = new TTimeStamp(
            CalcAbsTime(fSetTime, fCurrTime,
                        static_cast<UInt_t>(fUSsinceSet)) );
      }
      return *fAbsCache;
   }
   static
   TTimeStamp        CalcAbsTime(const TTimeStamp& ref,
                                 const TTimeStamp& cur,
                                 const UInt_t      dtus);
   static
   TTimeStamp        CalcAbsTime(const TTimeStamp& ref,
                                 const TSnEventHeader& hdr);
   static
   TTimeStamp        CalcAbsTime(const TSnClockSetInfo& ref,
                                 const TSnEventHeader& hdr) {
      return CalcAbsTime(ref.CalcAbsCurrTime(), hdr);
   }
   
   void              SetPrevTime(const TTimeStamp& t)
      { fPrevTime = t; ClearAbsCache(); }
   void              SetSetTime(const TTimeStamp& t)
      { fSetTime = t; ClearAbsCache(); }
   void              SetCurrTime(const TTimeStamp& t) 
      { fCurrTime = t; ClearAbsCache(); }
   void              SetUSsinceSet(const Int_t t) 
      { fUSsinceSet = t; ClearAbsCache(); }
   
      
   ClassDef(TSnClockSetInfo, 1); // store info about a clock setting
};

#endif // SNS_TSnClockSetInfo
