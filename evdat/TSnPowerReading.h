#ifndef SNS_TSnPowerReading
#define SNS_TSnPowerReading

#include <TObject.h>
#include <TTimeStamp.h>

class TSnPowerReading : public TObject {
 public:
   struct TObsoletePars {
      // old parameters we don't usually want to stream, but we may need
      // for backwards compatibility
      UShort_t    fVadc1;
      UShort_t    fVadc2;
      
      TObsoletePars() : fVadc1(0), fVadc2(0) {}
      virtual ~TObsoletePars() {}
      ClassDef(TSnPowerReading::TObsoletePars, 1); // pwr back compat layer
   };
   
 private:
   Float_t     faveV1;
   Float_t     faveV2;
   Float_t     frmsV1;
   Float_t     frmsV2;
   TTimeStamp  fTime;
   
   TSnPowerReading::TObsoletePars fObs;    // pwr back compat layer
   Bool_t                         fHasObs; // if back compat vars set
   
   // intentionally private
   const TSnPowerReading::TObsoletePars& GetObsPars() const;
   
 public:
   TSnPowerReading(const Float_t av1=0, const Float_t av2=0,
                   const Float_t rm1=0, const Float_t rm2=0,
                   const TTimeStamp* time=0) :
      faveV1(av1), faveV2(av2), frmsV1(rm1), frmsV2(rm2),
      fHasObs(kFALSE) {
      if (time!=0) {
         fTime = *time;
      }
   }
   virtual ~TSnPowerReading() { }
   
   virtual void      Print(Option_t* option="") const;

   Float_t     GetAveV1() const { return faveV1; }
   Float_t     GetAveV2() const { return faveV2; }
   Float_t     GetRmsV1() const { return frmsV1; }
   Float_t     GetRmsV2() const { return frmsV2; }
   const TTimeStamp& GetTimeStamp() const { return fTime; }
         TTimeStamp& GetTimeStamp()       { return fTime; }
   UInt_t      GetTime() const { return fTime.GetSec(); }

   Float_t     GetCalVolt(const Float_t vgain=6.0649e-4) const;
   Float_t     GetCalCurr(const Float_t cped=1.112,
                          const Float_t cgain=-2.6828e-5) const;
   
   void        SetAveV1(const Float_t v) { faveV1 = v; }
   void        SetAveV2(const Float_t v) { faveV2 = v; }
   void        SetRmsV1(const Float_t r) { frmsV1 = r; }
   void        SetRmsV2(const Float_t r) { frmsV2 = r; }
   void        SetTime(const TTimeStamp& time) { fTime = time; }
   void        SetTime(const UInt_t time) { fTime.Set(time,kTRUE,0,kFALSE); }
   void        Set(const Float_t av1, const Float_t av2,
                   const Float_t rm1, const Float_t rm2,
                   const TTimeStamp& time) {
      faveV1 = av1; faveV2 = av2;
      frmsV1 = rm1; frmsV2 = rm2;
      fTime = time;
   }
   void        Set(const Float_t av1, const Float_t av2,
                   const Float_t rm1, const Float_t rm2,
                   const UInt_t& time) {
      faveV1 = av1; faveV2 = av2;
      frmsV1 = rm1; frmsV2 = rm2;
      fTime.Set(time,kTRUE,0,kFALSE);
   }

   virtual
   void Copy(TObject& object) const;


   Bool_t   HasObsParams() const { return fHasObs; }
   UShort_t GetObsVadc1() const;
   UShort_t GetObsVadc2() const;
   void     SetObsVadc1(const UShort_t v);
   void     SetObsVadc2(const UShort_t v);
   
   ClassDef(TSnPowerReading, 5); // store power reading from station motherboard
};

#endif // SNS_TSnPowerReading
