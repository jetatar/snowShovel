#include "TSnCommIncoming.h"

#include <TString.h>
#include <TTimeStamp.h>

#include "TSnStatusUpdate.h"

ClassImp(TSnCommIncoming);

TSnCommIncoming::TSnCommIncoming(const TTimeStamp& t, 
                                 const TSnStatusUpdate& s,
                                 const Int_t stvers) :
   fTime(new TTimeStamp(t)), fStatus(new TSnStatusUpdate(s)),
   fStIOvers(stvers) {
   // copy the timestamp and status to this incoming obj
}

TSnCommIncoming::~TSnCommIncoming() {
   delete fTime;
   delete fStatus;
}

void TSnCommIncoming::SetTime(const TTimeStamp& t) {
   delete fTime;
   fTime = new TTimeStamp(t);
}

void TSnCommIncoming::SetStatus(const TSnStatusUpdate& s) {
   delete fStatus;
   fStatus = new TSnStatusUpdate(s);
}

void TSnCommIncoming::SetIncStatFrom(const TSnCommIncoming& i) {
   if (i.GetTime()!=0) {
      SetTime(*(i.GetTime()));
   } else {
      delete fTime; fTime=0;
   }
   if (i.GetStatus()!=0) {
      SetStatus(*(i.GetStatus()));
   } else {
      delete fStatus; fStatus=0;
   }
   SetStatusIOversion( i.GetStatusIOversion() );
}

void TSnCommIncoming::Print(Option_t* option) const {
   TObject::Print(option);
   Printf("Timestamp (%p):",static_cast<const void*>(fTime));
   if (fTime!=0) {
      fTime->Print(option);
   }
   Printf("Status (%p):",static_cast<const void*>(fStatus));
   if (fStatus!=0) {
      fStatus->Print(option);
   }
   Printf("Status IO version: %d", fStIOvers);
}
