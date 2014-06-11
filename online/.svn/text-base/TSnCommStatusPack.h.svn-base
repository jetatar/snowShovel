#ifndef SNS_TSnCommStatusPack
#define SNS_TSnCommStatusPack

#include <TTimeStamp.h>

#include "TSnCommObject.h"
class TSnStatusUpdate;
class TSnPowerReading;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommStatusPack
//
// Class describing a complete status update sent by a station.
// It is composed of a TSnStatusUpdate, a TSnPowerReading and a TTimeStamp.
//
// This container owns its config components.
//
////////////////////////////////////////////////////////////////////////////////

class TSnCommStatusPack : public TSnCommObject {
 private:
   TSnStatusUpdate* fStatus; // the status that just arrived
   TSnPowerReading* fPower;  // the voltage that just arrived
   TTimeStamp       fTime;   // the time of arrival
   Int_t           fStIOvers;// defaults to -1 => use default version of TSnIOStatusFrame other values allow back compat
   Int_t           fPwIOvers;// defaults to -1 => use default version of TSnIOPowerFrame other values allow back compat
   
 protected:
   TSnCommStatusPack(const TSnCommStatusPack& s);
   
 public:
   TSnCommStatusPack() : fStatus(0), fPower(0), fStIOvers(-1) {}
   TSnCommStatusPack(const TSnStatusUpdate& s,
                     const TSnPowerReading& p,
                     const TTimeStamp& t);
   virtual ~TSnCommStatusPack();
   
   const TSnStatusUpdate* GetStatus() const { return fStatus; }
         TSnStatusUpdate* GetStatus()       { return fStatus; }
   const TSnPowerReading* GetPowerReading() const { return fPower; }
         TSnPowerReading* GetPowerReading()       { return fPower; }
   const TTimeStamp&      GetTime() const { return fTime; }
         TTimeStamp&      GetTime()       { return fTime; }

   void ReadStatusFrom(const TSnIOBuffer& b);
   void ReadPowerFrom(const TSnIOBuffer& b);
   void SetTime(const TTimeStamp& s);

   UChar_t        GetStatusIOvers() const;
   Int_t          GetStIOvers() const { return fStIOvers; }
   void           SetStIOvers(const Int_t v) { fStIOvers = v; }
   UChar_t        GetPowerIOvers() const;
   Int_t          GetPwIOvers() const { return fStIOvers; }
   void           SetPwIOvers(const Int_t v) { fStIOvers = v; }
   
   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const;
   
   virtual Bool_t  IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const;

   virtual TSnCommObject* NewClone() const
      { return new TSnCommStatusPack(*this); }
   
   virtual TString GetLabel() const;
   
   virtual void   Print(Option_t *option="") const;
   
   
   ClassDef(TSnCommStatusPack, 2); // status info coming from a station
};

#endif // SNS_TSnCommStatusPack
