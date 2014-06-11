#ifndef SNS_TSnHeartbeat
#define SNS_TSnHeartbeat

#include <TObject.h>
#include <TTimeStamp.h>

class TSnHeartbeat : public TObject {
 private:
   UInt_t      fNum;   // heartbeat number
   TTimeStamp  fTime;  // time of heartbeat
   
 public:
   TSnHeartbeat() : fNum(0) {}
   TSnHeartbeat(const UInt_t n, const TTimeStamp& t) :
      fNum(n), fTime(t) {}
   virtual ~TSnHeartbeat() {}
   
   UInt_t            GetNum() const { return fNum; }
   const TTimeStamp& GetTime() const { return fTime; }
         TTimeStamp& GetTime()       { return fTime; }
   UInt_t            GetUnixTime() const { return fTime.GetSec(); }
   
   void SetNum(const UInt_t n) { fNum = n; }
   void SetTime(const TTimeStamp& t) { fTime = t; }
   void SetUnixTime(const UInt_t t) { fTime.Set(t,kTRUE,0,kFALSE); }
   
   ClassDef(TSnHeartbeat, 1); // store a heartbeat signal
};

#endif // SNS_TSnHeartbeat
