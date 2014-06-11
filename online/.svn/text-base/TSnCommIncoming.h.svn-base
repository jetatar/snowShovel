#ifndef SNS_TSnCommIncoming
#define SNS_TSnCommIncoming

#include <TObject.h>
class TTimeStamp;

class TSnStatusUpdate;

class TSnCommIncoming : public TObject {
 private:
   const TTimeStamp*      fTime;    // time of comm
   const TSnStatusUpdate* fStatus;  // status received with comm win
   Int_t                  fStIOvers;// status frame io version
   
 public:
   TSnCommIncoming() : fTime(0), fStatus(0), fStIOvers(0) {}
   TSnCommIncoming(const TTimeStamp& t, 
                   const TSnStatusUpdate& s,
                   const Int_t stvers);
   virtual ~TSnCommIncoming();

   const TTimeStamp*      GetTime() const { return fTime; }
   const TSnStatusUpdate* GetStatus() const { return fStatus; }
   Int_t                  GetStatusIOversion() const { return fStIOvers; }
   
   void  SetTime(const TTimeStamp& t);
   void  SetStatus(const TSnStatusUpdate& s);
   void  SetStatusIOversion(const Int_t s) { fStIOvers = s; }
   
   void  SetIncStatFrom(const TSnCommIncoming& i);
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TSnCommIncoming, 2); // base class of an incoming comm info packet
};

#endif // SNS_TSnCommIncoming
